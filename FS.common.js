'use strict';

// This file supports both iOS and Android

// Stop bluebird going nuts because it can't find "self"
if (typeof self === 'undefined') {
  global.self = global;
}

var RNFSManager = require('react-native').NativeModules.RNFSManager;
var NativeAppEventEmitter = require('react-native').NativeAppEventEmitter;  // iOS
var DeviceEventEmitter = require('react-native').DeviceEventEmitter;        // Android
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
var _uploadFiles = Promise.promisify(RNFSManager.uploadFiles);
var _pathForBundle = Promise.promisify(RNFSManager.pathForBundle);

var NATIVE_ADD_LISTENER_AVAILABLE = !!NativeAppEventEmitter.addListener;
var DEVICE_EVENT_EMITTER_LISTENER_AVAILABLE = !!DeviceEventEmitter.addListener;

var addListener;

if (NATIVE_ADD_LISTENER_AVAILABLE) {
  addListener = (event, cb) => {
    return NativeAppEventEmitter.addListener(event, cb);
  }
} else if (DEVICE_EVENT_EMITTER_LISTENER_AVAILABLE) {
  addListener = (event, cb) => {
    return DeviceEventEmitter.addListener(event, cb);
  }
}

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

  unlink(filepath) {
    return _unlink(filepath)
      .catch(convertError);
  },

  mkdir(filepath, excludeFromBackup) {
    excludeFromBackup = !!excludeFromBackup;
    return _mkdir(filepath, excludeFromBackup)
      .catch(convertError);
  },

  downloadFile(fromUrl, toFile, begin, progress) {
    var jobId = getJobId();
    var subscription;

    if (!begin) begin = (info) => {
      console.log('Download begun:', info);
    };

    if (begin) {
      subscription = addListener('DownloadBegin-' + jobId, begin);
    }

    if (progress) {
      subscription = addListener('DownloadProgress-' + jobId, progress);
    }

    return _downloadFile(fromUrl, toFile, jobId)
      .then(res => {
        if (subscription) {
          subscription.remove();
        }
        return res;
      })
      .catch(convertError);
  },

  stopDownload(jobId) {
    RNFSManager.stopDownload(jobId);
  },

  uploadFiles(toUrl, files, options, begin, progress) {
    var jobId = getJobId();
    var subscription;

    if (!begin) begin = (info) => {
      console.log('Upload begun:', info);
    };

    if (begin) {
      subscription = addListener('UploadBegin-' + jobId, begin)
    }

    if (progress) {
      subscription = addListener('UploadProgress-' + jobId, progress);
    }

    if(typeof options !== "object") {
      options = {};
    }

    return _uploadFiles(toUrl, files, options, jobId)
      .then(res => {
        if (subscription) {
          subscription.remove();
        }
        return res;
      });
  },

  stopUpload(jobId) {
    RNFSManager.stopUpload(jobId);
  },

  MainBundlePath: RNFSManager.MainBundlePath,
  CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath,
  LibraryDirectoryPath: RNFSManager.NSLibraryDirectoryPath,
  PicturesDirectoryPath: RNFSManager.NSPicturesDirectoryPath
};

module.exports = RNFS;
