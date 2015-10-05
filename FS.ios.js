'use strict';

var RNFSManager = require('react-native').NativeModules.RNFSManager;
var Promise = require('bluebird');
var base64 = require('base-64');

var _readDir = Promise.promisify(RNFSManager.readDir);
var _stat = Promise.promisify(RNFSManager.stat);
var _readFile = Promise.promisify(RNFSManager.readFile);
var _writeFile = Promise.promisify(RNFSManager.writeFile);
var _unlink = Promise.promisify(RNFSManager.unlink);
var _pathForBundle = Promise.promisify(RNFSManager.pathForBundle);

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

var RNFS = {

  readDir(path, rootDir) {
    return _readDir(path, rootDir)
      .catch(convertError);
  },

  stat(filepath) {
    return _stat(filepath)
      .then((result) => {
        return {
          'ctime': new Date(result.ctime*1000),
          'mtime': new Date(result.mtime*1000),
          'size': result.size,
          'mode': result.mode,
          isFile: () => result.type === NSFileTypeRegular,
          isDirectory: () => result.type === NSFileTypeDirectory,
        };
      })
      .catch(convertError);
  },

  readFile(filepath, shouldDecode) {
    var p = _readFile(filepath);

    if (shouldDecode !== false) {
      p = p.then((data) => {
        return base64.decode(data);
      });
    }

    return p.catch(convertError);
  },

  writeFile(filepath, contents, shouldEncode, options) {
    var contents;
    if (shouldEncode !== false) {
      contents = base64.encode(contents);
    }
    return _writeFile(filepath, contents, options)
      .catch(convertError);
  },

  pathForBundle(bundleName) {
    return _pathForBundle(bundleName);
  },

  unlink(filepath) {
    return _unlink(filepath)
      .catch(convertError);
  },

  MainBundle: RNFSManager.MainBundleDirectory,
  CachesDirectory: RNFSManager.NSCachesDirectory,
  DocumentDirectory: RNFSManager.NSDocumentDirectory,
  CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath
};

module.exports = RNFS;
