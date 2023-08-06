// TODO: Well, as the note below says, this file is supposed to be auto-generated
// from NativeModule spec file in JS, but for some reason it does not happen now
// automatically with our setup and react-native-windows@0.72.4.
// It should be resolved later, but for now we can just (re-)generate this file as
// necessary running the following command in the root of library codebase,
// then moving the generated file here from ".\codegen" folder.
//
// .\node_modules\.bin\react-native-windows-codegen
//   --libraryName RNReactNativeFsSpec
//   --file .\src\NativeReactNativeFs.ts
//   --namespace winrt::ReactNativeFs
//   --modulesWindows true

/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once

#include "NativeModules.h"
#include <tuple>

namespace winrt::ReactNativeFs {

REACT_STRUCT(ReactNativeFsSpec_FileOptions)
struct ReactNativeFsSpec_FileOptions {
    REACT_FIELD(NSFileProtectionKey)
    std::optional<std::string> NSFileProtectionKey;
};

REACT_STRUCT(ReactNativeFsSpec_NativeDownloadFileOptions)
struct ReactNativeFsSpec_NativeDownloadFileOptions {
    REACT_FIELD(jobId)
    double jobId;
    REACT_FIELD(fromUrl)
    std::string fromUrl;
    REACT_FIELD(toFile)
    std::string toFile;
    REACT_FIELD(background)
    bool background;
    REACT_FIELD(backgroundTimeout)
    double backgroundTimeout;
    REACT_FIELD(cacheable)
    bool cacheable;
    REACT_FIELD(connectionTimeout)
    double connectionTimeout;
    REACT_FIELD(discretionary)
    bool discretionary;
    REACT_FIELD(headers)
    ::React::JSValue headers;
    REACT_FIELD(progressDivider)
    double progressDivider;
    REACT_FIELD(progressInterval)
    double progressInterval;
    REACT_FIELD(readTimeout)
    double readTimeout;
    REACT_FIELD(hasBeginCallback)
    bool hasBeginCallback;
    REACT_FIELD(hasProgressCallback)
    bool hasProgressCallback;
    REACT_FIELD(hasResumableCallback)
    bool hasResumableCallback;
};

REACT_STRUCT(ReactNativeFsSpec_DownloadResult)
struct ReactNativeFsSpec_DownloadResult {
    REACT_FIELD(jobId)
    double jobId;
    REACT_FIELD(statusCode)
    double statusCode;
    REACT_FIELD(bytesWritten)
    double bytesWritten;
};

REACT_STRUCT(ReactNativeFsSpec_FSInfoResult)
struct ReactNativeFsSpec_FSInfoResult {
    REACT_FIELD(totalSpace)
    double totalSpace;
    REACT_FIELD(freeSpace)
    double freeSpace;
};

REACT_STRUCT(ReactNativeFsSpec_MkdirOptions)
struct ReactNativeFsSpec_MkdirOptions {
    REACT_FIELD(NSURLIsExcludedFromBackupKey)
    std::optional<bool> NSURLIsExcludedFromBackupKey;
    REACT_FIELD(NSFileProtectionKey)
    std::optional<std::string> NSFileProtectionKey;
};

REACT_STRUCT(ReactNativeFsSpec_NativeReadDirResItemT)
struct ReactNativeFsSpec_NativeReadDirResItemT {
    REACT_FIELD(ctime)
    double ctime;
    REACT_FIELD(mtime)
    double mtime;
    REACT_FIELD(name)
    std::string name;
    REACT_FIELD(path)
    std::string path;
    REACT_FIELD(size)
    double size;
    REACT_FIELD(type)
    std::string type;
};

REACT_STRUCT(ReactNativeFsSpec_NativeStatResult)
struct ReactNativeFsSpec_NativeStatResult {
    REACT_FIELD(ctime)
    double ctime;
    REACT_FIELD(mtime)
    double mtime;
    REACT_FIELD(size)
    double size;
    REACT_FIELD(type)
    std::string type;
    REACT_FIELD(mode)
    double mode;
    REACT_FIELD(originalFilepath)
    std::string originalFilepath;
};

REACT_STRUCT(ReactNativeFsSpec_TouchOptions)
struct ReactNativeFsSpec_TouchOptions {
    REACT_FIELD(ctime)
    std::optional<double> ctime;
    REACT_FIELD(mtime)
    std::optional<double> mtime;
};

REACT_STRUCT(ReactNativeFsSpec_UploadFileItem)
struct ReactNativeFsSpec_UploadFileItem {
    REACT_FIELD(name)
    std::string name;
    REACT_FIELD(filename)
    std::string filename;
    REACT_FIELD(filepath)
    std::string filepath;
    REACT_FIELD(filetype)
    std::string filetype;
};

REACT_STRUCT(ReactNativeFsSpec_NativeUploadFileOptions)
struct ReactNativeFsSpec_NativeUploadFileOptions {
    REACT_FIELD(toUrl)
    std::string toUrl;
    REACT_FIELD(binaryStreamOnly)
    std::optional<bool> binaryStreamOnly;
    REACT_FIELD(files)
    std::vector<ReactNativeFsSpec_UploadFileItem> files;
    REACT_FIELD(headers)
    std::optional<::React::JSValue> headers;
    REACT_FIELD(fields)
    std::optional<::React::JSValue> fields;
    REACT_FIELD(method)
    std::optional<std::string> method;
};

REACT_STRUCT(ReactNativeFsSpec_UploadResult)
struct ReactNativeFsSpec_UploadResult {
    REACT_FIELD(jobId)
    double jobId;
    REACT_FIELD(statusCode)
    double statusCode;
    REACT_FIELD(headers)
    ::React::JSValue headers;
    REACT_FIELD(body)
    std::string body;
};

REACT_STRUCT(ReactNativeFsSpec_Constants)
struct ReactNativeFsSpec_Constants {
    REACT_FIELD(CachesDirectoryPath)
    std::string CachesDirectoryPath;
    REACT_FIELD(DocumentDirectoryPath)
    std::string DocumentDirectoryPath;
    REACT_FIELD(DownloadDirectoryPath)
    std::string DownloadDirectoryPath;
    REACT_FIELD(ExternalCachesDirectoryPath)
    std::string ExternalCachesDirectoryPath;
    REACT_FIELD(ExternalDirectoryPath)
    std::string ExternalDirectoryPath;
    REACT_FIELD(ExternalStorageDirectoryPath)
    std::string ExternalStorageDirectoryPath;
    REACT_FIELD(MainBundlePath)
    std::optional<std::string> MainBundlePath;
    REACT_FIELD(TemporaryDirectoryPath)
    std::string TemporaryDirectoryPath;
    REACT_FIELD(FileTypeRegular)
    std::string FileTypeRegular;
    REACT_FIELD(FileTypeDirectory)
    std::string FileTypeDirectory;
    REACT_FIELD(DocumentDirectory)
    double DocumentDirectory;
    REACT_FIELD(LibraryDirectoryPath)
    std::optional<std::string> LibraryDirectoryPath;
    REACT_FIELD(PicturesDirectoryPath)
    std::optional<std::string> PicturesDirectoryPath;
    REACT_FIELD(RoamingDirectoryPath)
    std::optional<std::string> RoamingDirectoryPath;
    REACT_FIELD(FileProtectionKeys)
    std::optional<std::string> FileProtectionKeys;
};

struct ReactNativeFsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto constants = std::tuple{
      TypedConstant<ReactNativeFsSpec_Constants>{0},
  };
  static constexpr auto methods = std::tuple{
      Method<void(std::string) noexcept>{0, L"addListener"},
      Method<void(double) noexcept>{1, L"removeListeners"},
      Method<void(std::string, std::string, Promise<void>) noexcept>{2, L"appendFile"},
      Method<void(std::string, std::string, ReactNativeFsSpec_FileOptions, Promise<void>) noexcept>{3, L"copyFile"},
      Method<void(ReactNativeFsSpec_NativeDownloadFileOptions, Promise<ReactNativeFsSpec_DownloadResult>) noexcept>{4, L"downloadFile"},
      Method<void(std::string, Promise<bool>) noexcept>{5, L"exists"},
      Method<void(Promise<ReactNativeFsSpec_FSInfoResult>) noexcept>{6, L"getFSInfo"},
      Method<void(std::string, std::string, Promise<std::string>) noexcept>{7, L"hash"},
      Method<void(std::string, ReactNativeFsSpec_MkdirOptions, Promise<void>) noexcept>{8, L"mkdir"},
      Method<void(std::string, std::string, ReactNativeFsSpec_FileOptions, Promise<void>) noexcept>{9, L"moveFile"},
      Method<void(std::string, double, double, Promise<std::string>) noexcept>{10, L"read"},
      Method<void(std::string, Promise<std::string>) noexcept>{11, L"readFile"},
      Method<void(std::string, Promise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>>) noexcept>{12, L"readDir"},
      Method<void(std::string, Promise<ReactNativeFsSpec_NativeStatResult>) noexcept>{13, L"stat"},
      Method<void(double) noexcept>{14, L"stopDownload"},
      Method<void(double) noexcept>{15, L"stopUpload"},
      Method<void(std::string, ReactNativeFsSpec_TouchOptions, Promise<void>) noexcept>{16, L"touch"},
      Method<void(std::string, Promise<void>) noexcept>{17, L"unlink"},
      Method<void(ReactNativeFsSpec_NativeUploadFileOptions, Promise<ReactNativeFsSpec_UploadResult>) noexcept>{18, L"uploadFiles"},
      Method<void(std::string, std::string, double, Promise<void>) noexcept>{19, L"write"},
      Method<void(std::string, std::string, ReactNativeFsSpec_FileOptions, Promise<void>) noexcept>{20, L"writeFile"},
      Method<void(std::string, std::string, Promise<void>) noexcept>{21, L"copyFileAssets"},
      Method<void(std::string, std::string, Promise<void>) noexcept>{22, L"copyFileRes"},
      Method<void(std::string, Promise<bool>) noexcept>{23, L"existsAssets"},
      Method<void(std::string, Promise<bool>) noexcept>{24, L"existsRes"},
      Method<void(Promise<std::vector<std::string>>) noexcept>{25, L"getAllExternalFilesDirs"},
      Method<void(std::string, Promise<std::string>) noexcept>{26, L"readFileAssets"},
      Method<void(std::string, Promise<std::string>) noexcept>{27, L"readFileRes"},
      Method<void(std::string, Promise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>>) noexcept>{28, L"readDirAssets"},
      Method<void(std::string, Promise<std::string>) noexcept>{29, L"scanFile"},
      Method<void(std::string, bool, bool, Promise<bool>) noexcept>{30, L"setReadable"},
      Method<void(std::string, std::string, double, double, double, double, std::string, Promise<std::string>) noexcept>{31, L"copyAssetsFileIOS"},
      Method<void(std::string, std::string, Promise<std::string>) noexcept>{32, L"copyAssetsVideoIOS"},
      Method<void(double) noexcept>{33, L"completeHandlerIOS"},
      Method<void(double, Promise<bool>) noexcept>{34, L"isResumable"},
      Method<void(std::string, Promise<std::string>) noexcept>{35, L"pathForBundle"},
      Method<void(std::string, Promise<std::string>) noexcept>{36, L"pathForGroup"},
      Method<void(double) noexcept>{37, L"resumeDownload"},
      Method<void(std::string, std::string, Promise<void>) noexcept>{38, L"copyFolder"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto constantCheckResults = CheckConstants<TModule, ReactNativeFsSpec>();
    constexpr auto methodCheckResults = CheckMethods<TModule, ReactNativeFsSpec>();

    REACT_SHOW_CONSTANT_SPEC_ERRORS(
          0,
          "ReactNativeFsSpec_Constants",
          "    REACT_GET_CONSTANTS(GetConstants) ReactNativeFsSpec_Constants GetConstants() noexcept {/*implementation*/}\n"
          "    REACT_GET_CONSTANTS(GetConstants) static ReactNativeFsSpec_Constants GetConstants() noexcept {/*implementation*/}\n");

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string event) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string event) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "appendFile",
          "    REACT_METHOD(appendFile) void appendFile(std::string path, std::string b64, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(appendFile) static void appendFile(std::string path, std::string b64, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "copyFile",
          "    REACT_METHOD(copyFile) void copyFile(std::string from, std::string to, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyFile) static void copyFile(std::string from, std::string to, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "downloadFile",
          "    REACT_METHOD(downloadFile) void downloadFile(ReactNativeFsSpec_NativeDownloadFileOptions && options, ::React::ReactPromise<ReactNativeFsSpec_DownloadResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(downloadFile) static void downloadFile(ReactNativeFsSpec_NativeDownloadFileOptions && options, ::React::ReactPromise<ReactNativeFsSpec_DownloadResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "exists",
          "    REACT_METHOD(exists) void exists(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(exists) static void exists(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          6,
          "getFSInfo",
          "    REACT_METHOD(getFSInfo) void getFSInfo(::React::ReactPromise<ReactNativeFsSpec_FSInfoResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getFSInfo) static void getFSInfo(::React::ReactPromise<ReactNativeFsSpec_FSInfoResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          7,
          "hash",
          "    REACT_METHOD(hash) void hash(std::string path, std::string algorithm, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(hash) static void hash(std::string path, std::string algorithm, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          8,
          "mkdir",
          "    REACT_METHOD(mkdir) void mkdir(std::string path, ReactNativeFsSpec_MkdirOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(mkdir) static void mkdir(std::string path, ReactNativeFsSpec_MkdirOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          9,
          "moveFile",
          "    REACT_METHOD(moveFile) void moveFile(std::string from, std::string to, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(moveFile) static void moveFile(std::string from, std::string to, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          10,
          "read",
          "    REACT_METHOD(read) void read(std::string path, double length, double position, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(read) static void read(std::string path, double length, double position, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          11,
          "readFile",
          "    REACT_METHOD(readFile) void readFile(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readFile) static void readFile(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          12,
          "readDir",
          "    REACT_METHOD(readDir) void readDir(std::string path, ::React::ReactPromise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readDir) static void readDir(std::string path, ::React::ReactPromise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          13,
          "stat",
          "    REACT_METHOD(stat) void stat(std::string path, ::React::ReactPromise<ReactNativeFsSpec_NativeStatResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stat) static void stat(std::string path, ::React::ReactPromise<ReactNativeFsSpec_NativeStatResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          14,
          "stopDownload",
          "    REACT_METHOD(stopDownload) void stopDownload(double jobId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stopDownload) static void stopDownload(double jobId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          15,
          "stopUpload",
          "    REACT_METHOD(stopUpload) void stopUpload(double jobId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(stopUpload) static void stopUpload(double jobId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          16,
          "touch",
          "    REACT_METHOD(touch) void touch(std::string path, ReactNativeFsSpec_TouchOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(touch) static void touch(std::string path, ReactNativeFsSpec_TouchOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          17,
          "unlink",
          "    REACT_METHOD(unlink) void unlink(std::string path, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(unlink) static void unlink(std::string path, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          18,
          "uploadFiles",
          "    REACT_METHOD(uploadFiles) void uploadFiles(ReactNativeFsSpec_NativeUploadFileOptions && options, ::React::ReactPromise<ReactNativeFsSpec_UploadResult> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(uploadFiles) static void uploadFiles(ReactNativeFsSpec_NativeUploadFileOptions && options, ::React::ReactPromise<ReactNativeFsSpec_UploadResult> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          19,
          "write",
          "    REACT_METHOD(write) void write(std::string path, std::string b64, double position, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(write) static void write(std::string path, std::string b64, double position, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          20,
          "writeFile",
          "    REACT_METHOD(writeFile) void writeFile(std::string path, std::string b64, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(writeFile) static void writeFile(std::string path, std::string b64, ReactNativeFsSpec_FileOptions && options, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          21,
          "copyFileAssets",
          "    REACT_METHOD(copyFileAssets) void copyFileAssets(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyFileAssets) static void copyFileAssets(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          22,
          "copyFileRes",
          "    REACT_METHOD(copyFileRes) void copyFileRes(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyFileRes) static void copyFileRes(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          23,
          "existsAssets",
          "    REACT_METHOD(existsAssets) void existsAssets(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(existsAssets) static void existsAssets(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          24,
          "existsRes",
          "    REACT_METHOD(existsRes) void existsRes(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(existsRes) static void existsRes(std::string path, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          25,
          "getAllExternalFilesDirs",
          "    REACT_METHOD(getAllExternalFilesDirs) void getAllExternalFilesDirs(::React::ReactPromise<std::vector<std::string>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(getAllExternalFilesDirs) static void getAllExternalFilesDirs(::React::ReactPromise<std::vector<std::string>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          26,
          "readFileAssets",
          "    REACT_METHOD(readFileAssets) void readFileAssets(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readFileAssets) static void readFileAssets(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          27,
          "readFileRes",
          "    REACT_METHOD(readFileRes) void readFileRes(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readFileRes) static void readFileRes(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          28,
          "readDirAssets",
          "    REACT_METHOD(readDirAssets) void readDirAssets(std::string path, ::React::ReactPromise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(readDirAssets) static void readDirAssets(std::string path, ::React::ReactPromise<std::vector<ReactNativeFsSpec_NativeReadDirResItemT>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          29,
          "scanFile",
          "    REACT_METHOD(scanFile) void scanFile(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(scanFile) static void scanFile(std::string path, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          30,
          "setReadable",
          "    REACT_METHOD(setReadable) void setReadable(std::string filepath, bool readable, bool ownerOnly, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(setReadable) static void setReadable(std::string filepath, bool readable, bool ownerOnly, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          31,
          "copyAssetsFileIOS",
          "    REACT_METHOD(copyAssetsFileIOS) void copyAssetsFileIOS(std::string imageUri, std::string destPath, double width, double height, double scale, double compression, std::string resizeMode, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyAssetsFileIOS) static void copyAssetsFileIOS(std::string imageUri, std::string destPath, double width, double height, double scale, double compression, std::string resizeMode, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          32,
          "copyAssetsVideoIOS",
          "    REACT_METHOD(copyAssetsVideoIOS) void copyAssetsVideoIOS(std::string imageUri, std::string destPath, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyAssetsVideoIOS) static void copyAssetsVideoIOS(std::string imageUri, std::string destPath, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          33,
          "completeHandlerIOS",
          "    REACT_METHOD(completeHandlerIOS) void completeHandlerIOS(double jobId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(completeHandlerIOS) static void completeHandlerIOS(double jobId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          34,
          "isResumable",
          "    REACT_METHOD(isResumable) void isResumable(double jobId, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(isResumable) static void isResumable(double jobId, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          35,
          "pathForBundle",
          "    REACT_METHOD(pathForBundle) void pathForBundle(std::string bundle, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(pathForBundle) static void pathForBundle(std::string bundle, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          36,
          "pathForGroup",
          "    REACT_METHOD(pathForGroup) void pathForGroup(std::string group, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(pathForGroup) static void pathForGroup(std::string group, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          37,
          "resumeDownload",
          "    REACT_METHOD(resumeDownload) void resumeDownload(double jobId) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(resumeDownload) static void resumeDownload(double jobId) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          38,
          "copyFolder",
          "    REACT_METHOD(copyFolder) void copyFolder(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(copyFolder) static void copyFolder(std::string from, std::string to, ::React::ReactPromise<void> &&result) noexcept { /* implementation */ }\n");
  }
};

} // namespace winrt::ReactNativeFs
