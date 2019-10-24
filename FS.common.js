/**
 * React Native FS
 * @flow
 */

'use strict';

// This file supports both iOS and Android

var RNFSManager = require('react-native').NativeModules.RNFSManager;

var NativeEventEmitter = require('react-native').NativeEventEmitter;

var RNFS_NativeEventEmitter = new NativeEventEmitter(RNFSManager);

var base64 = require('base-64');
var utf8 = require('utf8');
var isIOS = require('react-native').Platform.OS === 'ios';

var RNFSFileTypeRegular = RNFSManager.RNFSFileTypeRegular;
var RNFSFileTypeDirectory = RNFSManager.RNFSFileTypeDirectory;

var jobId = 0;

var getJobId = () => {
  jobId += 1;
  return jobId;
};

var normalizeFilePath = (path: string) => (path.startsWith('file://') ? path.slice(7) : path);

type MkdirOptions = {
  NSURLIsExcludedFromBackupKey?: boolean; // iOS only
  NSFileProtectionKey?: string; // IOS only
};

type FileOptions = {
  NSFileProtectionKey?: string; // IOS only
};

type ReadDirItem = {
  ctime: ?Date;    // The creation date of the file (iOS only)
  mtime: ?Date;    // The last modified date of the file
  name: string;     // The name of the item
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};

type StatResult = {
  name: ?string;     // The name of the item TODO: why is this not documented?
  path: string;     // The absolute path to the item
  size: string;     // Size in bytes
  mode: number;     // UNIX file mode
  ctime: number;    // Created date
  mtime: number;    // Last modified date
  originalFilepath: string;    // In case of content uri this is the pointed file path, otherwise is the same as path
  isFile: () => boolean;        // Is the file just a file?
  isDirectory: () => boolean;   // Is the file a directory?
};

type Headers = { [name: string]: string };
type Fields = { [name: string]: string };

type DownloadFileOptions = {
  fromUrl: string;          // URL to download file from
  toFile: string;           // Local filesystem path to save the file to
  headers?: Headers;        // An object of headers to be passed to the server
  background?: boolean;     // Continue the download in the background after the app terminates (iOS only)
  discretionary?: boolean;  // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
  cacheable?: boolean;      // Whether the download can be stored in the shared NSURLCache (iOS only)
  progressInterval?: number;
  progressDivider?: number;
  begin?: (res: DownloadBeginCallbackResult) => void;
  progress?: (res: DownloadProgressCallbackResult) => void;
  resumable?: () => void;    // only supported on iOS yet
  connectionTimeout?: number; // only supported on Android yet
  readTimeout?: number;       // supported on Android and iOS
  backgroundTimeout?: number; // Maximum time (in milliseconds) to download an entire resource (iOS only, useful for timing out background downloads)
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
  binaryStreamOnly?: boolean; // Allow for binary data stream for file to be uploaded without extra headers, Default is 'false'
  files: UploadFileItem[];  // An array of objects with the file information to be uploaded.
  headers?: Headers;        // An object of headers to be passed to the server
  fields?: Fields;          // An object of fields to be passed to the server
  method?: string;          // Default is 'POST', supports 'POST' and 'PUT'
  beginCallback?: (res: UploadBeginCallbackResult) => void; // deprecated
  progressCallback?: (res: UploadProgressCallbackResult) => void; // deprecated
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

/**
 * Generic function used by readFile and readFileAssets
 */
function readFileGeneric(filepath: string, encodingOrOptions: ?string, command: Function) {
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

  return command(normalizeFilePath(filepath)).then((b64) => {
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
}

/**
 * Generic function used by readDir and readDirAssets
 */
function readDirGeneric(dirpath: string, command: Function) {
  return command(normalizeFilePath(dirpath)).then(files => {
    return files.map(file => ({
      ctime: file.ctime && new Date(file.ctime * 1000) || null,
      mtime: file.mtime && new Date(file.mtime * 1000) || null,
      name: file.name,
      path: file.path,
      size: file.size,
      isFile: () => file.type === RNFSFileTypeRegular,
      isDirectory: () => file.type === RNFSFileTypeDirectory,
    }));
  });
}

var RNFS = {

  mkdir(filepath: string, options: MkdirOptions = {}): Promise<void> {
    return RNFSManager.mkdir(normalizeFilePath(filepath), options).then(() => void 0);
  },

  moveFile(filepath: string, destPath: string, options: FileOptions = {}): Promise<void> {
    return RNFSManager.moveFile(normalizeFilePath(filepath), normalizeFilePath(destPath), options).then(() => void 0);
  },

  copyFile(filepath: string, destPath: string, options: FileOptions = {}): Promise<void> {
    return RNFSManager.copyFile(normalizeFilePath(filepath), normalizeFilePath(destPath), options).then(() => void 0);
  },

  pathForBundle(bundleNamed: string): Promise<string> {
    return RNFSManager.pathForBundle(bundleNamed);
  },

  pathForGroup(groupName: string): Promise<string> {
    return RNFSManager.pathForGroup(groupName);
  },

  getFSInfo(): Promise<FSInfoResult> {
    return RNFSManager.getFSInfo();
  },

  getAllExternalFilesDirs(): Promise<string> {
    return RNFSManager.getAllExternalFilesDirs();
  },

  unlink(filepath: string): Promise<void> {
    return RNFSManager.unlink(normalizeFilePath(filepath)).then(() => void 0);
  },

  exists(filepath: string): Promise<boolean> {
    return RNFSManager.exists(normalizeFilePath(filepath));
  },

  stopDownload(jobId: number): void {
    RNFSManager.stopDownload(jobId);
  },

  resumeDownload(jobId: number): void {
    RNFSManager.resumeDownload(jobId);
  },

  isResumable(jobId: number): Promise<bool> {
    return RNFSManager.isResumable(jobId);
  },

  stopUpload(jobId: number): void {
    RNFSManager.stopUpload(jobId);
  },

  completeHandlerIOS(jobId: number): void {
    return RNFSManager.completeHandlerIOS(jobId);
  },

  readDir(dirpath: string): Promise<ReadDirItem[]> {
    return readDirGeneric(dirpath, RNFSManager.readDir);
  },

  // Android-only
  readDirAssets(dirpath: string): Promise<ReadDirItem[]> {
    if (!RNFSManager.readDirAssets) {
      throw new Error('readDirAssets is not available on this platform');
    }
    return readDirGeneric(dirpath, RNFSManager.readDirAssets);
  },

  // Android-only
  existsAssets(filepath: string) {
    if (!RNFSManager.existsAssets) {
      throw new Error('existsAssets is not available on this platform');
    }
    return RNFSManager.existsAssets(filepath);
  },

  // Android-only
  existsRes(filename: string) {
    if (!RNFSManager.existsRes) {
      throw new Error('existsRes is not available on this platform');
    }
    return RNFSManager.existsRes(filename);
  },

  // Node style version (lowercase d). Returns just the names
  readdir(dirpath: string): Promise<string[]> {
    return RNFS.readDir(normalizeFilePath(dirpath)).then(files => {
      return files.map(file => file.name);
    });
  },

  // setReadable for Android
  setReadable(filepath: string, readable: boolean, ownerOnly: boolean): Promise<boolean> {
    return RNFSManager.setReadable(filepath, readable, ownerOnly).then((result) => {
      return result;
    })
  },

  stat(filepath: string): Promise<StatResult> {
    return RNFSManager.stat(normalizeFilePath(filepath)).then((result) => {
      return {
        'path': filepath,
        'ctime': new Date(result.ctime * 1000),
        'mtime': new Date(result.mtime * 1000),
        'size': result.size,
        'mode': result.mode,
        'originalFilepath': result.originalFilepath,
        isFile: () => result.type === RNFSFileTypeRegular,
        isDirectory: () => result.type === RNFSFileTypeDirectory,
      };
    });
  },

  readFile(filepath: string, encodingOrOptions?: any): Promise<string> {
    return readFileGeneric(filepath, encodingOrOptions, RNFSManager.readFile);
  },

  read(filepath: string, length: number = 0, position: number = 0, encodingOrOptions?: any): Promise<string> {
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

    return RNFSManager.read(normalizeFilePath(filepath), length, position).then((b64) => {
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

  // Android only
  readFileAssets(filepath: string, encodingOrOptions?: any): Promise<string> {
    if (!RNFSManager.readFileAssets) {
      throw new Error('readFileAssets is not available on this platform');
    }
    return readFileGeneric(filepath, encodingOrOptions, RNFSManager.readFileAssets);
  },

  // Android only
  readFileRes(filename: string, encodingOrOptions?: any): Promise<string> {
    if (!RNFSManager.readFileRes) {
      throw new Error('readFileRes is not available on this platform');
    }
    return readFileGeneric(filename, encodingOrOptions, RNFSManager.readFileRes);
  },

  hash(filepath: string, algorithm: string): Promise<string> {
    return RNFSManager.hash(normalizeFilePath(filepath), algorithm);
  },

  // Android only
  copyFileAssets(filepath: string, destPath: string) {
    if (!RNFSManager.copyFileAssets) {
      throw new Error('copyFileAssets is not available on this platform');
    }
    return RNFSManager.copyFileAssets(normalizeFilePath(filepath), normalizeFilePath(destPath)).then(() => void 0);
  },

  // Android only
  copyFileRes(filename: string, destPath: string) {
    if (!RNFSManager.copyFileRes) {
      throw new Error('copyFileRes is not available on this platform');
    }
    return RNFSManager.copyFileRes(filename, normalizeFilePath(destPath)).then(() => void 0);
  },

  // iOS only
  // Copies fotos from asset-library (camera-roll) to a specific location
  // with a given width or height
  // @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
  copyAssetsFileIOS(imageUri: string, destPath: string, width: number, height: number,
    scale: number = 1.0, compression: number = 1.0, resizeMode: string = 'contain'): Promise<string> {
    return RNFSManager.copyAssetsFileIOS(imageUri, destPath, width, height, scale, compression, resizeMode);
  },

  // iOS only
  // Copies fotos from asset-library (camera-roll) to a specific location
  // with a given width or height
  // @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
  copyAssetsVideoIOS(imageUri: string, destPath: string): Promise<string> {
    return RNFSManager.copyAssetsVideoIOS(imageUri, destPath);
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
        options = {
          ...options,
          ...encodingOrOptions
        };
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

    return RNFSManager.writeFile(normalizeFilePath(filepath), b64, options).then(() => void 0);
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

    return RNFSManager.appendFile(normalizeFilePath(filepath), b64);
  },

  write(filepath: string, contents: string, position?: number, encodingOrOptions?: any): Promise<void> {
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

    if (position === undefined) {
      position = -1;
    }

    return RNFSManager.write(normalizeFilePath(filepath), b64, position).then(() => void 0);
  },

  downloadFile(options: DownloadFileOptions): { jobId: number, promise: Promise<DownloadResult> } {
    if (typeof options !== 'object') throw new Error('downloadFile: Invalid value for argument `options`');
    if (typeof options.fromUrl !== 'string') throw new Error('downloadFile: Invalid value for property `fromUrl`');
    if (typeof options.toFile !== 'string') throw new Error('downloadFile: Invalid value for property `toFile`');
    if (options.headers && typeof options.headers !== 'object') throw new Error('downloadFile: Invalid value for property `headers`');
    if (options.background && typeof options.background !== 'boolean') throw new Error('downloadFile: Invalid value for property `background`');
    if (options.progressDivider && typeof options.progressDivider !== 'number') throw new Error('downloadFile: Invalid value for property `progressDivider`');
    if (options.progressInterval && typeof options.progressInterval !== 'number') throw new Error('downloadFile: Invalid value for property `progressInterval`');
    if (options.readTimeout && typeof options.readTimeout !== 'number') throw new Error('downloadFile: Invalid value for property `readTimeout`');
    if (options.connectionTimeout && typeof options.connectionTimeout !== 'number') throw new Error('downloadFile: Invalid value for property `connectionTimeout`');
    if (options.backgroundTimeout && typeof options.backgroundTimeout !== 'number') throw new Error('downloadFile: Invalid value for property `backgroundTimeout`');

    var jobId = getJobId();
    var subscriptions = [];

    if (options.begin) {
      subscriptions.push(RNFS_NativeEventEmitter.addListener('DownloadBegin', options.begin));
    }

    if (options.progress) {
      subscriptions.push(RNFS_NativeEventEmitter.addListener('DownloadProgress', options.progress));
    }

    if (options.resumable) {
      subscriptions.push(RNFS_NativeEventEmitter.addListener('DownloadResumable', options.resumable));
    }

    var bridgeOptions = {
      jobId: jobId,
      fromUrl: options.fromUrl,
      toFile: normalizeFilePath(options.toFile),
      headers: options.headers || {},
      background: !!options.background,
      progressDivider: options.progressDivider || 0,
      progressInterval: options.progressInterval || 0,
      readTimeout: options.readTimeout || 15000,
      connectionTimeout: options.connectionTimeout || 5000,
      backgroundTimeout: options.backgroundTimeout || 3600000, // 1 hour
      hasBeginCallback: options.begin instanceof Function,
      hasProgressCallback: options.progress instanceof Function,
      hasResumableCallback: options.resumable instanceof Function,
    };

    return {
      jobId,
      promise: RNFSManager.downloadFile(bridgeOptions).then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      })
        .catch(e => {
          return Promise.reject(e);
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
      subscriptions.push(RNFS_NativeEventEmitter.addListener('UploadBegin', options.begin));
    } else if (options.beginCallback) {
      // Deprecated
      subscriptions.push(RNFS_NativeEventEmitter.addListener('UploadBegin', options.beginCallback));
    }

    if (options.progress) {
      subscriptions.push(RNFS_NativeEventEmitter.addListener('UploadProgress', options.progress));
    } else if (options.progressCallback) {
      // Deprecated
      subscriptions.push(RNFS_NativeEventEmitter.addListener('UploadProgress', options.progressCallback));
    }

    var bridgeOptions = {
      jobId: jobId,
      toUrl: options.toUrl,
      files: options.files,
      binaryStreamOnly: options.binaryStreamOnly || false,
      headers: options.headers || {},
      fields: options.fields || {},
      method: options.method || 'POST',
      hasBeginCallback: options.begin instanceof Function || options.beginCallback instanceof Function,
      hasProgressCallback: options.progress instanceof Function || options.progressCallback instanceof Function,
    };

    return {
      jobId,
      promise: RNFSManager.uploadFiles(bridgeOptions).then(res => {
        subscriptions.forEach(sub => sub.remove());
        return res;
      })
    };
  },

  touch(filepath: string, mtime?: Date, ctime?: Date): Promise<void> {
    if (ctime && !(ctime instanceof Date)) throw new Error('touch: Invalid value for argument `ctime`');
    if (mtime && !(mtime instanceof Date)) throw new Error('touch: Invalid value for argument `mtime`');
    var ctimeTime = 0;
    if (isIOS) {
      ctimeTime = ctime && ctime.getTime();
    }
    return RNFSManager.touch(
      normalizeFilePath(filepath),
      mtime && mtime.getTime(),
      ctimeTime
    );
  },

  scanFile(path: string): Promise<ReadDirItem[]> {
    return RNFSManager.scanFile(path);
  },

  MainBundlePath: RNFSManager.RNFSMainBundlePath,
  CachesDirectoryPath: RNFSManager.RNFSCachesDirectoryPath,
  ExternalCachesDirectoryPath: RNFSManager.RNFSExternalCachesDirectoryPath,
  DocumentDirectoryPath: RNFSManager.RNFSDocumentDirectoryPath,
  ExternalDirectoryPath: RNFSManager.RNFSExternalDirectoryPath,
  ExternalStorageDirectoryPath: RNFSManager.RNFSExternalStorageDirectoryPath,
  TemporaryDirectoryPath: RNFSManager.RNFSTemporaryDirectoryPath,
  LibraryDirectoryPath: RNFSManager.RNFSLibraryDirectoryPath,
  PicturesDirectoryPath: RNFSManager.RNFSPicturesDirectoryPath,
  FileProtectionKeys: RNFSManager.RNFSFileProtectionKeys
};

module.exports = RNFS;
