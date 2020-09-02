// Copyright (C) Microsoft Corporation. All rights reserved.

#include "pch.h"

#include "RNFSManager.h"

#include <filesystem>
#include <sstream>
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

void RNFSManager::Initialize(ReactContext const& reactContext) noexcept
{
    m_reactContext = reactContext;
}

void RNFSManager::ConstantsViaConstantsProvider(ReactConstantProvider& constants) noexcept
{
    constants.Add(L"RNFSMainBundlePath", to_string(Package::Current().InstalledLocation().Path()));
    constants.Add(L"RNFSCachesDirectoryPath", to_string(ApplicationData::Current().LocalCacheFolder().Path()));

    constants.Add(L"RNFSRoamingDirectoryPath", to_string(ApplicationData::Current().RoamingFolder().Path()));
    constants.Add(L"RNFSDocumentDirectoryPath", to_string(ApplicationData::Current().LocalFolder().Path()));

    constants.Add(L"RNFSTemporaryDirectoryPath", to_string(ApplicationData::Current().TemporaryFolder().Path()));
    constants.Add(L"RNFSExternalDirectoryPath", to_string(ApplicationData::Current().LocalFolder().Path()));

    constants.Add(L"RNFSFileTypeRegular", 0);
    constants.Add(L"RNFSFileTypeDirectory", 1);
}

void RNFSManager::mkdir(std::string directory, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    std::filesystem::path path(directory);
    path.make_preferred();

    // Consistent with Apple's createDirectoryAtPath method and result, but not with Android's
    if (std::filesystem::create_directories(path) == false)
    {
        promise.Reject("Failed to create directory.");
    }
    else
    {
        promise.Resolve();
    }
}
catch (...)
{
    promise.Reject("Unexpected error while making directory.");
}

winrt::fire_and_forget RNFSManager::moveFile(std::string filepath, std::string destPath, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    std::filesystem::path dest(destPath);
    dest.make_preferred();

    StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFolder destFolder{ co_await StorageFolder::GetFolderFromPathAsync(dest.c_str()) };
    StorageFile file{ co_await srcFolder.GetFileAsync(fileName) };

    co_await file.MoveAsync(destFolder, fileName, NameCollisionOption::ReplaceExisting);

    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to move file");
}


winrt::fire_and_forget RNFSManager::copyFile(std::string filepath, std::string destPath, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    std::filesystem::path dest(destPath);
    dest.make_preferred();

    StorageFolder srcFolder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFolder destFolder{ co_await StorageFolder::GetFolderFromPathAsync(dest.c_str()) };
    StorageFile file{ co_await srcFolder.GetFileAsync(fileName) };

    co_await file.CopyAsync(destFolder, fileName, NameCollisionOption::ReplaceExisting);

    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to copy file.");
}

// TODO: Find a way to test this. May need a demo app because the program crashes when testing for some reason
winrt::fire_and_forget RNFSManager::getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    auto localFolder{ Windows::Storage::ApplicationData::Current().LocalFolder() };
    auto properties{ co_await localFolder.Properties().RetrievePropertiesAsync({L"System.FreeSpace", L"System.Capacity"}) };

    JSValueObject result;
    result["freeSpace"] = unbox_value<uint64_t>(properties.Lookup(L"System.FreeSpace"));
    result["totalSpace"] = unbox_value<uint64_t>(properties.Lookup(L"System.Capacity"));

    promise.Resolve(result);
}
catch (...)
{
    promise.Reject("Failed to retrieve file system info.");
}

winrt::fire_and_forget RNFSManager::unlink(std::string filepath, ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };

    auto target{ co_await folder.GetItemAsync(fileName) };
    co_await target.DeleteAsync();

    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to unlink.");
}


void RNFSManager::exists(std::string filepath, ReactPromise<bool> promise) noexcept
try
{
    std::filesystem::path path(filepath);
    promise.Resolve(std::filesystem::exists(path));
}
catch (...)
{
    promise.Reject("Failed to check if file or directory exists.");
}

// TODO: Downloader
void RNFSManager::stopDownload(int32_t jobID) noexcept
{
    m_tasks.Cancel(jobID);
}

// TODO: Downloader
void RNFSManager::stopUpload(int32_t jobID) noexcept
{
    m_tasks.Cancel(jobID);
}

winrt::fire_and_forget RNFSManager::readFile(std::string filepath, ReactPromise<std::string> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    Streams::IBuffer buffer{ co_await FileIO::ReadBufferAsync(file) };
    winrt::hstring base64Content{ CryptographicBuffer::EncodeToBase64String(buffer) };

    promise.Resolve(winrt::to_string(base64Content));
}
catch (...)
{
    promise.Reject("Failed to read file.");
}

winrt::fire_and_forget RNFSManager::stat(std::string filepath, RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    std::filesystem::path path(filepath);
    path.make_preferred();

    std::string resultPath{ winrt::to_string(path.c_str()) };
    auto potentialPath{ winrt::to_hstring(resultPath) };
    bool isFile{ false };
    uint64_t ctime{ 0 };
    uint64_t mtime{ 0 };
    uint64_t size{ 0 };

    // Try to open as folder
    try
    {
        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(potentialPath) };
        auto properties{ co_await folder.GetBasicPropertiesAsync() };
        ctime = folder.DateCreated().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        mtime = properties.DateModified().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        size = properties.Size();
    }
    catch (...)
    {
        isFile = true;
        auto lastCharIndex{ filepath.length() - 1 };
        if (resultPath[lastCharIndex] == '\\')
        {
            path = std::filesystem::path(resultPath.substr(0, lastCharIndex));
        }
    }

    // Try to open as file
    if (isFile)
    {
        auto directoryPath{ path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"" };
        auto fileName{ path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"" };

        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
        auto properties{ (co_await folder.GetFileAsync(fileName)).Properties() };
        auto propertyMap{ co_await properties.RetrievePropertiesAsync({ L"System.DateCreated", L"System.DateModified", L"System.Size" }) };
        ctime = winrt::unbox_value<DateTime>(propertyMap.Lookup(L"System.DateCreated")).time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        mtime = winrt::unbox_value<DateTime>(propertyMap.Lookup(L"System.DateModified")).time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        size = winrt::unbox_value<uint64_t>(propertyMap.Lookup(L"System.Size"));
    }

    JSValueObject fileInfo;
    fileInfo["ctime"] = ctime;
    fileInfo["mtime"] = mtime;
    fileInfo["size"] = std::to_string(size);
    fileInfo["type"] = isFile ? 0 : 1;

    promise.Resolve(fileInfo);
}
catch (...)
{
    promise.Reject("Failed to retrieve file info.");
}

winrt::fire_and_forget RNFSManager::readDir(std::string directory, ReactPromise<JSValueArray> promise) noexcept
try
{
    std::filesystem::path path(directory);
    path.make_preferred();
    StorageFolder targetDirectory{ co_await StorageFolder::GetFolderFromPathAsync(path.c_str()) };

    JSValueArray resultsArray;

    auto items{ co_await targetDirectory.GetItemsAsync() };
    for (auto item : items)
    {
        auto properties{ co_await item.GetBasicPropertiesAsync() };

        JSValueObject itemInfo;
        itemInfo["ctime"] = targetDirectory.DateCreated().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        itemInfo["mtime"] = properties.DateModified().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
        itemInfo["name"] = to_string(item.Name());
        itemInfo["path"] = to_string(item.Path());
        itemInfo["size"] = properties.Size();
        itemInfo["type"] = item.IsOfType(StorageItemTypes::Folder) ? 1 : 0;

        resultsArray.push_back(std::move(itemInfo));
    }

    promise.Resolve(resultsArray);
}
catch (...)
{
    promise.Reject("Failed to read directory.");
}


winrt::fire_and_forget RNFSManager::read(std::string filepath, int length, int position, RN::ReactPromise<std::string> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    Streams::IBuffer buffer{ co_await FileIO::ReadBufferAsync(file) };

    Streams::IRandomAccessStream stream{ co_await file.OpenReadAsync() };
    stream.Seek(position);

    stream.ReadAsync(buffer, length, Streams::InputStreamOptions::None);
    std::string result{ winrt::to_string(CryptographicBuffer::EncodeToBase64String(buffer)) };

    promise.Resolve(result);
}
catch (...)
{
    promise.Reject("Failed to read from file.");
}


winrt::fire_and_forget RNFSManager::hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept
try
{
    auto temp = algorithm;
    // Note: SHA224 is not part of winrt 
    if (algorithm.compare("sha224") == 0)
    {
        promise.Reject("WinRT does not offer sha224 encryption.");
        co_return;
    }

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    auto search{ availableHashes.find(algorithm) };
    if (search == availableHashes.end())
    {
        promise.Reject("Failed to find hash algorithm.");
        co_return;
    }

    HashAlgorithmProvider provider{ search->second() };
    Streams::IBuffer buffer{ co_await FileIO::ReadBufferAsync(file) };

    auto hashedBuffer{ provider.HashData(buffer) };
    auto result{ winrt::to_string(CryptographicBuffer::EncodeToHexString(hashedBuffer)) };

    promise.Resolve(result);
}
catch (...)
{
    promise.Reject("Failed to get checksum from file.");
}


winrt::fire_and_forget RNFSManager::writeFile(std::string filepath, std::string base64Content, JSValueObject options, ReactPromise<void> promise) noexcept
try
{
    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting) };

    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };
    co_await stream.WriteAsync(buffer);

    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to write to file.");
}


winrt::fire_and_forget RNFSManager::appendFile(std::string filepath, std::string base64Content, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };
    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };

    stream.Seek(UINT64_MAX); // Writes to end of file
    co_await stream.WriteAsync(buffer);

    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to append to file.");
}

winrt::fire_and_forget RNFSManager::write(std::string filepath, std::string base64Content, int position, ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    StorageFile file{ co_await folder.GetFileAsync(fileName) };

    winrt::hstring base64ContentStr{ winrt::to_hstring(base64Content) };
    Streams::IBuffer buffer{ CryptographicBuffer::DecodeFromBase64String(base64ContentStr) };
    Streams::IRandomAccessStream stream{ co_await file.OpenAsync(FileAccessMode::ReadWrite) };

    if (position < 0)
    {
        stream.Seek(UINT64_MAX); // Writes to end of file
    }
    else
    {
        stream.Seek(position);
    }
    co_await stream.WriteAsync(buffer);
    promise.Resolve();
}
catch (...)
{
    promise.Reject("Failed to write to file.");
}

// TODO: Downloader
winrt::fire_and_forget RNFSManager::downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    // All of these are definitely not null

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

    // These might be null though

    //JobID
    auto jobId{ options["jobId"].AsInt32() };

    //Headers
    auto const& headers{ options["headers"].AsObject() };

    //Progress Interval
    //auto progressInterval{ options["progressInterval"].AsInt64() };

    //Progress Divider
    auto progressDivider{ options["progressDivider"].AsInt64() };

    //readTimeout
    //auto readTimeout{ options["readTimeout"].AsInt64() };

    //connectionTimeout ?
    //auto connectionTimeout{ options["connectionTimeout"].AsInt64() };

    //hasBeginCallback ?
    //auto hasBeginCallback{ options["hasBeginCallback"].AsBoolean() };

    //hasProgressCallback ?
    //auto hasProgressCallback{ options["hasProgressCallback"].AsBoolean() };

    winrt::Windows::Web::Http::HttpRequestMessage request(winrt::Windows::Web::Http::HttpMethod::Get(), uri);
    for (const auto& header : headers)
    {
        request.Headers().Insert(winrt::to_hstring(header.first), winrt::to_hstring(header.second.AsString()));
    }

    co_await m_tasks.Add(jobId, ProcessRequestAsync(promise, request, filePath, jobId, progressDivider));
}
catch (...)
{
    promise.Reject("Failed to download file.");
}

// TODO: Unfinished method
winrt::fire_and_forget RNFSManager::uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    auto jobId{ options["jobId"].AsInt32() };
    

    //auto binaryStreamOnly{ options["binaryStreamOnly"].AsBoolean() };

    auto method{ options["method"].AsString() };

    winrt::Windows::Web::Http::HttpMethod httpMethod{ winrt::Windows::Web::Http::HttpMethod::Post() };
    if(!method.compare("POST"))
    {
        if (method.compare("PUT"))
        {
            httpMethod = winrt::Windows::Web::Http::HttpMethod::Put();
        }
        else
        {
            promise.Reject("Invalid HTTP request: neither a POST nor a PUT request.");
        }
    }
   
    auto const& files{ options["files"].AsObject() };
    uint64_t totalUploadSize = 0;
    for (const auto& fileInfo : files)
    {
        auto filepath{ files["filepath"].AsString() };

        winrt::hstring directoryPath, fileName;
        splitPath(filepath, directoryPath, fileName);

        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
        StorageFile file{ co_await folder.GetFileAsync(fileName) };
        auto fileInfo{ co_await file.GetBasicPropertiesAsync() };

        totalUploadSize += fileInfo.Size();
    }

    co_await m_tasks.Add(jobId, ProcessUploadRequestAsync(promise, options, httpMethod, files, jobId, totalUploadSize));
}
catch (...)
{
    promise.Reject("Failed to upload file.");
}

// TODO: Modify FS.Common.js to detect Windows (or at least if the platform isn't Android to modify the creation time)
void RNFSManager::touch(std::string filepath, double mtime, double ctime, RN::ReactPromise<std::string> promise) noexcept
try
{
    std::filesystem::path path(filepath);
    path.make_preferred();
    auto s_path{ path.c_str() };
    LPCWSTR actual_path{ s_path };
    DWORD accessMode{ GENERIC_READ | GENERIC_WRITE };
    DWORD shareMode{ FILE_SHARE_WRITE };
    DWORD creationMode{ OPEN_EXISTING };

    std::unique_ptr<void, handle_closer> handle(safe_handle(CreateFile2(actual_path, accessMode, shareMode, creationMode, nullptr)));
    if (!handle)
    {
        promise.Reject("Failed to create handle for file to touch.");
        return;
    }

    int64_t mtime_64{ static_cast<int64_t>(mtime * 10000.0) + UNIX_EPOCH_IN_WINRT_SECONDS * 10000000 };
    int64_t ctime_64{ static_cast<int64_t>(ctime * 10000.0) + UNIX_EPOCH_IN_WINRT_SECONDS * 10000000 };

    FILETIME cFileTime, mFileTime;
    cFileTime.dwLowDateTime = static_cast<DWORD>(ctime_64);
    ctime_64 >>= 32;
    cFileTime.dwHighDateTime = static_cast<DWORD>(ctime_64);

    mFileTime.dwLowDateTime = static_cast<DWORD>(mtime_64);
    mtime_64 >>= 32;
    mFileTime.dwHighDateTime = static_cast<DWORD>(mtime_64);

    if (SetFileTime(handle.get(), &cFileTime, nullptr, &mFileTime) == 0)
    {
        promise.Reject("Failed to set new creation time and modified time of file.");
    }
    else
    {
        promise.Resolve(winrt::to_string(s_path));
    }
}
catch (...)
{
    promise.Reject("Failed to touch file.");
}

void RNFSManager::splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept
{
    std::filesystem::path path(fullPath);
    path.make_preferred();

    directoryPath = path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"";
    fileName = path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"";
}

IAsyncAction RNFSManager::ProcessRequestAsync(RN::ReactPromise<RN::JSValueObject> promise,
    HttpRequestMessage request, std::wstring_view filePath, int jobId, int progressIncrement)
{
    try
    {
        HttpResponseMessage response = co_await m_httpClient.SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);
        IReference<uint64_t> contentLength{ response.Content().Headers().ContentLength() };
        {
            JSValueObject headersMap;
            for (auto const& header : response.Headers())
            {
                headersMap[to_string(header.Key())] = to_string(header.Value());
            }

            m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadBegin",
                JSValueObject{
                    { "jobId", jobId },
                    { "statusCode", (int)response.StatusCode() },
                    { "contentLength", contentLength.Type() == PropertyType::UInt64 ? JSValue(contentLength.Value()) : JSValue{nullptr} },
                    { "headers", std::move(headersMap) },
                });
        }

        int64_t totalRead{ 0 };

        std::filesystem::path fsFilePath{ filePath };

		StorageFolder storageFolder{ co_await StorageFolder::GetFolderFromPathAsync(fsFilePath.parent_path().wstring()) };
		StorageFile storageFile{ co_await storageFolder.CreateFileAsync(fsFilePath.filename().wstring(), CreationCollisionOption::ReplaceExisting) };
		IRandomAccessStream  stream{ co_await storageFile.OpenAsync(FileAccessMode::ReadWrite) };
        IOutputStream outputStream{ stream.GetOutputStreamAt(0) };

        auto contentStream = co_await response.Content().ReadAsInputStreamAsync();
        auto contentLengthForProgress = contentLength.Type() == PropertyType::UInt64 ? contentLength.Value() : -1;
        auto nextProgressIncrement = progressIncrement;
        Buffer buffer{ 8 * 1024 };
        uint32_t read = 0;

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

            if (contentLengthForProgress >= 0)
            {
                totalRead += read;
                if (totalRead * 100 / contentLengthForProgress >= nextProgressIncrement ||
                    totalRead == contentLengthForProgress)
                {
                    m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"DownloadProgress",
                        JSValueObject{
                            { "jobId", jobId },
                            { "contentLength", contentLength.Type() == PropertyType::UInt64 ? JSValue(contentLength.Value()) : JSValue{nullptr} },
                            { "bytesWritten", totalRead },
                        });

                    nextProgressIncrement += progressIncrement;
                }
            }
        }

        promise.Resolve(JSValueObject
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
        promise.Reject(ReactError{ std::to_string(ex.code()), ss.str(), JSValueObject{} });
    }
}

IAsyncAction RNFSManager::ProcessUploadRequestAsync(RN::ReactPromise<RN::JSValueObject> promise, RN::JSValueObject& options,
    winrt::Windows::Web::Http::HttpMethod httpMethod, RN::JSValueObject const& files, int jobId, uint64_t totalUploadSize)
{
    try
    {
        std::string toUrl{ options["toUrl"].AsString() };
        std::wstring URLForURI(toUrl.begin(), toUrl.end());
        Uri uri{ URLForURI };

        //type Headers = { [name: string]: string };
        auto const& headers{ options["headers"].AsObject() };

        //type Fields = { [name: string]: string };
        auto const& fields{ options["fields"].AsObject() }; // placed in the header

        bool isFirst{ true }; // When to start the request
        int64_t totalSent{ 0 };
        for (const auto& fileInfo : files)
        {
            auto name{ files["name"].AsString() }; // name to be sent via http request
            auto filename{ files["filename"].AsString() }; // filename to be sent via http request
            auto filepath{ files["filepath"].AsString() }; // accessing the file
            auto filetype{ files["filetype"].AsString() }; // content type to be sent via http request

            winrt::Windows::Web::Http::HttpRequestMessage request(httpMethod, uri);

            for (auto const& entry : headers)
            {
                request.Headers().Insert(winrt::to_hstring(entry.first), winrt::to_hstring(entry.second.AsString()));
            }

            request.Headers().Insert(L"Name", winrt::to_hstring(name));
            request.Headers().Insert(L"Filename", winrt::to_hstring(filename));
            request.Headers().Insert(L"Content-Type", winrt::to_hstring(filetype));

            for (auto const& field : fields)
            {
                request.Headers().Insert(winrt::to_hstring(field.first), winrt::to_hstring(field.second.AsString()));
            }

            winrt::hstring directoryPath, fileName;
            splitPath(filepath, directoryPath, fileName);

            StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
            StorageFile file{ co_await folder.GetFileAsync(fileName) };

            Windows::Web::Http::HttpBufferContent content{ co_await FileIO::ReadBufferAsync(file) };
            request.Content(content);

            HttpResponseMessage response = co_await m_httpClient.SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);
            if (isFirst)
            {
                m_reactContext.CallJSFunction(L"RCTDeviceEventEmitter", L"emit", L"UploadBegin",
                    JSValueObject{
                        { "jobId", jobId },
                    });
                isFirst = false;
            }

            //HttpStatusCode resultStatusCode = response.StatusCode();
            //Headers::HttpResponseHeaderCollection resultHeaders = response.Headers();
            //IHttpContent resultBody = response.Content();

        }

        //HttpResponseMessage response = co_await m_httpClient.SendRequestAsync(request, HttpCompletionOption::ResponseHeadersRead);
        //IReference<uint64_t> contentLength{ response.Content().Headers().ContentLength() };
        {
           // JSValueObject headersMap;
            //for (auto const& header : response.Headers())
            //{
            //    headersMap[to_string(header.Key())] = to_string(header.Value());
            //}
            
        }

        

        promise.Resolve(JSValueObject
            {
                { "jobId", jobId },
                { "statusCode", 123},
                { "headers", "asdf" },
                { "body", "asdf" },
            });
    }
    catch (winrt::hresult_canceled const& ex)
    {
        std::stringstream ss;
        ss << "CANCELLED: job '" << jobId << "' to file '" << "'";
        promise.Reject(ReactError{ std::to_string(ex.code()), ss.str(), JSValueObject{} });
    }
}
