// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once
#include "NativeModules.h"
#include <string>

namespace RN = winrt::Microsoft::ReactNative;

REACT_MODULE(RNFSManager, L"RNFSManager");
struct RNFSManager
{
    REACT_CONSTANT_PROVIDER(ConstantsViaConstantsProvider) // Implemented, but unsure how this works
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

    REACT_METHOD(getFSInfo); // Implemented, no unit tests but cannot be tested
    winrt::fire_and_forget getFSInfo(RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    //REACT_METHOD(getAllExternalFilesDirs); // TODO: Check to see what design consensus would be. Unlikely that we need it
    //void getAllExternalFilesDirs(RN::ReactPromise<std::string> promise) noexcept;

    REACT_METHOD(unlink); // Implemented
    winrt::fire_and_forget unlink(std::string filePath, RN::ReactPromise<void> promise) noexcept;

    REACT_METHOD(exists); // Implemented
    void exists(std::string fullpath, RN::ReactPromise<bool> promise) noexcept;

    REACT_METHOD(stopDownload); //DOWNLOADER
    void stopDownload(int jobID) noexcept;

    REACT_METHOD(stopUpload); //DOWNLOADER
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


    REACT_METHOD(downloadFile); //DOWNLOADER
    void downloadFile(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(uploadFiles); //DOWNLOADER
    void uploadFiles(RN::JSValueObject options, RN::ReactPromise<RN::JSValueObject> promise) noexcept;

    REACT_METHOD(touch); // Implemented, unit tests incomplete
    void touch(std::string filepath, double mtime, double ctime, RN::ReactPromise<void> promise) noexcept;

private:
    void splitPath(const std::string& fullPath, winrt::hstring& directoryPath, winrt::hstring& fileName) noexcept;
};
