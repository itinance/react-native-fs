// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once
#include "NativeModules.h"
#include <string>
#include <mutex>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.Web.Http.h>

using namespace winrt::Windows::Security::Cryptography;
using namespace winrt::Windows::Security::Cryptography::Core;
using namespace winrt::Microsoft::ReactNative;

namespace RN = winrt::Microsoft::ReactNative;

struct CancellationDisposable
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

struct TaskCancellationManager
{
    using JobId = int32_t;

    TaskCancellationManager() = default;
    ~TaskCancellationManager() noexcept;

    TaskCancellationManager(TaskCancellationManager const&) = delete;
    TaskCancellationManager& operator=(TaskCancellationManager const&) = delete;

    void Add(JobId jobId, winrt::Windows::Foundation::IAsyncInfo const& async) noexcept;
    void Cancel(JobId jobId) noexcept;

private:
    std::mutex m_mutex; // to protect m_pendingTasks
    std::map<JobId, CancellationDisposable> m_pendingTasks;
};

REACT_MODULE(RNFSManager, L"RNFSManager");
struct RNFSManager
{

    REACT_INIT(Initialize)
        void Initialize(ReactContext const& reactContext) noexcept;

    REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider) // Implemented, but unsure how this works
        void ConstantsViaConstantsProvider(RN::ReactConstantProvider& constants) noexcept;

    REACT_METHOD(mkdir); // Implemented
    void mkdir(std::string directory, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept;

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

    REACT_METHOD(getFSInfo); // Implemented, no unit tests but cannot be tested
    winrt::fire_and_forget getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    //REACT_METHOD(getAllExternalFilesDirs); // TODO: Check to see what design consensus would be. Unlikely that we need it
    //void getAllExternalFilesDirs(RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(unlink); // Implemented
    winrt::fire_and_forget unlink(std::string filePath, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(exists); // Implemented
    void exists(std::string fullpath, RN::ReactPromise<bool> promise) noexcept;

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
        int length,
        int position,
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
    void uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(touch); // Implemented
    void touch(std::string filepath, double mtime, double ctime, RN::ReactPromise<void> promise) noexcept;

private:
    void splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept;

private:
    TaskCancellationManager m_tasks;
    
    const int64_t UNIX_EPOCH_IN_WINRT_SECONDS = 11644473600;

    const std::map<std::string, std::function<HashAlgorithmProvider()>> availableHashes{
        {"md5", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Md5()); } },
        {"sha1", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha1()); } },
        {"sha256", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha256()); } },
        {"sha384", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha384()); } },
        {"sha512", []() { return HashAlgorithmProvider::OpenAlgorithm(HashAlgorithmNames::Sha512()); } }
    };

    winrt::Windows::Web::Http::HttpClient _httpClient;

    ReactContext _reactContext;

    REACT_EVENT(TimedEvent, L"TimedEventCpp");
    std::function<void(int)> TimedEvent;
};
