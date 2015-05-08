## react-native-fs

Native filesystem access for react-native

Note: this project is under development and functionality will improve over time. Currently it provides only the bare minimum of functionality.

Creating, renaming, copying, etc. of files will follow soon.

## Usage

First you need to install react-native-fs:

```javascript
npm install react-native-fs --save
```

In XCode, in the project navigator, right click Libraries ➜ Add Files to [your project's name] Go to node_modules ➜ react-native-fs and add the .xcodeproj file

In XCode, in the project navigator, select your project. Add the lib*.a from the RNFS project to your project's Build Phases ➜ Link Binary With Libraries Click .xcodeproj file you added before in the project navigator and go the Build Settings tab. Make sure 'All' is toggled on (instead of 'Basic'). Look for Header Search Paths and make sure it contains both $(SRCROOT)/../react-native/React and $(SRCROOT)/../../React - mark both as recursive.

Run your project (Cmd+R)

## Examples

### Basic

```javascript
// require the module
var RNFS = require('react-native-fs');

// get a list of files and directories in the main bundle
RNFS.readDir('/', RNFS.MainBundle)
  .then((result) => {
    console.log('GOT RESULT', result);

    // stat the first file
    return Promise.all([RNFS.stat(result[0].path), result[0].path]);
  })
  .then((statResult) => {
    if (statResult[0].isFile()) {
      // if we have a file, read it
      return RNFS.readFile(statResult[1]);
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
RNFS.writeFile(path, 'Lorem ipsum dolor sit amet')
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

`MainBundle` (`Number`) The identifier for the main bundle  
`CachesDirectory` (`Number`) The identifier for the caches  directory  
`DocumentDirectory` (`Number`) The identifier for the document directory

`CachesDirectoryPath` (`String`) The absolute path to the caches directory
`DocumentDirectoryPath`  (`String`) The absolute path to the document directory

### `promise readDir(path, directory)`

Reads the contents of `path` in `directory`.  
`path` is a string and `directory` is one of the following:  
`RNFS.MainBundle`, `RNFS.CachesDirectory`, `RNFS.DocumentDirectory`

The returned promise resolves with an array of objects with the following properties:

`name` (`String`), The name of the item  
`path` (`String`), The absolute path to the item

### `promise stat(path)`

Stats an item at `path`.  
The promise resolves with an object with the following properties:  
`ctime` (`Date`) - The creation date of the item
`mtime` (`Date`) - The modification date of the item
`size` (`Number`) - The size of the item in bytes  
`isFile` (`Function`) - Returns true when the item is a file  
`isDirectory` (`Function`) - Returns true when the item is a directory

### `promise readFile(path, shouldDecode)`

Reads the file at `path` and - by default - decodes the transferred base64 string. If `shouldDecode` is `false`, the base64 encoded string is returned

Note: you will take quite a performance hit if you are reading big files

### `promise writeFile(filepath, contents [, options])`

Write the `contents` to `filepath`. `options` optionally takes an object specifying the file's properties, like mode etc.

The promise resolves with a boolean.

### `promise unlink(filepath)`

Unlinks the item at `filepath`. If the item does not exists, an error will be thrown.

The promise resolves with an array, which contains a boolean and the path that has been unlinked. Tip: use `spread` to receive the two arguments instead of a single array in your handler.
