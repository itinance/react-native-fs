// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once
#include "NativeModules.h"
#include <string>
#include <mutex>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.Web.Http.h>

namespace Cryptography = winrt::Windows::Security::Cryptography;
namespace CryptographyCore = winrt::Windows::Security::Cryptography::Core;
namespace RN = winrt::Microsoft::ReactNative;

struct CancellationDisposable final
{
    CancellationDisposable() = default;
    CancellationDisposable(winrt::Windows::Foundation::IAsyncInfo const& async, std::function<void()>&& onCancel) noexcept;

    CancellationDisposable(CancellationDisposable&& other) noexcept;
    CancellationDisposable& operator=(CancellationDisposable&& other) noexcept;

    CancellationDisposable(CancellationDisposable const&) = delete;
    CancellationDisposable& operator=(CancellationDisposable const&) = delete;

    ~CancellationDisposable() noexcept;

    void Cancel() noexcept;
private:
    winrt::Windows::Foundation::IAsyncInfo m_async{ nullptr };
    std::function<void()> m_onCancel;
};

struct TaskCancellationManager final
{
    using JobId = int32_t;

    TaskCancellationManager() = default;
    ~TaskCancellationManager() noexcept;

    TaskCancellationManager(TaskCancellationManager const&) = delete;
    TaskCancellationManager& operator=(TaskCancellationManager const&) = delete;

    winrt::Windows::Foundation::IAsyncAction Add(JobId jobId, winrt::Windows::Foundation::IAsyncAction const& asyncAction) noexcept;
    void Cancel(JobId jobId) noexcept;

private:
    std::mutex m_mutex; // to protect m_pendingTasks
    std::map<JobId, CancellationDisposable> m_pendingTasks;
};

REACT_MODULE(RNFSManager, L"RNFSManager");
struct RNFSManager final
{
    REACT_INIT(Initialize)
        void Initialize(RN::ReactContext const& reactContext) noexcept;

    REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider)
        void ConstantsViaConstantsProvider(RN::ReactConstantProvider& constants) noexcept;

    REACT_METHOD(mkdir); // Implemented
    winrt::fire_and_forget mkdir(std::string directory, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(moveFile); // Implemented
    winrt::fire_and_forget moveFile(
        std::string filepath,
        std::string destPath,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(copyFile); // Implemented
    winrt::fire_and_forget copyFile(
        std::string filepath,
        std::string destPath,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(copyFolder); // Implemented
    winrt::fire_and_forget copyFolder(
        std::string src,
        std::string dest,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(getFSInfo); // Implemented, no unit tests but cannot be tested
    winrt::fire_and_forget getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(unlink); // Implemented
    winrt::fire_and_forget unlink(std::string filePath, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(exists); // Implemented
    winrt::fire_and_forget exists(std::string fullpath, RN::ReactPromise<bool> promise) noexcept;

    REACT_METHOD(stopDownload); // DOWNLOADER
    void stopDownload(int jobID) noexcept;

    REACT_METHOD(stopUpload); // DOWNLOADER
    void stopUpload(int jobID) noexcept;

    REACT_METHOD(readDir); // Implemented
    winrt::fire_and_forget readDir(std::string directory, RN::ReactPromise<RN::JSValueArray> promise) noexcept;

    REACT_METHOD(stat); // Implemented, unit tests incomplete
    winrt::fire_and_forget stat(std::string filepath, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(readFile); // Implemented
    winrt::fire_and_forget readFile(std::string filePath, RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(read); // Implemented
    winrt::fire_and_forget read(
        std::string filePath,
        uint32_t length,
        uint64_t position,
        RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(hash); // Implemented
    winrt::fire_and_forget hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(writeFile); // Implemented
    winrt::fire_and_forget writeFile(
        std::string filePath,
        std::string base64Content,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(appendFile); // Implemented, no unit tests
    winrt::fire_and_forget appendFile(
        std::string filepath,
        std::string base64Content,
        RN::ReactPromise<void> promise
    ) noexcept;


    REACT_METHOD(write); // Implemented
    winrt::fire_and_forget write(
        std::string filePath,
        std::string base64Content,
        int position,
        RN::ReactPromise<void> promise) noexcept;


    REACT_METHOD(downloadFile); // DOWNLOADER
    winrt::fire_and_forget downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(uploadFiles); // DOWNLOADER
    winrt::fire_and_forget uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(touch); // Implemented
    void touch(std::string filepath, int64_t mtime, int64_t ctime, bool modifyCreationTime, RN::ReactPromise<std::string> promise) noexcept;

    REACT_EVENT(TimedEvent, L"TimedEventCpp");
    std::function<void(int)> TimedEvent;

private:
    void splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept;

    winrt::Windows::Foundation::IAsyncAction ProcessDownloadRequestAsync(RN::ReactPromise<RN::JSValueObject> promise,
        winrt::Windows::Web::Http::HttpRequestMessage request, std::wstring_view filePath, int32_t jobId, int64_t progressInterval, int64_t progressDivider);

    winrt::Windows::Foundation::IAsyncAction ProcessUploadRequestAsync(RN::ReactPromise<RN::JSValueObject> promise, RN::JSValueObject& options,
        winrt::Windows::Web::Http::HttpMethod httpMethod, RN::JSValueArray const& files, int32_t jobId, uint64_t totalUploadSize);

    winrt::fire_and_forget copyFolderHelper(
        winrt::Windows::Storage::StorageFolder src,
        winrt::Windows::Storage::StorageFolder dest) noexcept;

    constexpr static int64_t UNIX_EPOCH_IN_WINRT_INTERVAL = 11644473600 * 10000000;
     
    const std::unordered_map<std::string, std::function<CryptographyCore::HashAlgorithmProvider()>> availableHashes{
        {"md5", []() { return CryptographyCore::HashAlgorithmProvider::OpenAlgorithm(CryptographyCore::HashAlgorithmNames::Md5()); } },
        {"sha1", []() { return CryptographyCore::HashAlgorithmProvider::OpenAlgorithm(CryptographyCore::HashAlgorithmNames::Sha1()); } },
        {"sha256", []() { return CryptographyCore::HashAlgorithmProvider::OpenAlgorithm(CryptographyCore::HashAlgorithmNames::Sha256()); } },
        {"sha384", []() { return CryptographyCore::HashAlgorithmProvider::OpenAlgorithm(CryptographyCore::HashAlgorithmNames::Sha384()); } },
        {"sha512", []() { return CryptographyCore::HashAlgorithmProvider::OpenAlgorithm(CryptographyCore::HashAlgorithmNames::Sha512()); } }
    };

    RN::ReactContext m_reactContext;
    winrt::Windows::Web::Http::HttpClient m_httpClient;
    TaskCancellationManager m_tasks;
};
