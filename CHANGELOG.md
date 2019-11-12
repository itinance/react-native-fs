# Changelog

## Changes for v2.16 (for RN >= 0.61)
- #783 Import RCTImageLoaderProtocol instead of RCTImageLoader to fix iOS build on RN>0.60
- #808 Should not send tail string for binaryStreamOnly option on Android

## Changes for v2.15
- #717 bugfix #443: Add option progressInterval for downloadFile 
- #759 Fix for issue #749: RNFS.uploadFiles upload raw not multipart 
- #728 Correctly read binaryStreamOnly param
- #752 Fix Xcode and Java deprecation warnings 
- #779 Add conditional comments around methods not supported in Mac Catalyst
- #736 Added support for ph:// uris to copyAssetsFileIOS

## Changes for v2.14
- #718 Add tvOS deployment target to podspec
- #710 Added existsRes function to Android
- #702 Fix: S3 pre signed uploads or similar binary stream uploads writing corrupt files
- #695 Added .Net46 side of windows support
- #691 Add `androidx` suppport
- #669 Changed "size" from Int to Double
- #660 add a custom error for cancelled downloadFile Requests

## Changes for v2.13
- #544 [Android] Add scanFile method
- #597 [Android] Perform copyFile in background thread to prevent UI blocking
- #587 [Windows] Fixed implementation for Windows
- #585 [Android] Fix EISDIR on stat directory
- #583 [Android] fix Android downloadFile overflow contentLength and bytesWritten

## Changes for v2.12
- #601 [iOS] Another fix for copyAssetsVideoIOS
- #599 [iOS] Fix for copyAssetsVideoIOS regarding iCloud-Files
- #564 [Android] Upgrade to Gradle 3 (BREAKING compatiblity for < RN 0.57)
- #571 [Android] Fix issue #566 android progress callback not sync and handle uppercase file extension mimetype

## Changes for v2.11
- Prepared for RN 0.56 thanx to [#535](https://github.com/itinance/react-native-fs/pull/535) by [rmevans9](https://github.com/rmevans9)
- #503 make sure to return the original file uri if content:// scheme is not used
- #510 Fixes an IndexOutOfBounds while uploading files in Android
- #515 Add cacheable option to downloadFile on iOScompletion callback
- #516 [iOS] Ensure _bytesWritten is correct in download
- #519 updated compilesdkversion and buildtoolsversion
- #535 Make this work with RN56
- #558 [Android] fixed missing parameter in movefile and writefile
- #557 [Android] copyFile: fix missing parameter on Android
- #564 [Android] Replace deprecated 'compile' gradle configuration with 'implementation

## Changes for v2.10
- UploadFiles is now also available for Android [#486](https://github.com/itinance/react-native-fs/pull/486) by [hank121314](https://github.com/hank121314)
- Fixed a memory leak that caused after running many simultaneous upload jobs on iOS [#502](https://github.com/itinance/react-native-fs/pull/502) by [Ignigena](https://github.com/Ignigena)
- Android: Resolve filepath for content URI [480](https://github.com/itinance/react-native-fs/pull/480) by [andtos90](https://github.com/andtos90)
- (Android only) Add ExternalCachesDirectoryPath [490](https://github.com/itinance/react-native-fs/pull/490) by [superandrew213](https://github.com/superandrew213)

## Changes for v2.9
- (iOS only) Resumable downloads and better background downloads handling [#335](https://github.com/itinance/react-native-fs/pull/335) by [ptelad](https://github.com/ptelad)
- (Android only) getAllExternalFilesDirs() added by [ngrj](https://github.com/ngrj)
- Content URI support [#395](https://github.com/itinance/react-native-fs/pull/395) by [krzysztof-miemiec](https://github.com/krzysztof-miemiec)
- Fixed Cocoapods-Installation

## Changes for v2.8
- Fix for [#346](https://github.com/itinance/react-native-fs/pull/347): compressed file assets are detected as directories thx to [jacargentina](https://github.com/jacargentina)
- added support for Video-Assets on iOS (copyAssetsVideoIOS) and setReadable() on Android by [itinance](https://github.com/itinance)
- Added react-native-windows support for UWP [#337](https://github.com/itinance/react-native-fs/pull/337) thx to [rozele](https://github.com/rozele)
- Expose the iOS `discretionary` flag on `downloadFile` [#360](https://github.com/itinance/react-native-fs/pull/360) thx to [jamesreggio](https://github.com/jamesreggio)

## Changes for v2.5
- breaking change for RN 0.47 at android (https://github.com/facebook/react-native/releases/tag/v0.47.0)

## Changes for v2.4
- Made new thread for other native processes [ANDROID] (https://github.com/itinance/react-native-fs/commit/ad36b078db9728489155a55c1b7daa42ed191945) thx to [codesinghanoop](https://github.com/codesinghanoop)
- Upgrade gradle build tools to 25 (https://github.com/itinance/react-native-fs/commit/239bccb9d56fe9308daafb86920ed29eb9e5cfe4) thx to [markusguenther](https://github.com/markusguenther)
- Fixed Podfile Path-Error (https://github.com/itinance/react-native-fs/commit/9fd51e7e977400f3194c100af88b4c25e7510530) thx to [colorfulberry](https://github.com/colorfulberry)
- Add read-method with length and position params (https://github.com/itinance/react-native-fs/commit/a39c22be81f0c1f2263dbe60f3cd6cfcc902d2ac) thx to [simitti](https://github.com/simitii)

## Changes for v2.3

- React-Native 0.40 is minimum required for compiling on iOS (otherwise install an older release, see below)
- Access to iOS-based "assets-library" is now supported with `copyAssetsFileIOS`
- `readDir` will return now creation- and modification-time of files as with `stat()` (thanks @Ignigena)
- optional connectionTimeout and readTimeout-Settings on `downloadFile` for Android (thanks @drunksaint)

## Breaking change in v2.0

- Removed attributes from `writeFile` and `appendFile` for iOS / Android consistency
- `downloadFile` takes `options` object rather than parameters
- `stopDownload` will cause the rejection of promise returned by `downloadFile`
- `uploadFiles` promise result `response` property is now `body`
- A boolean is no longer returned from any method except `exists`
- `downloadFile` and `uploadFiles` return an object of the form `{ jobId: number, promise: Promise }`
- `mkdir` takes optional 2nd parameter `options` for iOS users to set the `NSURLIsExcludedFromBackupKey` attribute
