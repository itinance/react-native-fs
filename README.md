## react-native-fs

Native filesystem access for react-native

Note: this project is under development and functionality will improve over time. Currently it provides only the bare minimum of functionality.

Renaming, copying, and creating files will follow soon.

## Usage

First you need to install react-native-fs:

```javascript
npm install react-native-fs --save
```

In XCode, in the project navigator, right click Libraries ➜ Add Files to [your project's name] Go to node_modules ➜ react-native-keyboardevents and add the .xcodeproj file

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

## API

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
