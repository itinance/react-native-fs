import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

// Note: It would be better to have all these type definitions in a dedicated
// module, however as of its current version RN's Codegen does not seem to handle
// type imports correctly.

export type DownloadBeginCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number; // The HTTP status code
  contentLength: number; // The total size in bytes of the download resource
  headers: Headers; // The HTTP response headers from the server
};

export type DownloadProgressCallbackResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  contentLength: number; // The total size in bytes of the download resource
  bytesWritten: number; // The number of bytes written to the file so far
};

/**
 * These are options expected by native implementations of downloadFile()
 * function.
 */
export type NativeDownloadFileOptions = {
  jobId: number;
  fromUrl: string; // URL to download file from
  toFile: string; // Local filesystem path to save the file to
  background: boolean; // Continue the download in the background after the app terminates (iOS only)
  backgroundTimeout: number; // Maximum time (in milliseconds) to download an entire resource (iOS only, useful for timing out background downloads)
  cacheable: boolean; // Whether the download can be stored in the shared NSURLCache (iOS only)
  connectionTimeout: number; // only supported on Android yet
  discretionary: boolean; // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
  headers: Headers; // An object of headers to be passed to the server
  progressDivider: number;
  progressInterval: number;
  readTimeout: number; // supported on Android and iOS
  hasBeginCallback: boolean;
  hasProgressCallback: boolean;
  hasResumableCallback: boolean;
};

export type DownloadFileOptions = {
  fromUrl: string; // URL to download file from
  toFile: string; // Local filesystem path to save the file to
  background?: boolean; // Continue the download in the background after the app terminates (iOS only)
  backgroundTimeout?: number; // Maximum time (in milliseconds) to download an entire resource (iOS only, useful for timing out background downloads)
  cacheable?: boolean; // Whether the download can be stored in the shared NSURLCache (iOS only)
  connectionTimeout?: number; // only supported on Android yet
  discretionary?: boolean; // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
  headers?: Headers; // An object of headers to be passed to the server
  progressDivider?: number;
  progressInterval?: number;
  readTimeout?: number; // supported on Android and iOS

  begin?: (res: DownloadBeginCallbackResult) => void;
  progress?: (res: DownloadProgressCallbackResult) => void;

  // TODO: Yeah, original typing did not have "res" argument at all,
  // but the code using this type actually passes an argument to
  // resumable. Should be double-checked, if we have this argument,
  // or drop it.
  resumable?: (res: unknown) => void; // only supported on iOS yet
};

export type DownloadResult = {
  jobId: number; // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
  statusCode: number; // The HTTP status code
  bytesWritten: number; // The number of bytes written to the file
};

export type FileOptions = {
  // iOS-specific.
  NSFileProtectionKey?: string;
};

export type FSInfoResult = {
  totalSpace: number; // The total amount of storage space on the device (in bytes).
  freeSpace: number; // The amount of available storage space on the device (in bytes).
};

// TODO: Are these names really needed, can be just called
// smth like a Map?
export type Headers = { [name: string]: string };
export type Fields = { [name: string]: string };

export type MkdirOptions = {
  // iOS-specific.
  NSURLIsExcludedFromBackupKey?: boolean;
  NSFileProtectionKey?: string;
};

export type ReadDirItem = {
  // Common.
  mtime?: Date | null; // The last modified date of the file
  name?: string; // The name of the item
  path: string; // The absolute path to the item
  size: number; // Size in bytes.

  // TODO: Can't these be just values rather than methods?
  // Ok.. the reason these are functions is that currently the library
  // receives from native side "type" field, and then in JS side it compares
  // it with system-dependent constant values for directory and file...
  // in other words... this is an unnecessary complication.
  isDirectory: () => boolean; // Is the file a directory?
  isFile: () => boolean; // Is the file just a file?

  // iOS-specific
  ctime?: Date | null; // The creation date of the file (iOS only)
};

// TODO: Essentially here StatResult is similar to ReadDirItem,
// but it does not contain Date fields, thus making it possible
// to pass it from native side, unlike ReadDirItem.

export type StatResult = {
  // TODO: why is this not documented?
  name?: string; // The name of the item.

  path: string; // The absolute path to the item
  size: number; // Size in bytes
  mode: number; // UNIX file mode
  ctime: Date | number; // Created date
  mtime: Date | number; // Last modified date

  // In case of content uri this is the pointed file path,
  // otherwise is the same as path.
  originalFilepath: string;

  // TODO: Can't these be just values?
  isFile: () => boolean; // Is the file just a file?
  isDirectory: () => boolean; // Is the file a directory?

  // TODO: This is temporary addition,
  // to make the code compile.
  type?: number;
};

export type NativeReadDirResItemT = {
  ctime: number;
  mtime: number;
  name: string;
  path: string;
  size: number;
  type: string;
};

type NativeStatResult = {
  ctime: number; // Created date
  mtime: number; // Last modified date
  size: number; // Size in bytes
  type: string;
  mode: number; // UNIX file mode

  // In case of content uri this is the pointed file path,
  // otherwise is the same as path.
  // TODO: This is not implemented on iOS
  originalFilepath: string;
};

export type UploadFileItem = {
  name: string; // Name of the file, if not defined then filename is used
  filename: string; // Name of file
  filepath: string; // Path to file
  filetype: string; // The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
};

export type UploadBeginCallbackResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
};

export type UploadProgressCallbackResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  totalBytesExpectedToSend: number; // The total number of bytes that will be sent to the server
  totalBytesSent: number; // The number of bytes sent to the server
};

export type UploadFileOptions = {
  toUrl: string; // URL to upload file to
  binaryStreamOnly?: boolean; // Allow for binary data stream for file to be uploaded without extra headers, Default is 'false'
  files: UploadFileItem[]; // An array of objects with the file information to be uploaded.
  headers?: Headers; // An object of headers to be passed to the server
  fields?: Fields; // An object of fields to be passed to the server
  method?: string; // Default is 'POST', supports 'POST' and 'PUT'
  beginCallback?: (res: UploadBeginCallbackResult) => void; // deprecated
  progressCallback?: (res: UploadProgressCallbackResult) => void; // deprecated
  begin?: (res: UploadBeginCallbackResult) => void;
  progress?: (res: UploadProgressCallbackResult) => void;
};

type NativeUploadFileOptions = {
  toUrl: string; // URL to upload file to
  binaryStreamOnly?: boolean; // Allow for binary data stream for file to be uploaded without extra headers, Default is 'false'
  files: UploadFileItem[]; // An array of objects with the file information to be uploaded.
  headers?: Headers; // An object of headers to be passed to the server
  fields?: Fields; // An object of fields to be passed to the server
  method?: string; // Default is 'POST', supports 'POST' and 'PUT'
};

export type UploadResult = {
  jobId: number; // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
  statusCode: number; // The HTTP status code
  headers: Headers; // The HTTP response headers from the server
  body: string; // The HTTP response body
};

type TouchOptions = {
  ctime?: number;
  mtime?: number;
};

export interface Spec extends TurboModule {
  readonly getConstants: () => {
    // System paths.
    CachesDirectoryPath: string;
    DocumentDirectoryPath: string;
    DownloadDirectoryPath: string;
    ExternalCachesDirectoryPath: string;
    ExternalDirectoryPath: string;
    ExternalStorageDirectoryPath: string;
    MainBundlePath?: string; // not on Android
    TemporaryDirectoryPath: string;

    // File system entity types.
    // TODO: At least on iOS there more file types we don't capture here:
    // https://developer.apple.com/documentation/foundation/nsfileattributetype?language=objc
    FileTypeRegular: string;
    FileTypeDirectory: string;

    // TODO: It was not declared in JS layer,
    // but it is exported constant on Android. Do we need it?
    DocumentDirectory: number;

    // iOS-specific
    LibraryDirectoryPath?: string;

    // Windows-specific.
    PicturesDirectoryPath?: string; // also on Android!
    RoamingDirectoryPath?: string;

    // NON-ANDROID-STUFF, AND NOT DOCUMENTED
    FileProtectionKeys?: string;
  };

  addListener(event: string): void;
  removeListeners(count: number): void;

  // Common.
  appendFile(path: string, b64: string): Promise<void>;
  copyFile(from: string, to: string, options: FileOptions): Promise<void>;
  downloadFile(options: NativeDownloadFileOptions): Promise<DownloadResult>;
  exists(path: string): Promise<boolean>;
  getFSInfo(): Promise<FSInfoResult>;
  hash(path: string, algorithm: string): Promise<string>;
  mkdir(path: string, options: MkdirOptions): Promise<void>;
  moveFile(from: string, to: string, options: FileOptions): Promise<void>;

  read(path: string, length: number, position: number): Promise<string>;
  readFile(path: string): Promise<string>;

  // TODO: Not sure about the type of result here.
  readDir(path: string): Promise<NativeReadDirResItemT[]>;

  stat(path: string): Promise<NativeStatResult>;
  stopDownload(jobId: number): void;
  stopUpload(jobId: number): void;
  touch(path: string, options: TouchOptions): Promise<void>;
  unlink(path: string): Promise<void>;
  uploadFiles(options: NativeUploadFileOptions): Promise<UploadResult>;
  write(path: string, b64: string, position: number): Promise<void>;
  writeFile(path: string, b64: string, options: FileOptions): Promise<void>;

  // Android-specific.
  copyFileAssets(from: string, to: string): Promise<void>;
  copyFileRes(from: string, to: string): Promise<void>;
  existsAssets(path: string): Promise<boolean>;
  existsRes(path: string): Promise<boolean>;
  getAllExternalFilesDirs(): Promise<string[]>;
  readFileAssets(path: string): Promise<string>;
  readFileRes(path: string): Promise<string>;
  readDirAssets(path: string): Promise<NativeReadDirResItemT[]>;
  scanFile(path: string): Promise<string>;

  setReadable(
    filepath: string,
    readable: boolean,
    ownerOnly: boolean,
  ): Promise<boolean>;

  // iOS-specific.
  copyAssetsFileIOS(
    imageUri: string,
    destPath: string,
    width: number,
    height: number,
    scale: number,
    compression: number,
    resizeMode: string,
  ): Promise<string>;

  copyAssetsVideoIOS(imageUri: string, destPath: string): Promise<string>;

  completeHandlerIOS(jobId: number): void;
  isResumable(jobId: number): Promise<boolean>;
  pathForBundle(bundle: string): Promise<string>;
  pathForGroup(group: string): Promise<string>;
  resumeDownload(jobId: number): void;

  // Windows-specific.
  copyFolder(from: string, to: string): Promise<void>;
}

export default TurboModuleRegistry.getEnforcing<Spec>('ReactNativeFs');
