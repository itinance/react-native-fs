// Copyright (C) Microsoft Corporation. All rights reserved.

#include "pch.h"

#include "RNFSManager.h"

#include <filesystem>
#include <sstream>
#include <stack>
#include <windows.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Headers.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Storage::Streams;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Web::Http;


union touchTime {
    int64_t initialTime;
    DWORD splitTime[2];
};

//
// For downloads and uploads
//
CancellationDisposable::CancellationDisposable(IAsyncInfo const& async, std::function<void()>&& onCancel) noexcept
    : m_async{ async }
    , m_onCancel{ std::move(onCancel) }
{
}

CancellationDisposable::CancellationDisposable(CancellationDisposable&& other) noexcept
    : m_async{ std::move(other.m_async) }
    , m_onCancel{ std::move(other.m_onCancel) }
{
}

CancellationDisposable& CancellationDisposable::operator=(CancellationDisposable&& other) noexcept
{
    if (this != &other)
    {
        CancellationDisposable temp{ std::move(*this) };
        m_async = std::move(other.m_async);
        m_onCancel = std::move(other.m_onCancel);
    }
    return *this;
}

CancellationDisposable::~CancellationDisposable() noexcept
{
    Cancel();
}

void CancellationDisposable::Cancel() noexcept
{
    if (m_async)
    {
        if (m_async.Status() == AsyncStatus::Started)
        {
            m_async.Cancel();
        }

        if (m_onCancel)
        {
            m_onCancel();
        }
    }
}

TaskCancellationManager::~TaskCancellationManager() noexcept
{
    // Do the explicit cleaning to make sure that CancellationDisposable
    // destructors run while this instance still has valid fields because
    // they are used by the onCancel callback.
    // We also want to clear the m_pendingTasks before running the
    // CancellationDisposable destructors since they touch the m_pendingTasks.
    std::map<JobId, CancellationDisposable> pendingTasks;
    {
        std::scoped_lock lock{ m_mutex };
        pendingTasks = std::move(m_pendingTasks);
    }
}

IAsyncAction TaskCancellationManager::Add(JobId jobId, IAsyncAction const& asyncAction) noexcept
{
    std::scoped_lock lock{ m_mutex };
    m_pendingTasks.try_emplace(jobId, asyncAction, [this, jobId]()
        {
            Cancel(jobId);
        });
    return asyncAction;
}

void TaskCancellationManager::Cancel(JobId jobId) noexcept
{
    // The destructor of the token does the cancellation. We must do it outside of lock.
    CancellationDisposable token;

    {
        std::scoped_lock lock{ m_mutex };
        if (!m_pendingTasks.empty())
        {
            if (auto it = m_pendingTasks.find(jobId); it != m_pendingTasks.end())
            {
                token = std::move(it->second);
                m_pendingTasks.erase(it);
            }
        }
    }
}

//
// For stat implementation
//
struct handle_closer
{
    void operator()(HANDLE h) noexcept
    {
        assert(h != INVALID_HANDLE_VALUE); if (h) CloseHandle(h);
    }
};

static inline HANDLE safe_handle(HANDLE h) noexcept
{
    return (h == INVALID_HANDLE_VALUE) ? nullptr : h;
}

void RNFSManager::Initialize(RN::ReactContext const& reactContext) noexcept
{
    m_reactContext = reactContext;
}


//
// RNFS implementations
//
void RNFSManager::ConstantsViaConstantsProvider(RN::ReactConstantProvider& constants) noexcept
{
    // RNFS.MainBundlePath
    constants.Add(L"RNFSMainBundlePath", to_string(Package::Current().InstalledLocation().Path()));

    // RNFS.CachesDirectoryPath
    constants.Add(L"RNFSCachesDirectoryPath", to_string(ApplicationData::Current().LocalCacheFolder().Path()));

    // RNFS.ExternalCachesDirectoryPath - NULL in iOS (No equivalent in Windows)

    // RNFS.DocumentDirectoryPath
    constants.Add(L"RNFSDocumentDirectoryPath", to_string(ApplicationData::Current().LocalFolder().Path()));

    // RNFS.DownloadDirectoryPath - IMPLEMENT for convenience? (absent in iOS and deprecated in Android)
    constants.Add(L"RNFSDownloadDirectoryPath", UserDataPaths::GetDefault().Downloads());

    // RNFS.ExternalDirectoryPath - NULL in iOS (Pending use case in Windows and deprecated in Android)
    constants.Add(L"RNFSExternalDirectoryPath", UserDataPaths::GetDefault().Documents());

    // RNFS.ExternalStorageDirectoryPath - NULL in iOS (Pending use case in Windows and deprecated in Android)

    // RNFS.TemporaryDirectoryPath
    constants.Add(L"RNFSTemporaryDirectoryPath", to_string(ApplicationData::Current().TemporaryFolder().Path()));

    // RNFS.LibraryDirectoryPath - NULL in Android (No equivalent in Windows)

    // RNFS.PicturesDirectoryPath - IMPLEMENT for convenience? (absent in iOS though and deprecated in Android)
    constants.Add(L"RNFSPicturesDirectoryPath", UserDataPaths::GetDefault().Pictures());

    // RNFS.FileProtectionKeys - NULL in Android (No equivalent in Windows)

    // TODO: Check to see if these can be accessed after package created
    // Needed for synchronization across Windows devices
    constants.Add(L"RNFSRoamingDirectoryPath", to_string(ApplicationData::Current().RoamingFolder().Path()));

    // Filetypes
    constants.Add(L"RNFSFileTypeRegular", 0);
    constants.Add(L"RNFSFileTypeDirectory", 1);
}

winrt::fire_and_forget RNFSManager::mkdir(std::string directory, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    size_t pathLength{ directory.length() };

    if (pathLength <= 0) {
        promise.Reject("Invalid path length");
    }
    else {
        bool hasTrailingSlash{ directory[pathLength - 1] == '\\' || directory[pathLength - 1] == '/' };
        std::filesystem::path path(hasTrailingSlash ? directory.substr(0, pathLength - 1) : directory);
        path.make_preferred();

        auto parentPath{ path.parent_path().wstring() };
        std::stack<std::wstring> directoriesToMake;
        directoriesToMake.push(path.filename().wstring());

        StorageFolder folder{ nullptr };
        while (folder == nullptr) {
            try {
                folder = co_await StorageFolder::GetFolderFromPathAsync(parentPath);
            }
            catch (const hresult_error& ex) {
                hresult result{ ex.code() };
                if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
                    auto index{ parentPath.find_last_of('\\') };
                    directoriesToMake.push(parentPath.substr(index + 1));
                    parentPath = parentPath.substr(0, index);
                }
                else {
                    promise.Reject(winrt::to_string(ex.message()).c_str());
                }
            }
        }

        while (!directoriesToMake.empty()) {
            folder = co_await folder.CreateFolderAsync(directoriesToMake.top(), CreationCollisionOption::OpenIfExists);
            directoriesToMake.pop();
        }
        promise.Resolve();
    }
}
catch (const hresult_error& ex)
{
    // "Unexpected error while making directory."
    promise.Reject( winrt::to_string(ex.message()).c_str() );
}


winrt::fire_and_forget RNFSManager::moveFile(std::string filepath, std::string destpath, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring srcDirectoryPath, srcFileName;
    splitPath(filepath, srcDirectoryPath, srcFileName);

    winrt::hstring destDirectoryPath, destFileName;
    splitPath(destpath, destDirectoryPath, destFileName);

    StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(srcDirectoryPath) };
    StorageFolder destFolder{ co_await StorageFolder::GetFolderFromPathAsync(destDirectoryPath) };
    StorageFile file{ co_await srcFolder.GetFileAsync(srcFileName) };

    co_await file.MoveAsync(destFolder, destFileName, NameCollisionOption::ReplaceExisting);

    promise.Resolve();
}
catch (const hresult_error& ex)
{
    // "Failed to move file."
    promise.Reject(winrt::to_string(ex.message()).c_str());
}


winrt::fire_and_forget RNFSManager::copyFile(std::string filepath, std::string destpath, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring srcDirectoryPath, srcFileName;
    splitPath(filepath, srcDirectoryPath, srcFileName);

    winrt::hstring destDirectoryPath, destFileName;
    splitPath(destpath, destDirectoryPath, destFileName);

    StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(srcDirectoryPath) };
    StorageFolder destFolder{ co_await StorageFolder::GetFolderFromPathAsync(destDirectoryPath) };
    StorageFile file{ co_await srcFolder.GetFileAsync(srcFileName) };

    co_await file.CopyAsync(destFolder, destFileName, NameCollisionOption::ReplaceExisting);

    promise.Resolve();
}
catch (const hresult_error& ex)
{
    // "Failed to copy file."
    promise.Reject(winrt::to_string(ex.message()).c_str());
}


winrt::fire_and_forget RNFSManager::copyFolder(
    std::string srcFolderPath,
    std::string destFolderPath,
    RN::ReactPromise<void> promise) noexcept
try
{
    std::filesystem::path srcPath{ srcFolderPath };
    srcPath.make_preferred();
    std::filesystem::path destPath{ destFolderPath };
    destPath.make_preferred();

    StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(winrt::to_hstring(srcPath.c_str())) };
    StorageFolder destFolder{ co_await StorageFolder::GetFolderFromPathAsync(winrt::to_hstring(destPath.c_str())) };

    auto items{ co_await srcFolder.GetItemsAsync() };
    for (auto item : items)
    {
        if (item.IsOfType(StorageItemTypes::File))
        {
            StorageFile file{ co_await StorageFile::GetFileFromPathAsync(item.Path()) };
            co_await file.CopyAsync(destFolder, file.Name(), NameCollisionOption::ReplaceExisting);
        }
        else if (item.IsOfType(StorageItemTypes::Folder))
        {
            StorageFolder src{ co_await StorageFolder::GetFolderFromPathAsync(item.Path()) };
            StorageFolder dest{ co_await destFolder.CreateFolderAsync(item.Name(), CreationCollisionOption::OpenIfExists) };
            copyFolderHelper(src, dest);
        }
    }

    promise.Resolve();
    co_return;
}
catch (const hresult_error& ex)
{
    // "Failed to copy file."
    promise.Reject(winrt::to_string(ex.message()).c_str());
}

winrt::fire_and_forget RNFSManager::copyFolderHelper(
    winrt::Windows::Storage::StorageFolder src,
    winrt::Windows::Storage::StorageFolder dest) noexcept
try
{
    auto items{ co_await src.GetItemsAsync() };
    for (auto item : items)
    {
        if (item.IsOfType(StorageItemTypes::File))
        {
            StorageFile file{ co_await StorageFile::GetFileFromPathAsync(item.Path()) };
            co_await file.CopyAsync(dest, file.Name(), NameCollisionOption::ReplaceExisting);
        }
        else if (item.IsOfType(StorageItemTypes::Folder))
        {
            StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(item.Path()) };
            StorageFolder destFolder{ co_await dest.CreateFolderAsync(item.Name(), CreationCollisionOption::OpenIfExists) };
            copyFolderHelper(srcFolder, destFolder);
        }
    }
}
catch (...)
{
    co_return;
}


winrt::fire_and_forget RNFSManager::getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    auto localFolder{ Windows::Storage::ApplicationData::Current().LocalFolder() };
    auto properties{ co_await localFolder.Properties().RetrievePropertiesAsync({L"System.FreeSpace", L"System.Capacity"}) };

    RN::JSValueObject result;
    result["freeSpace"] = unbox_value<uint64_t>(properties.Lookup(L"System.FreeSpace"));
    result["totalSpace"] = unbox_value<uint64_t>(properties.Lookup(L"System.Capacity"));

    promise.Resolve(result);
}
catch (const hresult_error& ex)
{
    // "Failed to retrieve file system info."
    promise.Reject(winrt::to_string(ex.message()).c_str());
}


winrt::fire_and_forget RNFSManager::unlink(std::string filepath, RN::ReactPromise<void> promise) noexcept
try
{
    size_t pathLength{ filepath.length() };

    if (pathLength <= 0) {
        promise.Reject("Invalid path.");
    }
    else {
        bool hasTrailingSlash{ filepath[pathLength - 1] == '\\' || filepath[pathLength - 1] == '/' };
        std::filesystem::path path(hasTrailingSlash ? filepath.substr(0, pathLength - 1) : filepath);
        path.make_preferred();

        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(path.parent_path().wstring()) };
        auto target{ co_await folder.GetItemAsync(path.filename().wstring()) };
        co_await target.DeleteAsync();

        promise.Resolve();
    }
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else
    {
        // "Failed to unlink file" 
        promise.Reject( winrt::to_string(ex.message()).c_str() );
    }
}


winrt::fire_and_forget RNFSManager::exists(std::string filepath, RN::ReactPromise<bool> promise) noexcept
try
{
    size_t fileLength{ filepath.length() };

    if (fileLength <= 0) {
        promise.Resolve(false);
    }
    else {
        bool hasTrailingSlash{ filepath[fileLength - 1] == '\\' || filepath[fileLength - 1] == '/' };
        std::filesystem::path path(hasTrailingSlash ? filepath.substr(0, fileLength - 1) : filepath);

        winrt::hstring directoryPath, fileName;
        splitPath(filepath, directoryPath, fileName);
        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
        if (fileName.size() > 0) {
            co_await folder.GetItemAsync(fileName);
        }
        promise.Resolve(true);
    }
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
        promise.Resolve(false);
    }
    // "Failed to check if file or directory exists.
    promise.Reject(winrt::to_string(ex.message()).c_str());
}


void RNFSManager::stopDownload(int32_t jobID) noexcept
{
    m_tasks.Cancel(jobID);
}


void RNFSManager::stopUpload(int32_t jobID) noexcept
{
    m_tasks.Cancel(jobID);
}


winrt::fire_and_forget RNFSManager::readFile(std::string filepath, RN::ReactPromise<std::string> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    Streams::IBuffer buffer{ co_await FileIO::ReadBufferAsync(file) };
    winrt::hstring base64Content{ Cryptography::CryptographicBuffer::EncodeToBase64String(buffer) };
    promise.Resolve(winrt::to_string(base64Content));
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else if (result == HRESULT_FROM_WIN32(E_ACCESSDENIED)) // UnauthorizedAccessException
    {
        promise.Reject(RN::ReactError{ "EISDIR", "EISDIR: illegal operation on a directory, read" });
    }
    else
    {
        // "Failed to read file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


winrt::fire_and_forget RNFSManager::stat(std::string filepath, RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    size_t pathLength{ filepath.length() };

    if (pathLength <= 0) {
        promise.Reject("Invalid path.");
    }
    else {
        bool hasTrailingSlash{ filepath[pathLength - 1] == '\\' || filepath[pathLength - 1] == '/' };
        std::filesystem::path path(hasTrailingSlash ? filepath.substr(0, pathLength - 1) : filepath);
        path.make_preferred();

        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(path.parent_path().wstring()) };
        IStorageItem item{ co_await folder.GetItemAsync(path.filename().wstring()) };

        auto properties{ co_await item.GetBasicPropertiesAsync() };
        RN::JSValueObject fileInfo;
        fileInfo["ctime"] = winrt::clock::to_time_t(item.DateCreated());
        fileInfo["mtime"] = winrt::clock::to_time_t(properties.DateModified());
        fileInfo["size"] = std::to_string(properties.Size());
        fileInfo["type"] = item.IsOfType(StorageItemTypes::Folder) ? 1 : 0;
        promise.Resolve(fileInfo);
    }
}
catch (...)
{
    promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
}


winrt::fire_and_forget RNFSManager::readDir(std::string directory, RN::ReactPromise<RN::JSValueArray> promise) noexcept
try
{
    std::filesystem::path path(directory);
    path.make_preferred();
    StorageFolder targetDirectory{ co_await StorageFolder::GetFolderFromPathAsync(path.c_str()) };

    RN::JSValueArray resultsArray;

    auto items{ co_await targetDirectory.GetItemsAsync() };
    for (auto item : items)
    {
        auto properties{ co_await item.GetBasicPropertiesAsync() };

        RN::JSValueObject itemInfo;
        itemInfo["ctime"] = winrt::clock::to_time_t(targetDirectory.DateCreated());
        itemInfo["mtime"] = winrt::clock::to_time_t(properties.DateModified());
        itemInfo["name"] = to_string(item.Name());
        itemInfo["path"] = to_string(item.Path());
        itemInfo["size"] = properties.Size();
        itemInfo["type"] = item.IsOfType(StorageItemTypes::Folder) ? 1 : 0;

        resultsArray.push_back(std::move(itemInfo));
    }

    promise.Resolve(resultsArray);
}
catch (const hresult_error& ex)
{
    // "Failed to read directory."
    promise.Reject(winrt::to_string(ex.message()).c_str());
}


winrt::fire_and_forget RNFSManager::read(std::string filepath, uint32_t length, uint64_t position, RN::ReactPromise<std::string> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    Streams::Buffer buffer{ length };

    Streams::IRandomAccessStream stream{ co_await file.OpenReadAsync() };
    stream.Seek(position);

    stream.ReadAsync(buffer, length, Streams::InputStreamOptions::None);
    std::string result{ winrt::to_string(Cryptography::CryptographicBuffer::EncodeToBase64String(buffer)) };

    promise.Resolve(result);
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else if (result == HRESULT_FROM_WIN32(E_ACCESSDENIED)) // UnauthorizedAccessException
    {
        promise.Reject(RN::ReactError{"EISDIR", "EISDIR: Could not open file for reading" });
    }
    else 
    {
        // "Failed to read from file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


winrt::fire_and_forget RNFSManager::hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept
try
{
    // Note: SHA224 is not part of winrt 
    if (algorithm.compare("sha224") == 0)
    {
        promise.Reject(RN::ReactError{ "Error", "WinRT does not offer sha224 encryption." });
        co_return;
    }

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    auto search{ availableHashes.find(algorithm) };
    if (search == availableHashes.end())
    {
        promise.Reject(RN::ReactError{ "Error", "Invalid hash algorithm " + algorithm});
        co_return;
    }

    CryptographyCore::HashAlgorithmProvider provider{ search->second() };
    Streams::IBuffer buffer{ co_await FileIO::ReadBufferAsync(file) };

    auto hashedBuffer{ provider.HashData(buffer) };
    auto result{ winrt::to_string(Cryptography::CryptographicBuffer::EncodeToHexString(hashedBuffer)) };

    promise.Resolve(result);
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else if (result == HRESULT_FROM_WIN32(E_ACCESSDENIED)) // UnauthorizedAccessException
    {
        promise.Reject(RN::ReactError{ "EISDIR", "EISDIR: illegal operation on a directory, read" });
    }
    else
    {
        // "Failed to get checksum from file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


winrt::fire_and_forget RNFSManager::writeFile(std::string filepath, std::string base64Content, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ Cryptography::CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting) };

    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };
    co_await stream.WriteAsync(buffer);

    promise.Resolve();
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else
    {
        // Failed to write to file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


winrt::fire_and_forget RNFSManager::appendFile(std::string filepath, std::string base64Content, RN::ReactPromise<void> promise) noexcept
try
{
    size_t fileLength = filepath.length();
    bool hasTrailingSlash{ filepath[fileLength - 1] == '\\' || filepath[fileLength - 1] == '/' };
    std::filesystem::path path(hasTrailingSlash ? filepath.substr(0, fileLength - 1) : filepath);

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.CreateFileAsync(fileName, CreationCollisionOption::OpenIfExists) };

    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ Cryptography::CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };
    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };

    stream.Seek(stream.Size()); // Writes to end of file
    co_await stream.WriteAsync(buffer);

    promise.Resolve();
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else
    {
        // "Failed to append to file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}
winrt::fire_and_forget RNFSManager::write(std::string filepath, std::string base64Content, int position, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ Cryptography::CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };
    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };

    if (position < 0)
    {
        stream.Seek(stream.Size()); // Writes to end of file
    }
    else
    {
        stream.Seek(position);
    }
    co_await stream.WriteAsync(buffer);
    promise.Resolve();
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject(RN::ReactError{ "ENOENT", "ENOENT: no such file or directory, open " + filepath });
    }
    else
    {
        // Failed to write to file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


winrt::fire_and_forget RNFSManager::downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept
{
    //JobID
    auto jobId{ options["jobId"].AsInt32() };
    try
    {
        //Filepath
        std::filesystem::path path(options["toFile"].AsString());
        path.make_preferred();
        if (path.filename().empty())
        {
            promise.Reject("Failed to determine filename in path");
            co_return;
        }
        auto filePath{ winrt::to_hstring(path.c_str()) };

        //URL
        std::string fromURLString{ options["fromUrl"].AsString() };
        std::wstring URLForURI(fromURLString.begin(), fromURLString.end());
        Uri uri{ URLForURI };

        //Headers
        auto const& headers{ options["headers"].AsObject() };

        //Progress Interval
        auto progressInterval{ options["progressInterval"].AsInt64() };

        //Progress Divider
        auto progressDivider{ options["progressDivider"].AsInt64() };

        winrt::Windows::Web::Http::HttpRequestMessage request{ winrt::Windows::Web::Http::HttpMethod::Get(), uri };
        Buffer buffer{ 8 * 1024 };
        HttpBufferContent content{ buffer };
        for (const auto& header : headers)
        {
            if (!request.Headers().TryAppendWithoutValidation(winrt::to_hstring(header.first), winrt::to_hstring(header.second.AsString())))
            {
                content.Headers().TryAppendWithoutValidation(winrt::to_hstring(header.first), winrt::to_hstring(header.second.AsString()));
            }
        }
        request.Content(content);

        co_await m_tasks.Add(jobId, ProcessDownloadRequestAsync(promise, request, filePath, jobId, progressInterval, progressDivider));
    }
    catch (const hresult_error& ex)
    {
        // "Failed to download file." 
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
    m_tasks.Cancel(jobId);
}


winrt::fire_and_forget RNFSManager::uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept
{
    auto jobId{ options["jobId"].AsInt32() };
    try
    {
        auto method{ options["method"].AsString() };

        winrt::Windows::Web::Http::HttpMethod httpMethod{ winrt::Windows::Web::Http::HttpMethod::Post() };
        if (method.compare("POST") != 0)
        {
            if (method.compare("PUT") == 0)
            {
                httpMethod = winrt::Windows::Web::Http::HttpMethod::Put();
            }
            else
            {
                promise.Reject("Invalid HTTP request: neither a POST nor a PUT request.");
                co_return;
            }
        }

        auto const& files{ options["files"].AsArray() };
        uint64_t totalUploadSize = 0;
        for (const auto& fileInfo : files)
        {
            auto const& fileObj{ fileInfo.AsObject() };
            auto filepath{ fileObj["filepath"].AsString() };

            winrt::hstring directoryPath, fileName;
            splitPath(filepath, directoryPath, fileName);

            try
            {
                StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
                StorageFile file{ co_await folder.GetFileAsync(fileName) };
                auto fileProperties{ co_await file.GetBasicPropertiesAsync() };
                totalUploadSize += fileProperties.Size();
            }
            catch (...)
            {
                continue;
            }
        }
        if (totalUploadSize <= 0)
        {
            promise.Reject("No files to upload");
            co_return;
        }

        co_await m_tasks.Add(jobId, ProcessUploadRequestAsync(promise, options, httpMethod, files, jobId, totalUploadSize));
    }
    catch (const hresult_error& ex)
    {
        // "Failed to upload file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
    m_tasks.Cancel(jobId);
}


void RNFSManager::touch(std::string filepath, int64_t mtime, int64_t ctime, bool modifyCreationTime, RN::ReactPromise<std::string> promise) noexcept
try
{

    std::filesystem::path path(filepath);
    path.make_preferred();
    auto s_path{ path.c_str() };
    PCWSTR actual_path{ s_path };
    DWORD accessMode{ GENERIC_READ | GENERIC_WRITE };
    DWORD shareMode{ FILE_SHARE_WRITE };
    DWORD creationMode{ OPEN_EXISTING };

    std::unique_ptr<void, handle_closer> handle(safe_handle(CreateFile2(actual_path, accessMode, shareMode, creationMode, nullptr)));
    if (!handle)
    {
        promise.Reject("Failed to create handle for file to touch.");
        return;
    }

    touchTime mtime_64{ mtime * 10000 + UNIX_EPOCH_IN_WINRT_INTERVAL };
    FILETIME mFileTime;
    mFileTime.dwLowDateTime = mtime_64.splitTime[0];
    mFileTime.dwHighDateTime = mtime_64.splitTime[1];

    if (modifyCreationTime)
    {
        touchTime ctime_64{ ctime * 10000 + UNIX_EPOCH_IN_WINRT_INTERVAL };
        FILETIME cFileTime;
        cFileTime.dwLowDateTime = ctime_64.splitTime[0];
        cFileTime.dwHighDateTime = ctime_64.splitTime[1];

        if (SetFileTime(handle.get(), &cFileTime, nullptr, &mFileTime) == 0)
        {
            promise.Reject("Failed to set new creation time and modified time of file.");
        }
        else
        {
            promise.Resolve(winrt::to_string(s_path));
        }
    }
    else
    {
        if (SetFileTime(handle.get(), nullptr, nullptr, &mFileTime) == 0)
        {
            promise.Reject("Failed to set new creation time and modified time of file.");
        }
        else
        {
            promise.Resolve(winrt::to_string(s_path));
        }
    }
}
catch (const hresult_error& ex)
{
    hresult result{ ex.code() };
    if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) // FileNotFoundException
    {
        promise.Reject("ENOENT: no such file.");
    }
    else
    {
        // "Failed to touch file."
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


void RNFSManager::splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept
{
    std::filesystem::path path(fullPath);
    path.make_preferred();

    directoryPath = path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"";
    fileName = path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"";
}


IAsyncAction RNFSManager::ProcessDownloadRequestAsync(RN::ReactPromise<RN::JSValueObject> promise,
    winrt::Windows::Web::Http::HttpRequestMessage request, std::wstring_view filePath, int32_t jobId, int64_t progressInterval, int64_t progressDivider)
{
    try
    {
        HttpResponseMessage response = co_await m_httpClient.SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);
        IReference<uint64_t> contentLength{ response.Content().Headers().ContentLength() };
        {
            RN::JSValueObject headersMap;
            for (auto const& header : response.Headers())
            {
                headersMap[to_string(header.Key())] = to_string(header.Value());
            }

            m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadBegin",
                RN::JSValueObject{
                    { "jobId", jobId },
                    { "statusCode", (int)response.StatusCode() },
                    { "contentLength", contentLength.Type() == PropertyType::UInt64 ? RN::JSValue(contentLength.Value()) : RN::JSValue{nullptr} },
                    { "headers", std::move(headersMap) },
                });
        }

        uint64_t totalRead{ 0 };

        std::filesystem::path fsFilePath{ filePath };

        StorageFolder storageFolder{ co_await StorageFolder::GetFolderFromPathAsync(fsFilePath.parent_path().wstring()) };
        StorageFile storageFile{ co_await storageFolder.CreateFileAsync(fsFilePath.filename().wstring(), CreationCollisionOption::ReplaceExisting) };
        IRandomAccessStream  stream{ co_await storageFile.OpenAsync(FileAccessMode::ReadWrite) };
        IOutputStream outputStream{ stream.GetOutputStreamAt(0) };

        auto contentStream = co_await response.Content().ReadAsInputStreamAsync();
        auto contentLengthForProgress = contentLength.Type() == PropertyType::UInt64 ? contentLength.Value() : -1;
        
        Buffer buffer{ 8 * 1024 };
        uint32_t read = 0;
        int64_t initialProgressTime{ winrt::clock::now().time_since_epoch().count() / 10000 };
        int64_t currentProgressTime;
        uint64_t progressDividerUnsigned{ uint64_t(progressDivider) };

        for (;;)
        {
            buffer.Length(0);
            auto readBuffer = co_await contentStream.ReadAsync(buffer, buffer.Capacity(), InputStreamOptions::None);
            read = readBuffer.Length();
            if (readBuffer.Length() == 0)
            {
                break;
            }

            co_await outputStream.WriteAsync(readBuffer);
            totalRead += read;

            if (progressInterval > 0)
            {
                currentProgressTime = winrt::clock::now().time_since_epoch().count() / 10000;
                if(currentProgressTime - initialProgressTime >= progressInterval)
                {
                    m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadProgress",
                        RN::JSValueObject{
                            { "jobId", jobId },
                            { "contentLength", contentLength.Type() == PropertyType::UInt64 ? RN::JSValue(contentLength.Value()) : RN::JSValue{nullptr} },
                            { "bytesWritten", totalRead },
                        });
                    initialProgressTime = winrt::clock::now().time_since_epoch().count() / 10000;
                }
            }
            else if (progressDivider <= 0)
            {
                m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadProgress",
                    RN::JSValueObject{
                        { "jobId", jobId },
                        { "contentLength", contentLength.Type() == PropertyType::UInt64 ? RN::JSValue(contentLength.Value()) : RN::JSValue{nullptr} },
                        { "bytesWritten", totalRead },
                    });
            }
            else
            {
                if (totalRead * 100 / contentLengthForProgress >= progressDividerUnsigned ||
                    totalRead == contentLengthForProgress) {
                    m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadProgress",
                        RN::JSValueObject{
                            { "jobId", jobId },
                            { "contentLength", contentLength.Type() == PropertyType::UInt64 ? RN::JSValue(contentLength.Value()) : RN::JSValue{nullptr} },
                            { "bytesWritten", totalRead },
                        });
                }
            }
        }

        promise.Resolve(RN::JSValueObject
            {
                { "jobId", jobId },
                { "statusCode", (int)response.StatusCode() },
                { "bytesWritten", totalRead },
            });
    }
    catch (winrt::hresult_canceled const& ex)
    {
        std::stringstream ss;
        ss << "CANCELLED: job '" << jobId << "' to file '" << to_string(filePath) << "'";
        promise.Reject(RN::ReactError{ std::to_string(ex.code()), ss.str(), RN::JSValueObject{} });
    }
    catch (const hresult_error& ex)
    {
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}


IAsyncAction RNFSManager::ProcessUploadRequestAsync(RN::ReactPromise<RN::JSValueObject> promise, RN::JSValueObject& options,
    winrt::Windows::Web::Http::HttpMethod httpMethod, RN::JSValueArray const& files, int32_t jobId, uint64_t totalUploadSize)
{
    try
    {
        winrt::hstring boundary{ L"-----" };
        std::string toUrl{ options["toUrl"].AsString() };
        std::wstring URLForURI(toUrl.begin(), toUrl.end());
        Uri uri{ URLForURI };

        winrt::Windows::Web::Http::HttpRequestMessage requestMessage{ httpMethod, uri };
        winrt::Windows::Web::Http::HttpMultipartFormDataContent requestContent{ boundary };

        auto const& headers{ options["headers"].AsObject() };
        
        for (auto const& entry : headers)
        {
            if (!requestMessage.Headers().TryAppendWithoutValidation(winrt::to_hstring(entry.first), winrt::to_hstring(entry.second.AsString())))
            {
                requestContent.Headers().TryAppendWithoutValidation(winrt::to_hstring(entry.first), winrt::to_hstring(entry.second.AsString()));
            }
        }

        auto const& fields{ options["fields"].AsObject() }; // placed in the header
        std::stringstream attempt;
        attempt << "form-data";
        for (auto const& field : fields)
        {
            attempt << "; " << field.first << "=" << field.second.AsString();
        }

        requestContent.Headers().ContentDisposition(Headers::HttpContentDispositionHeaderValue::Parse(winrt::to_hstring(attempt.str())));

        m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"UploadBegin",
            RN::JSValueObject{
                { "jobId", jobId },
            });

        uint64_t totalUploaded{ 0 };

        for (const auto& fileInfo : files)
        {
            auto const& fileObj{ fileInfo.AsObject() };
            auto name{ winrt::to_hstring(fileObj["name"].AsString()) }; // name to be sent via http request
            auto filename{ winrt::to_hstring(fileObj["filename"].AsString()) }; // filename to be sent via http request
            auto filepath{ fileObj["filepath"].AsString()}; // accessing the file

            try
            {
                winrt::hstring directoryPath, fileName;
                splitPath(filepath, directoryPath, fileName);
                StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
                StorageFile file{ co_await folder.GetFileAsync(fileName) };
                auto properties{ co_await file.GetBasicPropertiesAsync() };

                HttpBufferContent entry{ co_await FileIO::ReadBufferAsync(file) };
                requestContent.Add(entry, name, filename);

                totalUploaded += properties.Size();
                m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"UploadProgress",
                    RN::JSValueObject{
                        { "jobId", jobId },
                        { "totalBytesExpectedToSend", totalUploadSize },   // The total number of bytes that will be sent to the server
                        { "totalBytesSent", totalUploaded },
                    });
            }
            catch (...)
            {
                continue;
            }
        }

        requestMessage.Content(requestContent);
        HttpResponseMessage response = co_await m_httpClient.SendRequestAsync(requestMessage, HttpCompletionOption::ResponseHeadersRead);

        auto statusCode{ std::to_string(int(response.StatusCode())) };
        auto resultHeaders{ winrt::to_string(response.Headers().ToString()) };
        auto resultContent{ winrt::to_string(co_await response.Content().ReadAsStringAsync()) };

        promise.Resolve(RN::JSValueObject
            {
                { "jobId", jobId },
                { "statusCode", statusCode},
                { "headers", resultHeaders},
                { "body", resultContent},
            });
    }
    catch (winrt::hresult_canceled const& ex)
    {
        std::stringstream ss;
        ss << "CANCELLED: job '" << jobId << "' to file '" << "'";
        promise.Reject(RN::ReactError{ std::to_string(ex.code()), ss.str(), RN::JSValueObject{} });
    }
    catch (const hresult_error& ex)
    {
        promise.Reject(winrt::to_string(ex.message()).c_str());
    }
}

