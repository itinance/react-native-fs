## react-native-fs

Native filesystem access for react-native

## IMPORTANT

For RN < 0.57 and/or Gradle < 3 you MUST install react-native-fs at version @2.11.17!

For RN >= 0.57 and/or Gradle >= 3 you MUST install react-native-fs at version @2.12!

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

## Usage (iOS)

First you need to install react-native-fs:

```
npm install react-native-fs --save
```

**Note:** If your react-native version is < 0.40 install with this tag instead:

```
npm install react-native-fs@2.0.1-rc.2 --save
```

As @a-koka pointed out, you should then update your package.json to
`"react-native-fs": "2.0.1-rc.2"` (without the tilde)

### Adding automatically with react-native link

At the command line, in your project folder, type:

`react-native link react-native-fs`

Done! No need to worry about manually adding the library to your project.

###  Adding with CocoaPods

 Add the RNFS pod to your list of application pods in your Podfile, using the path from the Podfile to the installed module:~~

```
pod 'RNFS', :path => '../node_modules/react-native-fs'
```

Install pods as usual:
```
pod install
```

### Adding Manually in XCode

In XCode, in the project navigator, right click Libraries ➜ Add Files to [your project's name] Go to node_modules ➜ react-native-fs and add the .xcodeproj file

In XCode, in the project navigator, select your project. Add the `lib*.a` from the RNFS project to your project's Build Phases ➜ Link Binary With Libraries. Click the .xcodeproj file you added before in the project navigator and go the Build Settings tab. Make sure 'All' is toggled on (instead of 'Basic'). Look for Header Search Paths and make sure it contains both `$(SRCROOT)/../react-native/React` and `$(SRCROOT)/../../React` - mark both as recursive.

Run your project (Cmd+R)

## Usage (Android)

Android support is currently limited to only the `DocumentDirectory`. This maps to the app's `files` directory.

Make alterations to the following files:

* `android/settings.gradle`

```gradle
...
include ':react-native-fs'
project(':react-native-fs').projectDir = new File(settingsDir, '../node_modules/react-native-fs/android')
```

* `android/app/build.gradle`

```gradle
...
dependencies {
    ...
    compile project(':react-native-fs')
}
```

* register module (in MainActivity.java)

  * For react-native below 0.19.0 (use `cat ./node_modules/react-native/package.json | grep version`)

```java
import com.rnfs.RNFSPackage;  // <--- import

public class MainActivity extends Activity implements DefaultHardwareBackBtnHandler {

  ......

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    mReactRootView = new ReactRootView(this);

    mReactInstanceManager = ReactInstanceManager.builder()
      .setApplication(getApplication())
      .setBundleAssetName("index.android.bundle")
      .setJSMainModuleName("index.android")
      .addPackage(new MainReactPackage())
      .addPackage(new RNFSPackage())      // <------- add package
      .setUseDeveloperSupport(BuildConfig.DEBUG)
      .setInitialLifecycleState(LifecycleState.RESUMED)
      .build();

    mReactRootView.startReactApplication(mReactInstanceManager, "ExampleRN", null);

    setContentView(mReactRootView);
  }

  ......

}
```

  * For react-native 0.19.0 and higher
```java
import com.rnfs.RNFSPackage; // <------- add package

public class MainActivity extends ReactActivity {
   // ...
    @Override
    protected List<ReactPackage> getPackages() {
      return Arrays.<ReactPackage>asList(
        new MainReactPackage(), // <---- add comma
        new RNFSPackage() // <---------- add package
      );
    }
```

  * For react-native 0.29.0 and higher ( in MainApplication.java )
```java
import com.rnfs.RNFSPackage; // <------- add package

public class MainApplication extends Application implements ReactApplication {
   // ...
    @Override
    protected List<ReactPackage> getPackages() {
      return Arrays.<ReactPackage>asList(
        new MainReactPackage(), // <---- add comma
        new RNFSPackage() // <---------- add package
      );
    }
```

## Usage (Windows)

### Adding automatically with react-native link

The `link` command also works for adding the native dependency on Windows:

`react-native link react-native-fs`

### Adding Manually in Visual Studio

Follow the instructions in the ['Linking Libraries'](https://github.com/Microsoft/react-native-windows/blob/master/docs/LinkingLibrariesWindows.md) documentation on the react-native-windows GitHub repo. For the first step of adding the project to the Visual Studio solution file, the path to the project should be `../node_modules/react-native-fs/windows/RNFS/RNFS.csproj`.

## Examples

### Basic

```javascript
// require the module
var RNFS = require('react-native-fs');

// get a list of files and directories in the main bundle
RNFS.readDir(RNFS.MainBundlePath) // On Android, use "RNFS.DocumentDirectoryPath" (MainBundlePath is not defined)
  .then((result) => {
    console.log('GOT RESULT', result);

    // stat the first file
    return Promise.all([RNFS.stat(result[0].path), result[0].path]);
  })
  .then((statResult) => {
    if (statResult[0].isFile()) {
      // if we have a file, read it
      return RNFS.readFile(statResult[1], 'utf8');
    }

    return 'no file';
  })
  .then((contents) => {
    // log the file contents
    console.log(contents);
  })
  .catch((err) => {
    console.log(err.message, err.code);
  });
```

### File creation

```javascript
// require the module
var RNFS = require('react-native-fs');

// create a path you want to write to
// :warning: on iOS, you cannot write into `RNFS.MainBundlePath`,
// but `RNFS.DocumentDirectoryPath` exists on both platforms and is writable
var path = RNFS.DocumentDirectoryPath + '/test.txt';

// write the file
RNFS.writeFile(path, 'Lorem ipsum dolor sit amet', 'utf8')
  .then((success) => {
    console.log('FILE WRITTEN!');
  })
  .catch((err) => {
    console.log(err.message);
  });

```

### File deletion
```javascript
// create a path you want to delete
var path = RNFS.DocumentDirectoryPath + '/test.txt';

return RNFS.unlink(path)
  .then(() => {
    console.log('FILE DELETED');
  })
  // `unlink` will throw an error, if the item to unlink does not exist
  .catch((err) => {
    console.log(err.message);
  });
```

### File upload (iOS only)

```javascript
// require the module
var RNFS = require('react-native-fs');

var uploadUrl = 'http://requestb.in/XXXXXXX';  // For testing purposes, go to http://requestb.in/ and create your own link
// create an array of objects of the files you want to upload
var files = [
  {
    name: 'test1',
    filename: 'test1.w4a',
    filepath: RNFS.DocumentDirectoryPath + '/test1.w4a',
    filetype: 'audio/x-m4a'
  }, {
    name: 'test2',
    filename: 'test2.w4a',
    filepath: RNFS.DocumentDirectoryPath + '/test2.w4a',
    filetype: 'audio/x-m4a'
  }
];

var uploadBegin = (response) => {
  var jobId = response.jobId;
  console.log('UPLOAD HAS BEGUN! JobId: ' + jobId);
};

var uploadProgress = (response) => {
  var percentage = Math.floor((response.totalBytesSent/response.totalBytesExpectedToSend) * 100);
  console.log('UPLOAD IS ' + percentage + '% DONE!');
};

// upload files
RNFS.uploadFiles({
  toUrl: uploadUrl,
  files: files,
  method: 'POST',
  headers: {
    'Accept': 'application/json',
  },
  fields: {
    'hello': 'world',
  },
  begin: uploadBegin,
  progress: uploadProgress
}).promise.then((response) => {
    if (response.statusCode == 200) {
      console.log('FILES UPLOADED!'); // response.statusCode, response.headers, response.body
    } else {
      console.log('SERVER ERROR');
    }
  })
  .catch((err) => {
    if(err.description === "cancelled") {
      // cancelled by user
    }
    console.log(err);
  });

```

## API

### Constants

The following constants are available on the `RNFS` export:

- `MainBundlePath` (`String`) The absolute path to the main bundle directory (not available on Android)
- `CachesDirectoryPath` (`String`) The absolute path to the caches directory
- `ExternalCachesDirectoryPath` (`String`) The absolute path to the external caches directory (android only)
- `DocumentDirectoryPath`  (`String`) The absolute path to the document directory
- `TemporaryDirectoryPath` (`String`) The absolute path to the temporary directory (falls back to Caching-Directory on Android)
- `LibraryDirectoryPath` (`String`) The absolute path to the NSLibraryDirectory (iOS only)
- `ExternalDirectoryPath` (`String`) The absolute path to the external files, shared directory (android only)
- `ExternalStorageDirectoryPath` (`String`) The absolute path to the external storage, shared directory (android only)

IMPORTANT: when using `ExternalStorageDirectoryPath` it's necessary to request permissions (on Android) to read and write on the external storage, here an example: [React Native Offical Doc] (https://facebook.github.io/react-native/docs/permissionsandroid)

### `readDir(dirpath: string): Promise<ReadDirItem[]>`

Reads the contents of `path`. This must be an absolute path. Use the above path constants to form a usable file path.

The returned promise resolves with an array of objects with the following properties:

```
type ReadDirItem = {
  ctime: date;     // The creation date of the file (iOS only)
  mtime: date;     // The last modified date of the file
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};
```

### `readDirAssets(dirpath: string): Promise<ReadDirItem[]>`

Reads the contents of `dirpath ` in the Android app's assets folder.
`dirpath ` is the relative path to the file from the root of the `assets` folder.

The returned promise resolves with an array of objects with the following properties:

```
type ReadDirItem = {
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes.
  						// Note that the size of files compressed during the creation of the APK (such as JSON files) cannot be determined.
  						// `size` will be set to -1 in this case.
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};
```

Note: Android only.

### `readdir(dirpath: string): Promise<string[]>`

Node.js style version of `readDir` that returns only the names. Note the lowercase `d`.

### `stat(filepath: string): Promise<StatResult>`

Stats an item at `filepath`. If the `filepath` is linked to a virtual file, for example Android Content URI, the `originalPath` can be used to find the pointed file path.
The promise resolves with an object with the following properties:

```
type StatResult = {
  path:            // The same as filepath argument
  ctime: date;     // The creation date of the file
  mtime: date;     // The last modified date of the file
  size: string;     // Size in bytes
  mode: number;     // UNIX file mode
  originalFilepath: string;    // ANDROID: In case of content uri this is the pointed file path, otherwise is the same as path
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};
```

### `readFile(filepath: string, encoding?: string): Promise<string>`

Reads the file at `path` and return contents. `encoding` can be one of `utf8` (default), `ascii`, `base64`. Use `base64` for reading binary files.

Note: you will take quite a performance hit if you are reading big files

### `read(filepath: string, length = 0, position = 0, encodingOrOptions?: any): Promise<string>`

Reads `length` bytes from the given `position` of the file at `path` and returns contents. `encoding` can be one of `utf8` (default), `ascii`, `base64`. Use `base64` for reading binary files.

Note: reading big files piece by piece using this method may be useful in terms of performance.

### `readFileAssets(filepath:string, encoding?: string): Promise<string>`

Reads the file at `path` in the Android app's assets folder and return contents. `encoding` can be one of `utf8` (default), `ascii`, `base64`. Use `base64` for reading binary files.

`filepath` is the relative path to the file from the root of the `assets` folder.

Note: Android only.

### `writeFile(filepath: string, contents: string, encoding?: string): Promise<void>`

Write the `contents` to `filepath`. `encoding` can be one of `utf8` (default), `ascii`, `base64`. `options` optionally takes an object specifying the file's properties, like mode etc.

### `appendFile(filepath: string, contents: string, encoding?: string): Promise<void>`

Append the `contents` to `filepath`. `encoding` can be one of `utf8` (default), `ascii`, `base64`.

### `write(filepath: string, contents: string, position?: number, encoding?: string): Promise<void>`

Write the `contents` to `filepath` at the given random access position. When `position` is `undefined` or `-1` the contents is appended to the end of the file. `encoding` can be one of `utf8` (default), `ascii`, `base64`.

### `moveFile(filepath: string, destPath: string): Promise<void>`

Moves the file located at `filepath` to `destPath`. This is more performant than reading and then re-writing the file data because the move is done natively and the data doesn't have to be copied or cross the bridge.

### `copyFile(filepath: string, destPath: string): Promise<void>`

Copies the file located at `filepath` to `destPath`.

Note: On Android copyFile will overwrite `destPath` if it already exists. On iOS an error will be thrown if the file already exists.

### `copyFileAssets(filepath: string, destPath: string): Promise<void>`

Copies the file at `filepath` in the Android app's assets folder and copies it to the given `destPath ` path.

Note: Android only. Will overwrite destPath if it already exists

### `copyAssetsFileIOS(imageUri: string, destPath: string, width: number, height: number, scale : number = 1.0, compression : number = 1.0, resizeMode : string = 'contain'  ): Promise<string>`

iOS-only: copies a file from camera-roll, that is prefixed with "assets-library://asset/asset.JPG?..."
to a specific destination. It will download the original from iCloud if necessary.

If width and height is > 0, the image will be resized to a specific size and a specific compression rate.
If scale is below 1, the image will be scaled according to the scale-factor (between 0.0 and 1.0)
The resizeMode is also considered.

*Video-Support:*

One can use this method also to create a thumbNail from a video in a specific size.
Currently it is impossible to specify a concrete position, the OS will decide wich
Thumbnail you'll get then.
To copy a video from assets-library and save it as a mp4-file, refer to copyAssetsVideoIOS.

Further information: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
The promise will on success return the final destination of the file, as it was defined in the destPath-parameter.

### copyAssetsVideoIOS(videoUri: string, destPath: string): Promise<string>

iOS-only: copies a video from assets-library, that is prefixed with 'assets-library://asset/asset.MOV?...'
to a specific destination.

### `unlink(filepath: string): Promise<void>`

Unlinks the item at `filepath`. If the item does not exist, an error will be thrown.

Also recursively deletes directories (works like Linux `rm -rf`).

### `exists(filepath: string): Promise<boolean>`

Check if the item exists at `filepath`. If the item does not exist, return false.

### `existsAssets(filepath: string): Promise<boolean>`

Check in the Android assets folder if the item exists. `filepath` is the relative path from the root of the assets folder. If the item does not exist, return false.

### `hash(filepath: string, algorithm: string): Promise<string>`

Reads the file at `path` and returns its checksum as determined by `algorithm`, which can be one of `md5`, `sha1`, `sha224`, `sha256`, `sha384`, `sha512`.

### `touch(filepath: string, mtime?: Date, ctime?: Date): Promise<string>`

Sets the modification timestamp `mtime` and creation timestamp `ctime` of the file at `filepath`. Setting `ctime` is only supported on iOS, android always sets both timestamps to `mtime`.

### `mkdir(filepath: string, options?: MkdirOptions): Promise<void>`

```
type MkdirOptions = {
  NSURLIsExcludedFromBackupKey?: boolean; // iOS only
};
```

Create a directory at `filepath`. Automatically creates parents and does not throw if already exists (works like Linux `mkdir -p`).

(IOS only): The `NSURLIsExcludedFromBackupKey` property can be provided to set this attribute on iOS platforms. Apple will *reject* apps for storing offline cache data that does not have this attribute.

### `downloadFile(options: DownloadFileOptions): { jobId: number, promise: Promise<DownloadResult> }`

```
type DownloadFileOptions = {
  fromUrl: string;          // URL to download file from
  toFile: string;           // Local filesystem path to save the file to
  headers?: Headers;        // An object of headers to be passed to the server
  background?: boolean;     // Continue the download in the background after the app terminates (iOS only)
  discretionary?: boolean;  // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
  cacheable?: boolean;      // Whether the download can be stored in the shared NSURLCache (iOS only, defaults to true)
  progressDivider?: number;
  begin?: (res: DownloadBeginCallbackResult) => void;
  progress?: (res: DownloadProgressCallbackResult) => void;
  resumable?: () => void;    // only supported on iOS yet
  connectionTimeout?: number // only supported on Android yet
  readTimeout?: number       // supported on Android and iOS
};
```
```
type DownloadResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number;     // The HTTP status code
  bytesWritten: number;   // The number of bytes written to the file
};
```

Download file from `options.fromUrl` to `options.toFile`. Will overwrite any previously existing file.

If `options.begin` is provided, it will be invoked once upon download starting when headers have been received and passed a single argument with the following properties:

```
type DownloadBeginCallbackResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number;     // The HTTP status code
  contentLength: number;  // The total size in bytes of the download resource
  headers: Headers;       // The HTTP response headers from the server
};
```

If `options.progress` is provided, it will be invoked continuously and passed a single argument with the following properties:

```
type DownloadProgressCallbackResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  contentLength: number;  // The total size in bytes of the download resource
  bytesWritten: number;   // The number of bytes written to the file so far
};
```

If `options.progressDivider` is provided, it will return progress events that divided by `progressDivider`.

For example, if `progressDivider` = 10, you will receive only ten callbacks for this values of progress: 0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100
Use it for performance issues.
If `progressDivider` = 0, you will receive all `progressCallback` calls, default value is 0.

(IOS only): `options.background` (`Boolean`) - Whether to continue downloads when the app is not focused (default: `false`)
                           This option is currently only available for iOS, see the [Background Downloads Tutorial (iOS)](#background-downloads-tutorial-ios) section.

(IOS only): If `options.resumable` is provided, it will be invoked when the download has stopped and and can be resumed using `resumeDownload()`.

### `stopDownload(jobId: number): void`

Abort the current download job with this ID. The partial file will remain on the filesystem.

### (iOS only) `resumeDownload(jobId: number): void`

Resume the current download job with this ID.

### (iOS only) `isResumable(jobId: number): Promise<bool>`

Check if the the download job with this ID is resumable with `resumeDownload()`.

Example:

```
if (await RNFS.isResumable(jobId) {
    RNFS.resumeDownload(jobId)
}
```

### (iOS only) `completeHandlerIOS(jobId: number): void`

For use when using background downloads, tell iOS you are done handling a completed download.

Read more about background downloads in the [Background Downloads Tutorial (iOS)](#background-downloads-tutorial-ios) section.

### (iOS only) `uploadFiles(options: UploadFileOptions): { jobId: number, promise: Promise<UploadResult> }`

`options` (`Object`) - An object containing named parameters

```
type UploadFileOptions = {
  toUrl: string;            // URL to upload file to
  files: UploadFileItem[];  // An array of objects with the file information to be uploaded.
  headers?: Headers;        // An object of headers to be passed to the server
  fields?: Fields;          // An object of fields to be passed to the server
  method?: string;          // Default is 'POST', supports 'POST' and 'PUT'
  begin?: (res: UploadBeginCallbackResult) => void;
  progress?: (res: UploadProgressCallbackResult) => void;
};

```
```
type UploadResult = {
  jobId: number;        // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  statusCode: number;   // The HTTP status code
  headers: Headers;     // The HTTP response headers from the server
  body: string;         // The HTTP response body
};
```

Each file should have the following structure:

```
type UploadFileItem = {
  name: string;       // Name of the file, if not defined then filename is used
  filename: string;   // Name of file
  filepath: string;   // Path to file
  filetype: string;   // The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
};
```

If `options.begin` is provided, it will be invoked once upon upload has begun:

```
type UploadBeginCallbackResult = {
  jobId: number;        // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
};
```

If `options.progress` is provided, it will be invoked continuously and passed a single object with the following properties:

```
type UploadProgressCallbackResult = {
  jobId: number;                      // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  totalBytesExpectedToSend: number;   // The total number of bytes that will be sent to the server
  totalBytesSent: number;             // The number of bytes sent to the server
};
```

Percentage can be computed easily by dividing `totalBytesSent` by `totalBytesExpectedToSend`.

### (iOS only) `stopUpload(jobId: number): Promise<void>`

Abort the current upload job with this ID.

### `getFSInfo(): Promise<FSInfoResult>`

Returns an object with the following properties:

```
type FSInfoResult = {
  totalSpace: number;   // The total amount of storage space on the device (in bytes).
  freeSpace: number;    // The amount of available storage space on the device (in bytes).
};
```

### (Android only) `scanFile(path: string): Promise<string[]>`

Scan the file using [Media Scanner](https://developer.android.com/reference/android/media/MediaScannerConnection).

### (Android only) `getAllExternalFilesDirs(): Promise<string[]>`

Returns an array with the absolute paths to application-specific directories on all shared/external storage devices where the application can place persistent files it owns.

### (iOS only) `pathForGroup(groupIdentifier: string): Promise<string>`

`groupIdentifier` (`string`) Any value from the *com.apple.security.application-groups* entitlements list.

Returns the absolute path to the directory shared for all applications with the same security group identifier.
This directory can be used to to share files between application of the same developer.

Invalid group identifier will cause a rejection.

For more information read the [Adding an App to an App Group](https://developer.apple.com/library/content/documentation/Miscellaneous/Reference/EntitlementKeyReference/Chapters/EnablingAppSandbox.html#//apple_ref/doc/uid/TP40011195-CH4-SW19) section.

## Background Downloads Tutorial (iOS)

Background downloads in iOS require a bit of a setup.

First, in your `AppDelegate.m` file add the following:

```
#import <RNFSManager.h>

...

- (void)application:(UIApplication *)application handleEventsForBackgroundURLSession:(NSString *)identifier completionHandler:(void (^)())completionHandler
{
  [RNFSManager setCompletionHandlerForIdentifier:identifier completionHandler:completionHandler];
}

```

The `handleEventsForBackgroundURLSession` method is called when a background download is done and your app is not in the foreground.

We need to pass the `completionHandler` to RNFS along with its `identifier`.

The JavaScript will continue to work as usual when the download is done but now you must call `RNFS.completeHandlerIOS(jobId)` when you're done handling the download (show a notification etc.)

**BE AWARE!** iOS will give about 30 sec. to run your code after `handleEventsForBackgroundURLSession` is called and until `completionHandler`
is triggered so don't do anything that might take a long time (like unzipping), you will be able to do it after the user re-launces the app,
otherwide iOS will terminate your app.


## Test / Demo app

Test app to demostrate the use of the module. Useful for testing and developing the module:

https://github.com/cjdell/react-native-fs-test
