// Copyright (C) Microsoft Corporation. All rights reserved.

#include "pch.h"

#include "RNFSManager.h"

#include <filesystem>
#include <windows.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.FileProperties.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Storage;

#define CATCH_REJECT_PROMISE_MSG(result, message)                                                                      \
    catch (...)                                                                                                        \
    {                                                                                                                  \
        result.Reject(message);                                                                                        \
    }

void RNFSManager::ConstantsViaConstantsProvider(ReactConstantProvider& constants) noexcept
{
    // TODO: add back
    // constants.Add(L"RNFSMainBundlePath", to_string(Package::Current().InstalledLocation().Path()));
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

void RNFSManager::moveFile(
    std::string filepath,
    std::string destPath,
    RN::JSValueObject options,
    RN::ReactPromise<void> promise) noexcept {

}

void RNFSManager::copyFile(
    std::string filepath,
    std::string destPath,
    RN::JSValueObject options,
    RN::ReactPromise<void> promise) noexcept {

}

void RNFSManager::getFSInfo(RN::ReactPromise<RN::JSValueArray> promise) noexcept {

}

void RNFSManager::getAllExternalFilesDirs(RN::ReactPromise<std::string> promise) noexcept {

}

winrt::fire_and_forget RNFSManager::unlink(std::string filePath, ReactPromise<void> promise) noexcept
{
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

void RNFSManager::stopDownload(int jobID) noexcept {

}

void RNFSManager::resumeDownload(int jobID) noexcept {

}

void RNFSManager::isResumable(int jobID, RN::ReactPromise<bool> promise) noexcept {

}

void RNFSManager::stopUpload(int jobID) noexcept {

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


void RNFSManager::stat(std::string filepath, RN::ReactPromise<RN::JSValueArray> promise) noexcept {

}

winrt::fire_and_forget RNFSManager::readDir(std::string directory, ReactPromise<JSValueArray> promise) noexcept
{
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
            itemInfo["mtime"] = properties.DateModified().time_since_epoch() / std::chrono::seconds(1);
            itemInfo["name"] = to_string(item.Name());
            itemInfo["path"] = to_string(item.Path());
            itemInfo["size"] = properties.Size();
            itemInfo["type"] = item.IsOfType(StorageItemTypes::Folder) ? 1 : 0;

            resultsArray.push_back(std::move(itemInfo));
        }

        promise.Resolve(resultsArray);
    }
    CATCH_REJECT_PROMISE_MSG(promise, "failed to read directory");
}

void RNFSManager::read(
    std::string filePath,
    int length,
    int position,
    RN::ReactPromise<std::string> promise) noexcept {
    promise.Resolve("");
}

void RNFSManager::hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept {

}

winrt::fire_and_forget RNFSManager::writeFile(std::string filePath, std::string base64Content, JSValueObject options, ReactPromise<void> promise) noexcept
{
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
    CATCH_REJECT_PROMISE_MSG(promise, "failed to write file");
}

void RNFSManager::appendFile(
    std::string filepath,
    std::string base64Content,
    RN::ReactPromise<void> promise) noexcept {
    promise.Resolve();
}

void RNFSManager::write(
    std::string filePath,
    std::string base64Content,
    int position,
    RN::ReactPromise<void> promise) noexcept {
    promise.Resolve();
}

void RNFSManager::downloadFile (
    RN::JSValueObject options,
    RN::ReactPromise<RN::JSValueObject> promise) noexcept {

}

void RNFSManager::uploadFiles(
    RN::JSValueObject options,
    RN::ReactPromise<RN::JSValueObject> promise) noexcept {
}

void RNFSManager::touch (
    std::string filepath,
    double mtime,
    double ctime,
    RN::ReactPromise<void> promise) noexcept {

}

void RNFSManager::scanFile(std::string path, RN::ReactPromise<RN::JSValueObject> promise) noexcept {

}

void RNFSManager::splitPath(
    const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept
{
    std::filesystem::path path(fullPath);
    path.make_preferred();

    directoryPath = path.has_parent_path() ? winrt::to_hstring(path.parent_path().c_str()) : L"";
    fileName = path.has_filename() ? winrt::to_hstring(path.filename().c_str()) : L"";
}