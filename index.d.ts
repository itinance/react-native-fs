export type MkdirOptions = {
	/** (iOS only) */
	NSURLIsExcludedFromBackupKey?: boolean
	/** (iOS only) */
	NSFileProtectionKey?: string
}

export type FileOptions = {
	/** (iOS only) */
	NSFileProtectionKey?: string
}

export type ReadDirItem = {
	/** (iOS only) The creation date of the file */
	ctime: Date | undefined
	/** The last modified date of the file */
	mtime: Date | undefined
	/** The name of the item */
	name: string
	/** The absolute path to the item */
	path: string
	/** Size in bytes */
	size: string
	/** Is the file just a file? */
	isFile: () => boolean
	/** Is the file a directory? */
	isDirectory: () => boolean
}

export type StatResult = {
	/** The name of the item TODO: why is this not documented? */
	name: string | undefined
	/** The absolute path to the item */
	path: string
	/** Size in bytes */
	size: string
	/** UNIX file mode */
	mode: number
	/** Created date */
	ctime: number
	/** Last modified date */
	mtime: number
	/** In case of content uri this is the pointed file path, otherwise is the same as path */
	originalFilepath: string
	/** Is the file just a file? */
	isFile: () => boolean
	/** Is the file a directory? */
	isDirectory: () => boolean
}

type Headers = { [name: string]: string }
type Fields = { [name: string]: string }

export type DownloadFileOptions = {
	/** URL to download file from */
	fromUrl: string
	/** Local filesystem path to save the file to */
	toFile: string
	/** An object of headers to be passed to the server */
	headers?: Headers
	/** (iOS only) Continue the download in the background after the app terminates */
	background?: boolean
	/** (iOS only) Allow the OS to control the timing and speed of the download to improve perceived performance */
	discretionary?: boolean
	/** (iOS only) Whether the download can be stored in the shared NSURLCache */
	cacheable?: boolean
	/** 
	* (in milliseconds) Throttles the amount of progress callbacks invoked based on the maximum frequency of progressDivider.
	*
	* For example, if progressInterval = 100, you will not receive callbacks more often than every 100th millisecond.
	*/
	progressInterval?: number
	/** Invokes the progress callback every x percent. e.g. setting it to 10 will invoke the progress callback for 0, 10, 20, ..., 100 (Default: 0) */
	progressDivider?: number
	begin?: (res: DownloadBeginCallbackResult) => void
	progress?: (res: DownloadProgressCallbackResult) => void
	/** iOS only */
	resumable?: () => void
	/** Android only */
	connectionTimeout?: number
	/** Supported on Android and iOS */
	readTimeout?: number
	/** (iOS only) Maximum time (in milliseconds) to download an entire resource (useful for timing out background downloads) */
	backgroundTimeout?: number
}

type DownloadBeginCallbackResult = {
	/** The download job ID, required if one wishes to cancel the download. See `stopDownload`. */
	jobId: number
	/** The HTTP status code */
	statusCode: number
	/**  The total size in bytes of the download resource */
	contentLength: number
	/** The HTTP response headers from the server */
	headers: Headers
}

export type DownloadProgressCallbackResult = {
	/** The download job ID, required if one wishes to cancel the download. See `stopDownload`. */
	jobId: number
	/** The total size in bytes of the download resource */
	contentLength: number
	/** The number of bytes written to the file so far */
	bytesWritten: number
}

export type DownloadResult = {
	/** The download job ID, required if one wishes to cancel the download. See `stopDownload`. */
	jobId: number
	/** The HTTP status code */
	statusCode: number
	/** The number of bytes written to the file */
	bytesWritten: number
}

export type UploadFileOptions = {
	/** URL to upload file to */
	toUrl: string
	/** Allow for binary data stream for file to be uploaded without extra headers, Default is 'false' */
	binaryStreamOnly?: boolean
	/** An array of objects with the file information to be uploaded. */
	files: UploadFileItem[]
	/** An object of headers to be passed to the server */
	headers?: Headers
	/** An object of fields to be passed to the server */
	fields?: Fields
	/** Default is 'POST', supports 'POST' and 'PUT' */
	method?: string
	/** @deprecated */
	beginCallback?: (res: UploadBeginCallbackResult) => void
	/** @deprecated */
	progressCallback?: (res: UploadProgressCallbackResult) => void
	/** Callback when upload starts */
	begin?: (res: UploadBeginCallbackResult) => void
	/** Upload progress callback */
	progress?: (res: UploadProgressCallbackResult) => void
}

export type UploadFileItem = {
	/** Name of the file, if not defined then filename is used */
	name: string
	/** Name of file */
	filename: string
	/** Path to file */
	filepath: string
	/** The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension */
	filetype: string
}

export type UploadBeginCallbackResult = {
	/** The upload job ID, required if one wishes to cancel the upload. See `stopUpload`. */
	jobId: number
}

export type UploadProgressCallbackResult = {
	/** The upload job ID, required if one wishes to cancel the upload. See `stopUpload`. */
	jobId: number
	/** The total number of bytes that will be sent to the server */
	totalBytesExpectedToSend: number
	/** The number of bytes sent to the server */
	totalBytesSent: number
}

export type UploadResult = {
	/** The upload job ID, required if one wishes to cancel the upload. See `stopUpload` */
	jobId: number
	/** The HTTP status code */
	statusCode: number
	/** The HTTP response headers from the server */
	headers: Headers
	/** The HTTP response body */
	body: string
}

export type FSInfoResult = {
	/** The total amount of storage space on the device (in bytes). */
	totalSpace: number
	/** The amount of available storage space on the device (in bytes). */
	freeSpace: number
}

export function mkdir(filepath: string, options?: MkdirOptions): Promise<void>
export function moveFile(
	filepath: string,
	destPath: string,
	options?: FileOptions
): Promise<void>
export function copyFile(
	filepath: string,
	destPath: string,
	options?: FileOptions
): Promise<void>
export function pathForBundle(bundleNamed: string): Promise<string>
export function pathForGroup(groupName: string): Promise<string>
export function getFSInfo(): Promise<FSInfoResult>
export function getAllExternalFilesDirs(): Promise<string[]>
export function unlink(filepath: string): Promise<void>
export function exists(filepath: string): Promise<boolean>

export function stopDownload(jobId: number): void

export function resumeDownload(jobId: number): void

export function isResumable(jobId: number): Promise<boolean>

export function stopUpload(jobId: number): void

export function completeHandlerIOS(jobId: number): void

export function readDir(dirpath: string): Promise<ReadDirItem[]>

/**
 * Android-only
 */
export function scanFile(path: string): Promise<string[]>

/**
 * Android-only
 */
export function readDirAssets(dirpath: string): Promise<ReadDirItem[]>

/**
 * Android-only
 */
export function existsAssets(filepath: string): Promise<boolean>

/**
 * Android-only
 */
export function existsRes(filepath: string): Promise<boolean>

/**
 * Node style version (lowercase d). Returns just the names
 */
export function readdir(dirpath: string): Promise<string[]>

/**
 * Android-only
 */
export function setReadable(
	filepath: string,
	readable: boolean,
	ownerOnly: boolean
): Promise<boolean>

export function stat(filepath: string): Promise<StatResult>

export function readFile(
	filepath: string,
	encodingOrOptions?: any
): Promise<string>
export function read(
	filepath: string,
	length?: number,
	position?: number,
	encodingOrOptions?: any
): Promise<string>

/**
 * Android only
 */
export function readFileAssets(
	filepath: string,
	encodingOrOptions?: any
): Promise<string>

/**
 * Android only
 */
export function readFileRes(
	filepath: string,
	encodingOrOptions?: any
): Promise<string>

export function hash(filepath: string, algorithm: string): Promise<string>

/**
 * Android only
 */
export function copyFileAssets(
	filepath: string,
	destPath: string
): Promise<void>

/**
 * Android only
 */
export function copyFileRes(
	filepath: string,
	destPath: string
): Promise<void>

/**
 * iOS only
 * Copies fotos from asset-library (camera-roll) to a specific location
 * with a given width or height
 * @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
 */
export function copyAssetsFileIOS(
	imageUri: string,
	destPath: string,
	width: number,
	height: number,
	scale?: number,
	compression?: number,
	resizeMode?: string
): Promise<string>

/**
 * iOS only
 * Copies fotos from asset-library (camera-roll) to a specific location
 * with a given width or height
 * @see: https://developer.apple.com/reference/photos/phimagemanager/1616964-requestimageforasset
 */
export function copyAssetsVideoIOS(
	imageUri: string,
	destPath: string
): Promise<string>

export function writeFile(
	filepath: string,
	contents: string,
	encodingOrOptions?: any
): Promise<void>

export function appendFile(
	filepath: string,
	contents: string,
	encodingOrOptions?: string
): Promise<void>

export function write(
	filepath: string,
	contents: string,
	position?: number,
	encodingOrOptions?: any
): Promise<void>

export function downloadFile(
	options: DownloadFileOptions
): { jobId: number; promise: Promise<DownloadResult> }

export function uploadFiles(
	options: UploadFileOptions
): { jobId: number; promise: Promise<UploadResult> }

export function touch(
	filepath: string,
	mtime?: Date,
	ctime?: Date
): Promise<void>

export const MainBundlePath: string
export const CachesDirectoryPath: string
export const ExternalCachesDirectoryPath: string
export const DownloadDirectoryPath: string
export const DocumentDirectoryPath: string
export const ExternalDirectoryPath: string
export const ExternalStorageDirectoryPath: string
export const TemporaryDirectoryPath: string
export const LibraryDirectoryPath: string
export const PicturesDirectoryPath: string
export const FileProtectionKeys: string
