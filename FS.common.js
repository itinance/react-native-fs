'use strict';

// This file supports both iOS and Android

var RNFSManager = require('react-native').NativeModules.RNFSManager;
var NativeAppEventEmitter = require('react-native').NativeAppEventEmitter;  // iOS
var DeviceEventEmitter = require('react-native').DeviceEventEmitter;        // Android
var base64 = require('base-64');
var utf8 = require('utf8');

var NSFileTypeRegular = RNFSManager.NSFileTypeRegular;
var NSFileTypeDirectory = RNFSManager.NSFileTypeDirectory;

var jobId = 0;

var getJobId = () => {
  jobId += 1;
  return jobId;
};

var RNFS = {

  readDir(dirpath) {
    return RNFSManager.readDir(dirpath)
      .then(files => {
        return files.map(file => ({
          name: file.name,
          path: file.path,
          size: file.size,
          isFile: () => file.type === NSFileTypeRegular,
          isDirectory: () => file.type === NSFileTypeDirectory,
        }));
      });
  },

  // Node style version (lowercase d). Returns just the names
  readdir(dirpath) {
    return RNFSManager.readDir(dirpath)
      .then(files => files.map(file => file.name));
  },

  stat(filepath) {
    return RNFSManager.stat(filepath)
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

  readFile(filepath, encoding) {
    if (!encoding) encoding = 'utf8';

    return RNFSManager.readFile(filepath)
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
      });
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

    return RNFSManager.writeFile(filepath, b64, options);
  },

  mkdir(filepath, excludeFromBackup) {
    excludeFromBackup = !!excludeFromBackup;
    return RNFSManager.mkdir(filepath, excludeFromBackup);
  },

  downloadFile(fromUrl, toFile, begin, progress) {
    var jobId = getJobId();
    var subscriptionIos, subscriptionAndroid;

    if (!begin) begin = (info) => {
      console.log('Download begun:', info);
    };

    if (begin) {
      // Two different styles of subscribing to events for different platforms, hmmm....
      if (NativeAppEventEmitter.addListener)
        subscriptionIos = NativeAppEventEmitter.addListener('DownloadBegin-' + jobId, begin);
      if (DeviceEventEmitter.addListener)
        subscriptionAndroid = DeviceEventEmitter.addListener('DownloadBegin-' + jobId, begin);
    }

    if (progress) {
      if (NativeAppEventEmitter.addListener)
        subscriptionIos = NativeAppEventEmitter.addListener('DownloadProgress-' + jobId, progress);
      if (DeviceEventEmitter.addListener)
        subscriptionAndroid = DeviceEventEmitter.addListener('DownloadProgress-' + jobId, progress);
    }

    return RNFSManager.downloadFile(fromUrl, toFile, jobId)
      .then(res => {
        if (subscriptionIos) subscriptionIos.remove();
        if (subscriptionAndroid) subscriptionAndroid.remove();
        return res;
      });
  },

  stopDownload: RNFSManager.stopDownload,
  moveFile: RNFSManager.moveFile,
  pathForBundle: RNFSManager.pathForBundle,
  getFSInfo: RNFSManager.getFSInfo,
  unlink: RNFSManager.unlink,
  exists: RNFSManager.exists,

  MainBundlePath: RNFSManager.MainBundlePath,
  CachesDirectoryPath: RNFSManager.NSCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.NSDocumentDirectoryPath,
  ExternalDirectoryPath: RNFSManager.NSExternalDirectoryPath,
  LibraryDirectoryPath: RNFSManager.NSLibraryDirectoryPath,
  PicturesDirectoryPath: RNFSManager.NSPicturesDirectoryPath
};

module.exports = RNFS;
