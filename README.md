## react-native-fs

Native filesystem access for react-native

## Usage (iOS)

First you need to install react-native-fs:

```javascript
npm install react-native-fs --save
```

### Adding with CocoaPods

Add the RNFS pod to your list of application pods in your Podfile, using the path from the Podfile to the installed module:

```
pod 'RNFS', :path => './node_modules/react-native-fs'
```

Install pods as usual:
```
pod install
```

### Adding Manually in XCode

In XCode, in the project navigator, right click Libraries ➜ Add Files to [your project's name] Go to node_modules ➜ react-native-fs and add the .xcodeproj file

In XCode, in the project navigator, select your project. Add the `lib*.a` from the RNFS project to your project's Build Phases ➜ Link Binary With Libraries Click .xcodeproj file you added before in the project navigator and go the Build Settings tab. Make sure 'All' is toggled on (instead of 'Basic'). Look for Header Search Paths and make sure it contains both `$(SRCROOT)/../react-native/React` and `$(SRCROOT)/../../React` - mark both as recursive.

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

## Examples

### Basic

```javascript
// require the module
var RNFS = require('react-native-fs');

// get a list of files and directories in the main bundle
RNFS.readDir(RNFS.MainBundlePath)
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
  // spread is a method offered by bluebird to allow for more than a
  // single return value of a promise. If you use `then`, you will receive
  // the values inside of an array
  .spread((success, path) => {
    console.log('FILE DELETED', success, path);
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
// create an object of options
var options = {
  method: 'POST',
  headers: {
    'Accept': 'application/json',
  },
  fields: {
    'hello': 'world',
  }
};

var uploadBegin = (response) => {
  var jobId = response.jobId;
  console.log('UPLOAD HAS BEGUN! JobId: ' + jobId);
};

var uploadProgress = (response) => {
  var percentage = Math.floor((response.totalBytesSent/response.totalBytesExpectedToSend) * 100);
  console.log('UPLOAD IS ' + percentage + '% DONE!');
};

// upload files
RNFS.uploadFiles(uploadUrl, files, options, uploadBegin, uploadProgress)
  .then((response) => {
    console.log('FILES UPLOADED!');
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

- `MainBundlePath` (`String`) The absolute path to the main bundle directory
- `CachesDirectoryPath` (`String`) The absolute path to the caches directory
- `DocumentDirectoryPath`  (`String`) The absolute path to the document directory
- `TemporaryDirectoryPath` (`String`) The absolute path to the temporary directory (iOS only)
- `ExternalDirectoryPath` (`String`) The absolute path to the external, shared directory (android only)

### `promise readDir(path)`

Reads the contents of `path`. This must be an absolute path. Use the above path constants to form a usable file path.

The returned promise resolves with an array of objects with the following properties:

`name` (`String`) - The name of the item
`path` (`String`) - The absolute path to the item
`size` (`Number`) - Size in bytes

### `promise readdir(path)`

Node.js style version of `readDir` that returns only the names. Note the lowercase `d`.

### `promise stat(path)`

Stats an item at `path`.
The promise resolves with an object with the following properties:

`ctime` (`Date`) - The creation date of the item
`mtime` (`Date`) - The modification date of the item
`size` (`Number`) - The size of the item in bytes
`isFile` (`Function`) - Returns true when the item is a file
`isDirectory` (`Function`) - Returns true when the item is a directory

### `promise readFile(path [, encoding])`

Reads the file at `path` and return contents. `encoding` can be one of `utf8` (default), `ascii`, `base64`. Use `base64` for reading binary files.

Note: you will take quite a performance hit if you are reading big files

### `promise writeFile(filepath, contents [, encoding, options])`

Write the `contents` to `filepath`. `encoding` can be one of `utf8` (default), `ascii`, `base64`. `options` optionally takes an object specifying the file's properties, like mode etc.

The promise resolves with a boolean.

### `promise moveFile(filepath, destPath)`

Moves the file located at `filepath` to `destPath`. This is more performant than reading and then re-writing the file data because the move is done natively and the data doesn't have to be copied or cross the bridge.

### `promise unlink(filepath)`

Unlinks the item at `filepath`. If the item does not exist, an error will be thrown.

The promise resolves with an array, which contains a boolean and the path that has been unlinked. Tip: use `spread` to receive the two arguments instead of a single array in your handler.

Also recursively deletes directories (works like Linux `rm -rf`).

### `promise exists(filepath)`

check if the item exist at `filepath`. If the item does not exist, return false.

The promise resolves with boolean.

### `promise mkdir(filepath [, excludeFromBackup])`

Create a directory at `filepath`. Automatically creates parents and does not throw if already exists (works like Linux `mkdir -p`).

(IOS only): If `excludeFromBackup` is true, then `NSURLIsExcludedFromBackupKey` attribute will be set. Apple will *reject* apps for storing offline cache data that does not have this attribute.

### `promise downloadFile(options)`

`options` (`Object`) - An object containing named parameters

```
{
  fromUrl (String) - URL to download file from
  toFile (String) - Local filesystem path to save the file to
  headers (Object) - (Optional) An object of headers to be passed to the server
  background (Boolean) - (Optional) See below
  begin (Function) - (Optional) See below
  progress (Function) - (Optional) See below
}
```

Download file from `options.fromUrl` to `options.toFile`. Will overwrite any previously existing file.

If `options.begin` is provided, it will be invoked once upon download starting when headers have been received and passed a single argument with the following properties:

`jobId` (`Number`) - The download job ID, required if one wishes to cancel the download. See `stopDownload`.
`statusCode` (`Number`) - The HTTP status code
`contentLength` (`Number`) - The total size in bytes of the download resource
`headers` (`Map`) - The HTTP response headers from the server

If `options.progress` is provided, it will be invoked continuously and passed a single argument with the following properties:

`contentLength` (`Number`) - The total size in bytes of the download resource
`bytesWritten` (`Number`) - The number of bytes written to the file so far

Percentage can be computed easily by dividing `bytesWritten` by `contentLength`.

(IOS only): `options.background` (`Boolean`) - Whether to continue downloads when the app is not focused (default: `false`)
                           This option is currently only available for iOS, and you must [enable
                           background fetch](https://www.objc.io/issues/5-ios7/multitasking/#background-fetch<Paste>)
                           for your project in XCode.


### `void stopDownload(jobId)`

Abort the current download job with this ID. The partial file will remain on the filesystem.

### (iOS only) `promise uploadFiles(options)`

`options` (`Object`) - An object containing named parameters

```
{
  url (String) - URL to upload file to
  files (Array) - An array of objects with the file information to be uploaded.
  method (String) - (Optional) Default is 'POST', supports 'POST' and 'PUT'
  headers (Object) - (Optional) An object of headers to be passed to the server
  fields (Object) - (Optional) An object of fields to be passed to the server
  begin (Function) - (Optional) See below
  progress (Function) - (Optional) See below
}
```

`options.files` (`Array`) =

```
[
  {
    name (String) - (Optional) Name of the file, if not defined then filename is used
    filename (String) - Name of file
    filepath (String) - Path to file
    mimetype (String) - (Optional) The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
  }, {
    ...
  }
]
```

If `options.begin` is provided, it will be invoked once upon upload has begun:

`jobId` (`Number`) - The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.

If `options.progress` is provided, it will be invoked continuously and passed a single object with the following properties:

`totalBytesExpectedToSend` (`Number`) - The total number of bytes that will be sent to the server
`totalBytesSent` (`Number`) - The number of bytes sent to the server

Percentage can be computed easily by dividing `totalBytesSent` by `totalBytesExpectedToSend`.

### (iOS only) `void stopUpload(jobId)`

Abort the current upload job with this ID.

### `promise getFSInfo()`

Returns an object with the following properties:

`totalSpace` (`Number`): The total amount of storage space on the device (in bytes).
`freeSpace` (`Number`): The amount of available storage space on the device (in bytes).

## Test / Demo app

Test app to demostrate the use of the module. Useful for testing and developing the module:

https://github.com/cjdell/react-native-fs-test
