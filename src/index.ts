import { type EmitterSubscription, NativeEventEmitter } from 'react-native';

import RNFS from './ReactNativeFs';

import {
  type DownloadFileOptions,
  type DownloadResult,
  type FSInfoResult,
  type FileOptions,
  type MkdirOptions,
  type NativeDownloadFileOptions,
  type NativeReadDirResItemT,
  type ReadDirItem,
  type StatResult,
  type UploadFileOptions,
  type UploadResult,
} from './NativeReactNativeFs';

import {
  type EncodingOptions,
  decode,
  encode,
  normalizeFilePath,
  readFileGeneric,
  toEncoding,
} from './utils';

const nativeEventEmitter = new NativeEventEmitter(RNFS);

let lastJobId = 0;

// Internal functions.

type ReadDirCommand = (path: string) => Promise<NativeReadDirResItemT[]>;

/**
 * Generic function used by readDir and readDirAssets.
 */
async function readDirGeneric(
  dirpath: string,
  command: ReadDirCommand,
): Promise<ReadDirItem[]> {
  const files = await command(normalizeFilePath(dirpath));

  const { FileTypeDirectory, FileTypeRegular } = RNFS.getConstants();

  return files.map((file) => ({
    ctime: (file.ctime && new Date((file.ctime as number) * 1000)) || null,
    mtime: (file.mtime && new Date((file.mtime as number) * 1000)) || null,
    name: file.name,
    path: file.path,
    size: file.size,
    isFile: () => file.type === FileTypeRegular,
    isDirectory: () => file.type === FileTypeDirectory,
  }));
}

// Common exports.

export function appendFile(
  filepath: string,
  contents: string,
  encodingOrOptions?: EncodingOptions,
): Promise<void> {
  const b64 = encode(contents, toEncoding(encodingOrOptions));
  return RNFS.appendFile(normalizeFilePath(filepath), b64);
}

export function copyFile(
  filepath: string,
  destPath: string,
  options: FileOptions = {},
): Promise<void> {
  return RNFS.copyFile(
    normalizeFilePath(filepath),
    normalizeFilePath(destPath),
    options,
  );
}

export function downloadFile(options: DownloadFileOptions): {
  jobId: number;
  promise: Promise<DownloadResult>;
} {
  if (typeof options !== 'object') {
    throw new Error('downloadFile: Invalid value for argument `options`');
  }
  if (typeof options.fromUrl !== 'string') {
    throw new Error('downloadFile: Invalid value for property `fromUrl`');
  }
  if (typeof options.toFile !== 'string') {
    throw new Error('downloadFile: Invalid value for property `toFile`');
  }
  if (options.headers && typeof options.headers !== 'object') {
    throw new Error('downloadFile: Invalid value for property `headers`');
  }
  if (options.background && typeof options.background !== 'boolean') {
    throw new Error('downloadFile: Invalid value for property `background`');
  }
  if (options.progressDivider && typeof options.progressDivider !== 'number') {
    throw new Error(
      'downloadFile: Invalid value for property `progressDivider`',
    );
  }
  if (
    options.progressInterval &&
    typeof options.progressInterval !== 'number'
  ) {
    throw new Error(
      'downloadFile: Invalid value for property `progressInterval`',
    );
  }
  if (options.readTimeout && typeof options.readTimeout !== 'number') {
    throw new Error('downloadFile: Invalid value for property `readTimeout`');
  }
  if (
    options.connectionTimeout &&
    typeof options.connectionTimeout !== 'number'
  ) {
    throw new Error(
      'downloadFile: Invalid value for property `connectionTimeout`',
    );
  }
  if (
    options.backgroundTimeout &&
    typeof options.backgroundTimeout !== 'number'
  ) {
    throw new Error(
      'downloadFile: Invalid value for property `backgroundTimeout`',
    );
  }

  const jobId = ++lastJobId;
  const subscriptions: EmitterSubscription[] = [];

  if (options.begin) {
    subscriptions.push(
      nativeEventEmitter.addListener('DownloadBegin', (res) => {
        if (res.jobId === jobId && options.begin) options.begin(res);
      }),
    );
  }

  if (options.progress) {
    subscriptions.push(
      nativeEventEmitter.addListener('DownloadProgress', (res) => {
        if (res.jobId === jobId && options.progress) options.progress(res);
      }),
    );
  }

  if (options.resumable) {
    subscriptions.push(
      nativeEventEmitter.addListener('DownloadResumable', (res) => {
        if (res.jobId === jobId && options.resumable) options.resumable(res);
      }),
    );
  }

  var nativeOptions: NativeDownloadFileOptions = {
    jobId: jobId,
    fromUrl: options.fromUrl,
    toFile: normalizeFilePath(options.toFile),
    background: !!options.background,
    backgroundTimeout: options.backgroundTimeout || 3600000, // 1 hour
    cacheable: !!options.cacheable,
    connectionTimeout: options.connectionTimeout || 5000,
    discretionary: !!options.discretionary,
    headers: options.headers || {},
    progressDivider: options.progressDivider || 0,
    progressInterval: options.progressInterval || 0,
    readTimeout: options.readTimeout || 15000,
    hasBeginCallback: !!options.begin,
    hasProgressCallback: !!options.progress,
    hasResumableCallback: !!options.resumable,
  };

  return {
    jobId,
    promise: (async () => {
      try {
        return await RNFS.downloadFile(nativeOptions);
      } finally {
        subscriptions.forEach((sub) => sub.remove());
      }
    })(),
  };
}

export function exists(filepath: string): Promise<boolean> {
  return RNFS.exists(normalizeFilePath(filepath));
}

export const getFSInfo: () => Promise<FSInfoResult> = RNFS.getFSInfo;

export const isResumable: (jobId: number) => Promise<boolean> =
  RNFS.isResumable;

export function mkdir(
  filepath: string,
  options: MkdirOptions = {},
): Promise<void> {
  return RNFS.mkdir(normalizeFilePath(filepath), options);
}

export function moveFile(
  filepath: string,
  destPath: string,
  options: FileOptions = {},
): Promise<void> {
  return RNFS.moveFile(
    normalizeFilePath(filepath),
    normalizeFilePath(destPath),
    options,
  );
}

export async function read(
  filepath: string,
  length: number = 0,
  position: number = 0,
  encodingOrOptions?: EncodingOptions,
): Promise<string> {
  const b64 = await RNFS.read(normalizeFilePath(filepath), length, position);
  return decode(b64, toEncoding(encodingOrOptions));
}

export function readFile(
  filepath: string,
  encodingOrOptions?: EncodingOptions,
): Promise<string> {
  return readFileGeneric(filepath, encodingOrOptions, RNFS.readFile);
}

export function readDir(dirpath: string): Promise<ReadDirItem[]> {
  return readDirGeneric(dirpath, RNFS.readDir);
}

// Node style version (lowercase d). Returns just the names
export async function readdir(dirpath: string): Promise<string[]> {
  const files = await RNFS.readDir(normalizeFilePath(dirpath));
  return files.map((file) => file.name || '');
}

export async function stat(filepath: string): Promise<StatResult> {
  const result = await RNFS.stat(normalizeFilePath(filepath));

  const { FileTypeDirectory, FileTypeRegular } = RNFS.getConstants();

  return {
    path: filepath,
    ctime: new Date(result.ctime * 1000),
    mtime: new Date(result.mtime * 1000),
    size: result.size,
    mode: result.mode,
    originalFilepath: result.originalFilepath,
    isFile: () => result.type === FileTypeRegular,
    isDirectory: () => result.type === FileTypeDirectory,
  };
}

export const stopDownload: (jobId: number) => void = RNFS.stopDownload;

export function touch(
  filepath: string,
  mtime?: Date,
  ctime?: Date,
): Promise<void> {
  if (ctime && !(ctime instanceof Date)) {
    throw new Error('touch: Invalid value for argument `ctime`');
  }
  if (mtime && !(mtime instanceof Date)) {
    throw new Error('touch: Invalid value for argument `mtime`');
  }

  return RNFS.touch(normalizeFilePath(filepath), {
    ctime: ctime && ctime.getTime(),
    mtime: mtime && mtime.getTime(),
  });
}

export function unlink(filepath: string): Promise<void> {
  return RNFS.unlink(normalizeFilePath(filepath));
}

export function uploadFiles(options: UploadFileOptions): {
  jobId: number;
  promise: Promise<UploadResult>;
} {
  const jobId = ++lastJobId;
  const subscriptions: EmitterSubscription[] = [];

  if (typeof options !== 'object') {
    throw new Error('uploadFiles: Invalid value for argument `options`');
  }
  if (typeof options.toUrl !== 'string') {
    throw new Error('uploadFiles: Invalid value for property `toUrl`');
  }
  if (!Array.isArray(options.files)) {
    throw new Error('uploadFiles: Invalid value for property `files`');
  }
  if (options.headers && typeof options.headers !== 'object') {
    throw new Error('uploadFiles: Invalid value for property `headers`');
  }
  if (options.fields && typeof options.fields !== 'object') {
    throw new Error('uploadFiles: Invalid value for property `fields`');
  }
  if (options.method && typeof options.method !== 'string') {
    throw new Error('uploadFiles: Invalid value for property `method`');
  }

  if (options.begin) {
    subscriptions.push(
      nativeEventEmitter.addListener('UploadBegin', options.begin),
    );
  } else if (options.beginCallback) {
    // Deprecated
    subscriptions.push(
      nativeEventEmitter.addListener('UploadBegin', options.beginCallback),
    );
  }

  if (options.progress) {
    subscriptions.push(
      nativeEventEmitter.addListener('UploadProgress', options.progress),
    );
  } else if (options.progressCallback) {
    // Deprecated
    subscriptions.push(
      nativeEventEmitter.addListener(
        'UploadProgress',
        options.progressCallback,
      ),
    );
  }

  var nativeOptions = {
    jobId: jobId,
    toUrl: options.toUrl,
    files: options.files,
    binaryStreamOnly: options.binaryStreamOnly || false,
    headers: options.headers || {},
    fields: options.fields || {},
    method: options.method || 'POST',
    hasBeginCallback:
      options.begin instanceof Function ||
      options.beginCallback instanceof Function,
    hasProgressCallback:
      options.progress instanceof Function ||
      options.progressCallback instanceof Function,
  };

  return {
    jobId,
    promise: RNFS.uploadFiles(nativeOptions).then((res: UploadResult) => {
      subscriptions.forEach((sub) => sub.remove());
      return res;
    }),
  };
}

export function write(
  filepath: string,
  contents: string,
  position: number = -1,
  encodingOrOptions?: EncodingOptions,
): Promise<void> {
  const b64 = encode(contents, toEncoding(encodingOrOptions));
  return RNFS.write(normalizeFilePath(filepath), b64, position);
}

export function writeFile(
  filepath: string,
  contents: string,
  encodingOrOptions?: EncodingOptions & FileOptions,
): Promise<void> {
  const b64 = encode(contents, toEncoding(encodingOrOptions));
  return RNFS.writeFile(
    normalizeFilePath(filepath),
    b64,
    typeof encodingOrOptions === 'object' ? encodingOrOptions : {},
  );
}

// Android-specific.

export function copyFileAssets(from: string, to: string): Promise<void> {
  return RNFS.copyFileAssets(normalizeFilePath(from), normalizeFilePath(to));
}

export function copyFileRes(from: string, to: string): Promise<void> {
  return RNFS.copyFileRes(from, normalizeFilePath(to));
}

export function existsAssets(filepath: string): Promise<boolean> {
  return RNFS.existsAssets(filepath);
}

export function existsRes(filename: string): Promise<boolean> {
  return RNFS.existsRes(filename);
}

export const getAllExternalFilesDirs: () => Promise<string[]> =
  RNFS.getAllExternalFilesDirs;

export function hash(filepath: string, algorithm: string): Promise<string> {
  return RNFS.hash(normalizeFilePath(filepath), algorithm);
}

export function readDirAssets(dirpath: string): Promise<ReadDirItem[]> {
  return readDirGeneric(dirpath, RNFS.readDirAssets);
}

export function readFileAssets(
  filepath: string,
  encodingOrOptions?: EncodingOptions,
): Promise<string> {
  return readFileGeneric(filepath, encodingOrOptions, RNFS.readFileAssets);
}

export function readFileRes(
  filename: string,
  encodingOrOptions?: EncodingOptions,
): Promise<string> {
  return readFileGeneric(filename, encodingOrOptions, RNFS.readFileRes);
}

export const scanFile = RNFS.scanFile;

// TODO: Not documented!
// setReadable for Android
export const setReadable: (
  filepath: string,
  readable: boolean,
  ownerOnly: boolean,
) => Promise<boolean> = RNFS.setReadable;

// iOS-specific

export const completeHandlerIOS: (jobId: number) => void =
  RNFS.completeHandlerIOS;

// iOS only
// Copies fotos from asset-library (camera-roll) to a specific location
// with a given width or height
// @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
export function copyAssetsFileIOS(
  imageUri: string,
  destPath: string,
  width: number,
  height: number,
  scale: number = 1.0,
  compression: number = 1.0,
  resizeMode: string = 'contain',
): Promise<string> {
  return RNFS.copyAssetsFileIOS(
    imageUri,
    destPath,
    width,
    height,
    scale,
    compression,
    resizeMode,
  );
}

// iOS only
// Copies fotos from asset-library (camera-roll) to a specific location
// with a given width or height
// @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
export const copyAssetsVideoIOS: (
  imageUri: string,
  destPath: string,
) => Promise<string> = RNFS.copyAssetsVideoIOS;

// TODO: This is presumably iOS-specific, it is not documented,
// so it should be double-checked, what it does.
export const pathForBundle: (bundle: string) => Promise<string> =
  RNFS.pathForBundle;

export const pathForGroup: (group: string) => Promise<string> =
  RNFS.pathForGroup;

export const resumeDownload: (jobId: number) => void = RNFS.resumeDownload;

export const stopUpload: (jobId: number) => void = RNFS.stopUpload;

// Windows-specific.

// Windows workaround for slow copying of large folders of files
export function copyFolder(filepath: string, destPath: string): Promise<void> {
  return RNFS.copyFolder(
    normalizeFilePath(filepath),
    normalizeFilePath(destPath),
  );
}

const {
  MainBundlePath,
  CachesDirectoryPath,
  ExternalCachesDirectoryPath,
  DocumentDirectoryPath,
  DownloadDirectoryPath,
  ExternalDirectoryPath,
  ExternalStorageDirectoryPath,
  TemporaryDirectoryPath,
  LibraryDirectoryPath,
  PicturesDirectoryPath, // For Windows
  FileProtectionKeys,
  RoamingDirectoryPath, // For Windows
} = RNFS.getConstants();

export {
  MainBundlePath,
  CachesDirectoryPath,
  ExternalCachesDirectoryPath,
  DocumentDirectoryPath,
  DownloadDirectoryPath,
  ExternalDirectoryPath,
  ExternalStorageDirectoryPath,
  TemporaryDirectoryPath,
  LibraryDirectoryPath,
  PicturesDirectoryPath, // For Windows
  FileProtectionKeys,
  RoamingDirectoryPath, // For Windows
};
