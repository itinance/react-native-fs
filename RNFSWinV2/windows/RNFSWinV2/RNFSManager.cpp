// Copyright (C) Microsoft Corporation. All rights reserved.

#include "pch.h"

#include "RNFSManager.h"

#include <filesystem>
#include <windows.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::Cryptography::Core;
using namespace winrt::Windows::Storage;
using namespace winrt::Windows::Foundation;

const int64_t UNIX_EPOCH_IN_WINRT_SECONDS = 11644473600;

const std::map<std::string, std::function<HashAlgorithmProvider()>> availableHashes {
    {"md5", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Md5()); } },
    {"sha1", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha1()); } },
    {"sha256", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha256()); } },
    {"sha384", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha384()); } },
    {"sha512", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha512()); } }
};

struct handle_closer
{
    void operator()(HANDLE h) noexcept { assert(h != INVALID_HANDLE_VALUE); if (h) CloseHandle(h); }
};

inline HANDLE safe_handle(HANDLE h) noexcept
{
    return (h == INVALID_HANDLE_VALUE) ? nullptr : h;
}

void RNFSManager::ConstantsViaConstantsProvider(ReactConstantProvider& constants) noexcept
{
    // TODO: add back
    //constants.Add(L"RNFSMainBundlePath", to_string(Package::Current().InstalledLocation().Path()));
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
        if (std::filesystem::create_directories(directory) == false) 
        {
            promise.Reject("Failed to create directory.");
        }
        promise.Resolve();
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

        StorageFolder srcFolder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFolder destFolder = co_await StorageFolder::GetFolderFromPathAsync(dest.c_str());
        StorageFile file = co_await srcFolder.GetFileAsync(fileName);

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

        StorageFolder srcFolder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFolder destFolder = co_await StorageFolder::GetFolderFromPathAsync(dest.c_str());
        StorageFile file = co_await srcFolder.GetFileAsync(fileName);

        co_await file.CopyAsync(destFolder, fileName, NameCollisionOption::ReplaceExisting);

        promise.Resolve();
    }
    catch (...)
    {
        promise.Reject("Failed to copy file");
    }

// TODO: Find a way to test this. May need a demo app because the program crashes when testing for some reason
winrt::fire_and_forget RNFSManager::getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept
try
{
    auto localFolder{ Windows::Storage::ApplicationData::Current().LocalFolder() };
    auto properties = co_await localFolder.Properties().RetrievePropertiesAsync({L"System.FreeSpace", L"System.Capacity"});

    JSValueObject result;
    result["freeSpace"] = unbox_value<double>(properties.Lookup(L"System.FreeSpace"));
    result["totalSpace"] = unbox_value<double>(properties.Lookup(L"System.Capacity"));
    
    promise.Resolve(result);
}
catch (...)
{
    promise.Reject("Failed to retrieve file system info.");
}

winrt::fire_and_forget RNFSManager::unlink(std::string filePath, ReactPromise<void> promise) noexcept
    try
    {
        winrt::hstring directoryPath, fileName;
        splitPath(filePath, directoryPath, fileName);
    
        StorageFolder folder{ co_await StorageFolder::GetFolderFromPathAsync(directoryPath) };
    
        auto target = co_await folder.GetItemAsync(fileName);
        co_await target.DeleteAsync();
    
        promise.Resolve();
    }
    catch (...)
    {
        promise.Reject("Failed to unlink.");
    }


void RNFSManager::exists(std::string fullpath, ReactPromise<bool> promise) noexcept
    try
    {
        std::filesystem::path path(fullpath);
        promise.Resolve(std::filesystem::exists(path));
    }
    catch (...)
    {
        promise.Reject("Failed to check if file or directory exists.");
    }

// TODO: Downloader
void RNFSManager::stopDownload(int jobID) noexcept
{

}

// TODO: Downloader
void RNFSManager::stopUpload(int jobID) noexcept
{

}

winrt::fire_and_forget RNFSManager::readFile(std::string filePath, ReactPromise<std::string> promise) noexcept
    try
    {
        winrt::hstring directoryPath, fileName;
        splitPath(filePath, directoryPath, fileName);
    
        StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFile file = co_await folder.GetFileAsync(fileName);
    
        Streams::IBuffer buffer = co_await FileIO::ReadBufferAsync(file);
        winrt::hstring base64Content = CryptographicBuffer::EncodeToBase64String(buffer);
    
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

    std::string resultPath = winrt::to_string(path.c_str());
    auto directoryPath = path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"";
    auto fileName = path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"";

    StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);

    bool isFile = !fileName.empty();

    FileProperties::IStorageItemContentProperties properties = (isFile)
        ? (co_await folder.GetFileAsync(fileName)).Properties()
        : folder.Properties();

    auto propertyMap = co_await properties.RetrievePropertiesAsync({ L"System.DateCreated", L"System.DateModified", L"System.Size" });

    auto ctime = winrt::unbox_value<DateTime>(propertyMap.Lookup(L"System.DateCreated")).time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
    auto mtime = winrt::unbox_value<DateTime>(propertyMap.Lookup(L"System.DateModified")).time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
    auto size = std::to_string(winrt::unbox_value<uint64_t>(propertyMap.Lookup(L"System.Size")));

    JSValueObject fileInfo;
    fileInfo["path"] = resultPath;
    fileInfo["ctime"] = ctime;
    fileInfo["mtime"] = mtime;
    fileInfo["size"] = size;
    fileInfo["isFile"] = isFile;
    fileInfo["isFolder"] = !isFile;

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

        auto items = co_await targetDirectory.GetItemsAsync();
        for (auto item : items)
        {
            auto properties = co_await item.GetBasicPropertiesAsync();

            JSValueObject itemInfo;
            itemInfo["ctime"] = targetDirectory.DateCreated().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
            itemInfo["mtime"] = properties.DateModified().time_since_epoch() / std::chrono::seconds(1) - UNIX_EPOCH_IN_WINRT_SECONDS;
            itemInfo["name"] = to_string(item.Name());
            itemInfo["path"] = to_string(item.Path());
            itemInfo["size"] = properties.Size();
            itemInfo["isFile"] = item.IsOfType(StorageItemTypes::File) ? true : false;
            itemInfo["isFolder"] = item.IsOfType(StorageItemTypes::Folder) ? true : false;

            resultsArray.push_back(std::move(itemInfo));
        }

        promise.Resolve(resultsArray);
    }
    catch (...)
    {
        promise.Reject("Failed to read directory.");
    }


winrt::fire_and_forget RNFSManager::read(std::string filePath, int length, int position, RN::ReactPromise<std::string> promise) noexcept
    try
    {
        winrt::hstring directoryPath, fileName;
        splitPath(filePath, directoryPath, fileName);

        StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFile file = co_await folder.GetFileAsync(fileName);

        Streams::IBuffer buffer = co_await FileIO::ReadBufferAsync(file);

        Streams::IRandomAccessStream stream = co_await file.OpenReadAsync();
        stream.Seek(position);

        stream.ReadAsync(buffer, length, Streams::InputStreamOptions::None);
        std::string result = winrt::to_string(CryptographicBuffer::EncodeToBase64String(buffer));

        promise.Resolve(result);
    }
    catch (...)
    {
        promise.Reject("Failed to read from file.");
    }

// Note: SHA224 is not part of winrt  
winrt::fire_and_forget RNFSManager::hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept
try
{
    auto temp = algorithm;
    if (algorithm.compare("sha224") == 0)
    {
        promise.Reject("WinRT does not offer sha224 encryption.");
        co_return;
    }

    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
    StorageFile file = co_await folder.GetFileAsync(fileName);

    auto search = availableHashes.find(algorithm);
    if (search == availableHashes.end())
    {
        promise.Reject("Failed to find hash algorithm.");
        co_return;
    }

    HashAlgorithmProvider provider = search->second();
    Streams::IBuffer buffer = co_await FileIO::ReadBufferAsync(file);

    auto hashedBuffer = provider.HashData(buffer);
    auto result = winrt::to_string(CryptographicBuffer::EncodeToHexString(hashedBuffer));

    promise.Resolve(result);
}
catch (...)
{
    promise.Reject("Failed to get checksum from file.");
}

winrt::fire_and_forget RNFSManager::writeFile(std::string filePath, std::string base64Content, JSValueObject options, ReactPromise<void> promise) noexcept
    try
    {
        winrt::hstring base64ContentStr = winrt::to_hstring(base64Content);
        Streams::IBuffer buffer = CryptographicBuffer::DecodeFromBase64String(base64ContentStr);

        winrt::hstring directoryPath, fileName;
        splitPath(filePath, directoryPath, fileName);

        StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFile file = co_await folder.CreateFileAsync(fileName, CreationCollisionOption::ReplaceExisting);

        Streams::IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);
        co_await stream.WriteAsync(buffer);

        promise.Resolve();
    }
    catch (...)
    {
        promise.Reject("Failed to write to file.");
    }

// TODO: Implement
winrt::fire_and_forget RNFSManager::appendFile(std::string filepath, std::string base64Content, RN::ReactPromise<void> promise) noexcept
try
{
    winrt::hstring directoryPath, fileName;
    splitPath(filepath, directoryPath, fileName);

    StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
    StorageFile file = co_await folder.GetFileAsync(fileName);

    winrt::hstring base64ContentStr = winrt::to_hstring(base64Content);
    Streams::IBuffer buffer = CryptographicBuffer::DecodeFromBase64String(base64ContentStr);
    Streams::IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

    stream.Seek(UINT64_MAX); // Writes to end of file
    co_await stream.WriteAsync(buffer);

    promise.Resolve();
}
catch (...) 
{
    promise.Reject("Failed to append to file");
}

winrt::fire_and_forget RNFSManager::write(std::string filePath, std::string base64Content, int position, ReactPromise<void> promise) noexcept
    try {
        winrt::hstring directoryPath, fileName;
        splitPath(filePath, directoryPath, fileName);

        StorageFolder folder = co_await StorageFolder::GetFolderFromPathAsync(directoryPath);
        StorageFile file = co_await folder.GetFileAsync(fileName);

        winrt::hstring base64ContentStr = winrt::to_hstring(base64Content);
        Streams::IBuffer buffer = CryptographicBuffer::DecodeFromBase64String(base64ContentStr);
        Streams::IRandomAccessStream stream = co_await file.OpenAsync(FileAccessMode::ReadWrite);

        if (position < 0) {
            stream.Seek(UINT64_MAX); // Writes to end of file
        }
        else {
            stream.Seek(position);
        }
        co_await stream.WriteAsync(buffer);
        promise.Resolve();
    }
    catch(...) {
        promise.Reject("Failed to write to file.");
    }
    

// TODO: Downloader
void RNFSManager::downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept 
{

}

// TODO: Downloader
void RNFSManager::uploadFiles(
    RN::JSValueObject options,
    RN::ReactPromise<RN::JSValueObject> promise) noexcept {
}

void RNFSManager::touch (std::string filepath, double mtime, double ctime, RN::ReactPromise<void> promise) noexcept
try
{
    std::filesystem::path path(filepath);
    path.make_preferred();
    auto s_path = path.c_str();
    LPCWSTR actual_path = s_path;
    DWORD accessMode = GENERIC_READ | GENERIC_WRITE;
    DWORD shareMode = FILE_SHARE_WRITE;
    DWORD creationMode = OPEN_EXISTING;

    std::unique_ptr<void, handle_closer> handle(safe_handle(CreateFile2(actual_path, accessMode, shareMode, creationMode, nullptr)));
    if (!handle)
    {
        promise.Reject("Failed to open file to touch.");
    }

    int64_t mtime_64 = static_cast<int64_t>(mtime * 10000000.0) + UNIX_EPOCH_IN_WINRT_SECONDS * 10000000;
    int64_t ctime_64 = static_cast<int64_t>(ctime * 10000000.0) + UNIX_EPOCH_IN_WINRT_SECONDS * 10000000;

    FILETIME cFileTime, mFileTime;
    cFileTime.dwLowDateTime = static_cast<DWORD>(ctime_64);
    ctime_64 >>= 32;
    cFileTime.dwHighDateTime = static_cast<DWORD>(ctime_64);

    mFileTime.dwLowDateTime = static_cast<DWORD>(mtime_64);
    mtime_64 >>= 32;
    mFileTime.dwHighDateTime = static_cast<DWORD>(mtime_64);

    if (SetFileTime(handle.get(), &cFileTime, &mFileTime, nullptr) == 0)
    {
        promise.Reject("Failed to set new creation time and modified time of file.");
    }
    else
    {
        promise.Resolve();
    }
}
catch (...)
{
    promise.Reject("Failed to touch file.");
}

void RNFSManager::splitPath(
    const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept
{
    std::filesystem::path path(fullPath);
    path.make_preferred();

    directoryPath = path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"";
    fileName = path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"";
}