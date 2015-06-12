'use strict';

var RNFSManager = require('NativeModules').RNFSManager;
var Promise = require('bluebird');

var convertError = (err) => {
  if (err.isOperational) {
    err = err.cause;
  }

  var error = new Error(err.description || err.message);
  error.code = err.code;
  throw error;
};

var NSFileTypeRegular = RNFSManager.NSFileTypeRegular;
var NSFileTypeDirectory = RNFSManager.NSFileTypeDirectory;

/**
 * This function converts the relative path to application home specific directory
 * Ex: Documents/hello.txt -> <DocumentsDirForApp>/hello.txt
 * Caches/hello.txt -> <CacheDirForApp>/hello.txt
 * tmp/hello.txt -> <TmpDirForApp>/hello.txt
 * @param  {String}
 * @return {String}
 */
function processFilePath(filepath) {
  var found = filepath.indexOf('/');
  if (found !== 0) {
    //relative path, check where is it relative to
    var splitPath = filepath.split('/');
    if (splitPath[0] === 'Documents') {
      splitPath[0] = RNFSManager.NSDocumentDirectoryPath;
      return splitPath.join('/');
    } else if (splitPath[0] === 'Caches') {
      splitPath[0] = RNFSManager.NSCachesDirectoryPath;
      return splitPath.join('/');
    } else if (splitPath[0] === 'tmp') {
      splitPath[0] = RNFSManager.NSTmpDirectoryPath;
      return splitPath.join('/');
    }
    //treat it as relative to Documents directory
    return RNFSManager.NSDocumentDirectoryPath + '/' + filepath;
  }
  //absolute path
  return filepath;
}

var RNFS = {

  writeFile(filepath, contents) {
      var processedFilePath = processFilePath(filepath);
      if (arguments.length === 4) {
        RNFSManager.writeFile(processedFilePath, contents, arguments[2], arguments[3]);
      } else {
        RNFSManager.writeFile(processedFilePath, contents, {}, arguments[2]);
      }
    },

    readFile(filepath) {
      var processedFilePath = processFilePath(filepath);
      if (arguments.length === 3) {
        var options = arguments[1];
        var callback = arguments[2];
        RNFSManager.readFile(processedFilePath, options, callback);
      } else {
        var callback = arguments[1];
        RNFSManager.readFile(processedFilePath, {
          encoding: 'null',
          flag: 'r'
        }, callback);
      }
    },

    readdir(filepath, callback) {
      var processedFilePath = processFilePath(filepath);
      RNFSManager.readdir(processedFilePath, callback);
    },

    mkdir(filepath) {
      var processedFilePath = processFilePath(filepath);
      if (arguments.length === 3) {
        var mode = arguments[1];
        var callback = arguments[2];
        RNFSManager.mkdir(processedFilePath, mode, callback);
      } else {
        var callback = arguments[1];
        RNFSManager.mkdir(processedFilePath, '0777', callback);
      }
    },

    stat(filepath, callback) {
      var processedFilePath = processFilePath(filepath);
      RNFSManager.stat(processedFilePath, function(err, result) {
        if (!err) {
          var stats = {
            ctime: new Date(result.ctime * 1000),
            mtime: new Date(result.mtime * 1000),
            size: result.size,
            mode: result.mode,
            isFile: () => result.type === NSFileTypeRegular,
            isDirectory: () => result.type === NSFileTypeDirectory,
          };
        }
        callback(err, stats);
      });
    },

    unlink(filepath, callback) {
      var processedFilePath = processFilePath(filepath);
      RNFSManager.unlink(processedFilePath, callback);
    },

    MainBundle: RNFSManager.MainBundleDirectory,
    CachesDirectory: RNFSManager.NSCachesDirectory,
    DocumentDirectory: RNFSManager.NSDocumentDirectory,
    CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
    DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath
};

var Promisify = {

  readdir(path) {
      var p = Promise.promisify(RNFS.readdir);
      return p.apply(null, arguments)
    },

    stat(filepath) {
      var p = Promise.promisify(RNFS.stat);
      return p.apply(null, arguments);
    },

    readFile(filepath) {
      var p = Promise.promisify(RNFS.readFile);
      return p.apply(null, arguments)
    },

    writeFile(filepath, contents) {
      var p = Promise.promisify(RNFS.writeFile);
      return p.apply(null, arguments)
    },

    unlink(filepath) {
      var p = Promise.promisify(RNFS.unlink);
      return p.apply(null, arguments)
    },

    mkdir(filepath) {
      var p = Promise.promisify(RNFS.mkdir);
      return p.apply(null, arguments)
    },

    MainBundle: RNFSManager.MainBundleDirectory,
    CachesDirectory: RNFSManager.NSCachesDirectory,
    DocumentDirectory: RNFSManager.NSDocumentDirectory,
    CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
    DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath
};

RNFS.Promisify = Promisify;
module.exports = RNFS;
