'use strict';

// This file supports both iOS and Android

// Stop bluebird going nuts because it can't find "self"
if (typeof self === 'undefined') {
  global.self = global;
}

var RNFSManager = require('react-native').NativeModules.RNFSManager;
var NativeAppEventEmitter = require('react-native').NativeAppEventEmitter;
var Promise = require('bluebird');
var base64 = require('base-64');
var utf8 = require('utf8');

var _readDir = Promise.promisify(RNFSManager.readDir);
var _exists = Promise.promisify(RNFSManager.exists);
var _stat = Promise.promisify(RNFSManager.stat);
var _readFile = Promise.promisify(RNFSManager.readFile);
var _writeFile = Promise.promisify(RNFSManager.writeFile);
var _moveFile = Promise.promisify(RNFSManager.moveFile);
var _unlink = Promise.promisify(RNFSManager.unlink);
var _mkdir = Promise.promisify(RNFSManager.mkdir);
var _downloadFile = Promise.promisify(RNFSManager.downloadFile);
var _uploadFiles = RNFSManager.uploadFiles ? Promise.promisify(RNFSManager.uploadFiles) : function () { return Promise.reject('Not implemented on Android') };
var _pathForBundle = Promise.promisify(RNFSManager.pathForBundle);
var _getFSInfo = Promise.promisify(RNFSManager.getFSInfo);

var convertError = (err) => {
  if (err.isOperational && err.cause) {
    err = err.cause;
  }

  var error = new Error(err.description || err.message);
  error.code = err.code;
  throw error;
};

var NSFileTypeRegular = RNFSManager.NSFileTypeRegular;
var NSFileTypeDirectory = RNFSManager.NSFileTypeDirectory;

var jobId = 0;

var getJobId = () => {
  jobId += 1;
  return jobId;
};

var RNFS = {

  readDir(dirpath) {
    return _readDir(dirpath)
      .then(files => {
        return files.map(file => ({
          name: file.name,
          path: file.path,
          size: file.size,
          isFile: () => file.type === NSFileTypeRegular,
          isDirectory: () => file.type === NSFileTypeDirectory,
        }));
      })
      .catch(convertError);
  },

  // Node style version (lowercase d). Returns just the names
  readdir(dirpath) {
    return RNFS.readDir(dirpath)
      .then(files => {
        return files.map(file => file.name);
      });
  },

  stat(filepath) {
    return _stat(filepath)
      .then((result) => {
        return {
          'ctime': new Date(result.ctime * 1000),
          'mtime': new Date(result.mtime * 1000),
          'size': result.size,
          'mode': result.mode,
          isFile: () => result.type === NSFileTypeRegular,
          isDirectory: () => result.type === NSFileTypeDirectory,
        };
      })
      .catch(convertError);
  },

  exists(filepath) {
    return _exists(filepath)
      .catch(convertError);
  },

  readFile(filepath, encoding) {
    if (!encoding) encoding = 'utf8';

    return _readFile(filepath)
      .then((b64) => {
        var contents;

        if (encoding === 'utf8') {
          contents = utf8.decode(base64.decode(b64));
        } else if (encoding === 'ascii') {
          contents = base64.decode(b64);
        } else if (encoding === 'base64') {
          contents = b64;
        } else {
          throw new Error('Invalid encoding type "' + encoding + '"');
        }

        return contents;
      })
      .catch(convertError);
  },

  writeFile(filepath, contents, encoding, options) {
    var b64;

    if (!encoding) encoding = 'utf8';

    if (encoding === 'utf8') {
      b64 = base64.encode(utf8.encode(contents));
    } else if (encoding === 'ascii') {
      b64 = base64.encode(contents);
    } else if (encoding === 'base64') {
      b64 = contents;
    } else {
      throw new Error('Invalid encoding type "' + encoding + '"');
    }

    return _writeFile(filepath, b64, options)
      .catch(convertError);
  },

  moveFile(filepath, destPath) {
    return _moveFile(filepath, destPath)
      .catch(convertError);
  },

  pathForBundle(bundleName) {
    return _pathForBundle(bundleName);
  },

  getFSInfo() {
    return _getFSInfo()
      .catch(convertError);
  },

  unlink(filepath) {
    return _unlink(filepath)
      .catch(convertError);
  },

  mkdir(filepath, excludeFromBackup) {
    excludeFromBackup = !!excludeFromBackup;
    return _mkdir(filepath, excludeFromBackup)
      .catch(convertError);
  },

  downloadFile(options) {
    if (arguments.length > 1) {
      console.warn('Deprecated: Please see updated docs for `downloadFile`');

      options = {
        fromUrl: arguments[0],
        toFile: arguments[1],
        begin: arguments[2],
        progress: arguments[3]
      };
    }

    if (typeof options !== 'object') throw new Error('downloadFile: Invalid value for argument `options`');
    if (typeof options.fromUrl !== 'string') throw new Error('downloadFile: Invalid value for property `fromUrl`');
    if (typeof options.toFile !== 'string') throw new Error('downloadFile: Invalid value for property `toFile`');
    if (options.headers && typeof options.headers !== 'object') throw new Error('downloadFile: Invalid value for property `headers`');
    if (options.background && typeof options.background !== 'boolean') throw new Error('downloadFile: Invalid value for property `background`');

    var jobId = getJobId();
    var subscriptions = [];

    if (options.begin) {
      subscriptions.push(NativeAppEventEmitter.addListener('DownloadBegin-' + jobId, options.begin));
    }

    if (options.progress) {
      subscriptions.push(NativeAppEventEmitter.addListener('DownloadProgress-' + jobId, options.progress));
    }

    var bridgeOptions = {
      jobId: jobId,
      fromUrl: options.fromUrl,
      toFile: options.toFile,
      headers: options.headers || {},
      background: !!options.background,
    };

    return _downloadFile(bridgeOptions)
      .then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      })
      .catch(convertError);
  },

  stopDownload(jobId) {
    RNFSManager.stopDownload(jobId);
  },

  uploadFiles(options) {
    var jobId = getJobId();
    var subscriptions = [];

    if (typeof options !== 'object') throw new Error('uploadFiles: Invalid value for argument `options`');
    if (typeof options.toUrl !== 'string') throw new Error('uploadFiles: Invalid value for property `toUrl`');
    if (!Array.isArray(options.files)) throw new Error('uploadFiles: Invalid value for property `files`');
    if (options.headers && typeof options.headers !== 'object') throw new Error('uploadFiles: Invalid value for property `headers`');
    if (options.fields && typeof options.fields !== 'object') throw new Error('uploadFiles: Invalid value for property `fields`');
    if (options.method && typeof options.method !== 'string') throw new Error('uploadFiles: Invalid value for property `method`');

    if (options.beginCallback) {
      subscriptions.push(NativeAppEventEmitter.addListener('UploadBegin-' + jobId, options.beginCallback));
    }

    if (options.progressCallback) {
      subscriptions.push(NativeAppEventEmitter.addListener('UploadProgress-' + jobId, options.progressCallback));
    }

    var bridgeOptions = {
      jobId: jobId,
      toUrl: options.toUrl,
      files: options.files,
      headers: options.headers || {},
      fields: options.fields || {},
      method: options.method || 'POST'
    };

    return _uploadFiles(bridgeOptions)
      .then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      });
  },

  stopUpload(jobId) {
    RNFSManager.stopUpload(jobId);
  },

  MainBundlePath: RNFSManager.MainBundlePath,
  CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath,
  ExternalDirectoryPath: RNFSManager.NSExternalDirectoryPath,
  TemporaryDirectoryPath: RNFSManager.NSTemporaryDirectoryPath,
  LibraryDirectoryPath: RNFSManager.NSLibraryDirectoryPath,
  PicturesDirectoryPath: RNFSManager.NSPicturesDirectoryPath
};

module.exports = RNFS;
