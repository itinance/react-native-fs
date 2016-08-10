/**
 * React Native FS
 * @flow
 */

'use strict';

// This file supports both iOS and Android

var RNFSManager = require('react-native').NativeModules.RNFSManager;

var NativeAppEventEmitter = require('react-native').NativeAppEventEmitter;  // iOS
var DeviceEventEmitter = require('react-native').DeviceEventEmitter;        // Android
var base64 = require('base-64');
var utf8 = require('utf8');

var RNFSFileTypeRegular = RNFSManager.RNFSFileTypeRegular;
var RNFSFileTypeDirectory = RNFSManager.RNFSFileTypeDirectory;

var jobId = 0;

var getJobId = () => {
  jobId += 1;
  return jobId;
};

type MkdirOptions = {
  RNFSURLIsExcludedFromBackupKey?: boolean;
};

type ReadDirItem = {
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};

type StatResult = {
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes
  mode: number;     // UNIX file mode
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};

type Headers = { [name: string]: string };
type Fields = { [name: string]: string };

type DownloadFileOptions = {
  fromUrl: string;          // URL to download file from
  toFile: string;           // Local filesystem path to save the file to
  headers?: Headers;        // An object of headers to be passed to the server
  background?: boolean;
  progressDivider?: number;
  begin?: (res: DownloadBeginCallbackResult) => void;
  progress?: (res: DownloadProgressCallbackResult) => void;
};

type DownloadBeginCallbackResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number;     // The HTTP status code
  contentLength: number;  // The total size in bytes of the download resource
  headers: Headers;       // The HTTP response headers from the server
};

type DownloadProgressCallbackResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  contentLength: number;  // The total size in bytes of the download resource
  bytesWritten: number;   // The number of bytes written to the file so far
};

type DownloadResult = {
  jobId: number;          // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number;     // The HTTP status code
  bytesWritten: number;   // The number of bytes written to the file
};

type UploadFileOptions = {
  toUrl: string;            // URL to upload file to
  files: UploadFileItem[];  // An array of objects with the file information to be uploaded.
  headers?: Headers;        // An object of headers to be passed to the server
  fields?: Fields;          // An object of fields to be passed to the server
  method?: string;          // Default is 'POST', supports 'POST' and 'PUT'
  begin?: (res: UploadBeginCallbackResult) => void;
  progress?: (res: UploadProgressCallbackResult) => void;
};

type UploadFileItem = {
  name: string;       // Name of the file, if not defined then filename is used
  filename: string;   // Name of file
  filepath: string;   // Path to file
  filetype: string;   // The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
};

type UploadBeginCallbackResult = {
  jobId: number;        // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
};

type UploadProgressCallbackResult = {
  jobId: number;                      // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  totalBytesExpectedToSend: number;   // The total number of bytes that will be sent to the server
  totalBytesSent: number;             // The number of bytes sent to the server
};

type UploadResult = {
  jobId: number;        // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  statusCode: number;   // The HTTP status code
  headers: Headers;     // The HTTP response headers from the server
  body: string;         // The HTTP response body
};

type FSInfoResult = {
  totalSpace: number;   // The total amount of storage space on the device (in bytes).
  freeSpace: number;    // The amount of available storage space on the device (in bytes).
};

var RNFS = {

  mkdir(filepath: string, options: MkdirOptions = {}): Promise<void> {
    return RNFSManager.mkdir(filepath, options).then(() => void 0);
  },

  moveFile(filepath: string, destPath: string): Promise<void> {
    return RNFSManager.moveFile(filepath, destPath).then(() => void 0);
  },

  copyFile(filepath: string, destPath: string): Promise<void> {
    return RNFSManager.copyFile(filepath, destPath).then(() => void 0);
  },

  pathForBundle(bundleNamed: string): Promise<string> {
    return RNFSManager.pathForBundle(bundleNamed);
  },

  getFSInfo(): Promise<FSInfoResult> {
    return RNFSManager.getFSInfo();
  },

  unlink(filepath: string): Promise<void> {
    return RNFSManager.unlink(filepath).then(() => void 0);
  },

  exists(filepath: string): Promise<boolean> {
    return RNFSManager.exists(filepath);
  },

  stopDownload(jobId: number): void {
    RNFSManager.stopDownload(jobId);
  },

  stopUpload(jobId: number): void {
    RNFSManager.stopUpload(jobId);
  },

  readDir(dirpath: string): Promise<ReadDirItem[]> {
    return RNFSManager.readDir(dirpath).then(files => {
      return files.map(file => ({
        name: file.name,
        path: file.path,
        size: file.size,
        isFile: () => file.type === RNFSFileTypeRegular,
        isDirectory: () => file.type === RNFSFileTypeDirectory,
      }));
    });
  },

  // Node style version (lowercase d). Returns just the names
  readdir(dirpath: string): Promise<string[]> {
    return RNFS.readDir(dirpath).then(files => {
      return files.map(file => file.name);
    });
  },

  stat(filepath: string): Promise<StatResult> {
    return RNFSManager.stat(filepath).then((result) => {
      return {
        'ctime': new Date(result.ctime * 1000),
        'mtime': new Date(result.mtime * 1000),
        'size': result.size,
        'mode': result.mode,
        isFile: () => result.type === RNFSFileTypeRegular,
        isDirectory: () => result.type === RNFSFileTypeDirectory,
      };
    });
  },

  readFile(filepath: string, encodingOrOptions?: any): Promise<string> {
    var options = {
      encoding: 'utf8'
    };

    if (encodingOrOptions) {
      if (typeof encodingOrOptions === 'string') {
        options.encoding = encodingOrOptions;
      } else if (typeof encodingOrOptions === 'object') {
        options = encodingOrOptions;
      }
    }

    return RNFSManager.readFile(filepath).then((b64) => {
      var contents;

      if (options.encoding === 'utf8') {
        contents = utf8.decode(base64.decode(b64));
      } else if (options.encoding === 'ascii') {
        contents = base64.decode(b64);
      } else if (options.encoding === 'base64') {
        contents = b64;
      } else {
        throw new Error('Invalid encoding type "' + String(options.encoding) + '"');
      }

      return contents;
    });
  },

  writeFile(filepath: string, contents: string, encodingOrOptions?: any): Promise<void> {
    var b64;

    var options = {
      encoding: 'utf8'
    };

    if (encodingOrOptions) {
      if (typeof encodingOrOptions === 'string') {
        options.encoding = encodingOrOptions;
      } else if (typeof encodingOrOptions === 'object') {
        options = encodingOrOptions;
      }
    }

    if (options.encoding === 'utf8') {
      b64 = base64.encode(utf8.encode(contents));
    } else if (options.encoding === 'ascii') {
      b64 = base64.encode(contents);
    } else if (options.encoding === 'base64') {
      b64 = contents;
    } else {
      throw new Error('Invalid encoding type "' + options.encoding + '"');
    }

    return RNFSManager.writeFile(filepath, b64).then(() => void 0);
  },

  appendFile(filepath: string, contents: string, encodingOrOptions?: any): Promise<void> {
    var b64;

    var options = {
      encoding: 'utf8'
    };

    if (encodingOrOptions) {
      if (typeof encodingOrOptions === 'string') {
        options.encoding = encodingOrOptions;
      } else if (typeof encodingOrOptions === 'object') {
        options = encodingOrOptions;
      }
    }

    if (options.encoding === 'utf8') {
      b64 = base64.encode(utf8.encode(contents));
    } else if (options.encoding === 'ascii') {
      b64 = base64.encode(contents);
    } else if (options.encoding === 'base64') {
      b64 = contents;
    } else {
      throw new Error('Invalid encoding type "' + options.encoding + '"');
    }

    return RNFSManager.appendFile(filepath, b64);
  },

  downloadFile(options: DownloadFileOptions): { jobId: number, promise: Promise<DownloadResult> } {
    if (typeof options !== 'object') throw new Error('downloadFile: Invalid value for argument `options`');
    if (typeof options.fromUrl !== 'string') throw new Error('downloadFile: Invalid value for property `fromUrl`');
    if (typeof options.toFile !== 'string') throw new Error('downloadFile: Invalid value for property `toFile`');
    if (options.headers && typeof options.headers !== 'object') throw new Error('downloadFile: Invalid value for property `headers`');
    if (options.background && typeof options.background !== 'boolean') throw new Error('downloadFile: Invalid value for property `background`');
    if (options.progressDivider && typeof options.progressDivider !== 'number') throw new Error('downloadFile: Invalid value for property `progressDivider`');

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
      progressDivider: options.progressDivider || 0
    };

    return {
      jobId,
      promise: RNFSManager.downloadFile(bridgeOptions).then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      })
    };
  },

  uploadFiles(options: UploadFileOptions): { jobId: number, promise: Promise<UploadResult> } {
    if (!RNFSManager.uploadFiles) {
      return {
        jobId: -1,
        promise: Promise.reject(new Error('`uploadFiles` is unsupported on this platform'))
      };
    }

    var jobId = getJobId();
    var subscriptions = [];

    if (typeof options !== 'object') throw new Error('uploadFiles: Invalid value for argument `options`');
    if (typeof options.toUrl !== 'string') throw new Error('uploadFiles: Invalid value for property `toUrl`');
    if (!Array.isArray(options.files)) throw new Error('uploadFiles: Invalid value for property `files`');
    if (options.headers && typeof options.headers !== 'object') throw new Error('uploadFiles: Invalid value for property `headers`');
    if (options.fields && typeof options.fields !== 'object') throw new Error('uploadFiles: Invalid value for property `fields`');
    if (options.method && typeof options.method !== 'string') throw new Error('uploadFiles: Invalid value for property `method`');

    if (options.begin) {
      subscriptions.push(NativeAppEventEmitter.addListener('UploadBegin-' + jobId, options.begin));
    }
    if (options.beginCallback) {
      // Deprecated
      subscriptions.push(NativeAppEventEmitter.addListener('UploadBegin-' + jobId, options.beginCallback));
    }

    if (options.progress) {
      subscriptions.push(NativeAppEventEmitter.addListener('UploadProgress-' + jobId, options.progress));
    }
    if (options.progressCallback) {
      // Deprecated
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

    return {
      jobId,
      promise: RNFSManager.uploadFiles(bridgeOptions).then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      })
    };
  },

  MainBundlePath: RNFSManager.RNFSMainBundlePath,
  CachesDirectoryPath: RNFSManager.RNFSCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.RNFSDocumentDirectoryPath,
  ExternalDirectoryPath: RNFSManager.RNFSExternalDirectoryPath,
  ExternalStorageDirectoryPath: RNFSManager.RNFSExternalStorageDirectoryPath,
  TemporaryDirectoryPath: RNFSManager.RNFSTemporaryDirectoryPath,
  LibraryDirectoryPath: RNFSManager.RNFSLibraryDirectoryPath,
  PicturesDirectoryPath: RNFSManager.RNFSPicturesDirectoryPath

};

module.exports = RNFS;
