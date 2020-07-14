// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once
#include "NativeModules.h"
#include <string>

namespace RN = winrt::Microsoft::ReactNative;

REACT_MODULE(RNFSManager, L"RNFSManager");
struct RNFSManager
{
    REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider)
        void ConstantsViaConstantsProvider(RN::ReactConstantProvider& constants) noexcept;

    REACT_METHOD(mkdir); // Implemented
    void mkdir(std::string directory, RN::JSValueObject options, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(moveFile); // Implemented, no unit tests
    winrt::fire_and_forget moveFile(
        std::string filepath,
        std::string destPath,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(copyFile); // Implemented, no unit tests
    winrt::fire_and_forget copyFile(
        std::string filepath,
        std::string destPath,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(getFSInfo);
    void getFSInfo(RN::ReactPromise<RN::JSValueArray> promise) noexcept;

    REACT_METHOD(getAllExternalFilesDirs);
    void getAllExternalFilesDirs(RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(unlink); // Implemented
    winrt::fire_and_forget unlink(std::string filePath, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(exists); // Implemented
    void exists(std::string fullpath, RN::ReactPromise<bool> promise) noexcept;

    REACT_METHOD(stopDownload);
    void stopDownload(int jobID) noexcept;

    REACT_METHOD(resumeDownload);
    void resumeDownload(int jobID) noexcept;

    REACT_METHOD(isResumable);
    void isResumable(int jobID, RN::ReactPromise<bool> promise) noexcept;

    REACT_METHOD(stopUpload);
    void stopUpload(int jobID) noexcept;

    REACT_METHOD(readDir); // Implemented
    winrt::fire_and_forget readDir(std::string directory, RN::ReactPromise<RN::JSValueArray> promise) noexcept;

    REACT_METHOD(stat);
    void stat(std::string filepath, RN::ReactPromise<RN::JSValueArray> promise) noexcept;

    REACT_METHOD(readFile); // Implemented
    winrt::fire_and_forget readFile(std::string filePath, RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(read); // Implemented
    winrt::fire_and_forget read(
        std::string filePath,
        int length,
        int position,
        RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(hash);
    void hash(std::string filepath, std::string algorithm, RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(writeFile); // Implemented
    winrt::fire_and_forget writeFile(
        std::string filePath,
        std::string base64Content,
        RN::JSValueObject options,
        RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(appendFile);
    void appendFile(
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


    REACT_METHOD(downloadFile);
    void downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(uploadFiles);
    void uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(touch);
    void touch(std::string filepath, double mtime, double ctime, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(scanFile);
    void scanFile(std::string path, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

private:
    void splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept;
};
