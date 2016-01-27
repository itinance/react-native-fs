## react-native-fs

Native filesystem access for react-native

## Usage (iOS)

First you need to install react-native-fs:

```javascript
npm install react-native-fs --save
```

In XCode, in the project navigator, right click Libraries ➜ Add Files to [your project's name] Go to node_modules ➜ react-native-fs and add the .xcodeproj file

In XCode, in the project navigator, select your project. Add the lib*.a from the RNFS project to your project's Build Phases ➜ Link Binary With Libraries Click .xcodeproj file you added before in the project navigator and go the Build Settings tab. Make sure 'All' is toggled on (instead of 'Basic'). Look for Header Search Paths and make sure it contains both $(SRCROOT)/../react-native/React and $(SRCROOT)/../../React - mark both as recursive.

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

## API

### Constants

The following constants are available on the `RNFS` export:

`MainBundlePath` (`String`) The absolute path to the main bundle directory  
`CachesDirectoryPath` (`String`) The absolute path to the caches directory  
`DocumentDirectoryPath`  (`String`) The absolute path to the document directory

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

### `promise mkdir(filepath [, excludeFromBackup])`

Create a directory at `filepath`. Automatically creates parents and does not throw if already exists (works like Linux `mkdir -p`).

IOS only: If `excludeFromBackup` is true, then `NSURLIsExcludedFromBackupKey` attribute will be set. Apple will *reject* apps for storing offline cache data that does not have this attribute.

### `promise downloadFile(url, filepath [, beginCallback, progressCallback])`

Download file from `url` to `filepath`. Will overwrite any previously existing file.

If `beginCallback` is provided, it will be invoked once upon download starting when headers have been received and passed a single argument with the following properties:

`jobId` (`Number`) - The download job ID, required if one wishes to cancel the download. See `stopDownload`.  
`statusCode` (`Number`) - The HTTP status code  
`contentLength` (`Number`) - The total size in bytes of the download resource  
`headers` (`Map`) - The HTTP response headers from the server  

If `progressCallback` is provided, it will be invoked continuously and passed a single argument with the following properties:

`contentLength` (`Number`) - The total size in bytes of the download resource  
`bytesWritten` (`Number`) - The number of bytes written to the file so far  

Percentage can be computed easily by dividing `bytesWritten` by `contentLength`.

### `void stopDownload(jobId)`

Abort the current download job with this ID. The partial file will remain on the filesystem.

## Test / Demo app

Test app to demostrate the use of the module. Useful for testing and developing the module:

https://github.com/cjdell/react-native-fs-test
