type MkdirOptions = {
	NSURLIsExcludedFromBackupKey?: boolean // iOS only
	NSFileProtectionKey?: string // IOS only
}

type FileOptions = {
	NSFileProtectionKey?: string // IOS only
}

type ReadDirItem = {
	ctime: Date | undefined // The creation date of the file (iOS only)
	mtime: Date | undefined // The last modified date of the file
	name: string // The name of the item
	path: string // The absolute path to the item
	size: string // Size in bytes
	isFile: () => boolean // Is the file just a file?
	isDirectory: () => boolean // Is the file a directory?
}

type StatResult = {
	name: string | undefined // The name of the item TODO: why is this not documented?
	path: string // The absolute path to the item
	size: string // Size in bytes
	mode: number // UNIX file mode
	ctime: number // Created date
	mtime: number // Last modified date
	originalFilepath: string // In case of content uri this is the pointed file path, otherwise is the same as path
	isFile: () => boolean // Is the file just a file?
	isDirectory: () => boolean // Is the file a directory?
}

type Headers = { [name: string]: string }
type Fields = { [name: string]: string }

type DownloadFileOptions = {
	fromUrl: string // URL to download file from
	toFile: string // Local filesystem path to save the file to
	headers?: Headers // An object of headers to be passed to the server
	background?: boolean // Continue the download in the background after the app terminates (iOS only)
	discretionary?: boolean // Allow the OS to control the timing and speed of the download to improve perceived performance  (iOS only)
	cacheable?: boolean // Whether the download can be stored in the shared NSURLCache (iOS only)
	progressInterval?: number
	progressDivider?: number
	begin?: (res: DownloadBeginCallbackResult) => void
	progress?: (res: DownloadProgressCallbackResult) => void
	resumable?: () => void // only supported on iOS yet
	connectionTimeout?: number // only supported on Android yet
	readTimeout?: number // supported on Android and iOS
	backgroundTimeout?: number // Maximum time (in milliseconds) to download an entire resource (iOS only, useful for timing out background downloads)
}

type DownloadBeginCallbackResult = {
	jobId: number // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
	statusCode: number // The HTTP status code
	contentLength: number // The total size in bytes of the download resource
	headers: Headers // The HTTP response headers from the server
}

type DownloadProgressCallbackResult = {
	jobId: number // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
	contentLength: number // The total size in bytes of the download resource
	bytesWritten: number // The number of bytes written to the file so far
}

type DownloadResult = {
	jobId: number // The download job ID, required if one wishes to cancel the download. See `stopDownload`.
	statusCode: number // The HTTP status code
	bytesWritten: number // The number of bytes written to the file
}

type UploadFileOptions = {
	toUrl: string // URL to upload file to
	binaryStreamOnly?: boolean // Allow for binary data stream for file to be uploaded without extra headers, Default is 'false'
	files: UploadFileItem[] // An array of objects with the file information to be uploaded.
	headers?: Headers // An object of headers to be passed to the server
	fields?: Fields // An object of fields to be passed to the server
	method?: string // Default is 'POST', supports 'POST' and 'PUT'
	beginCallback?: (res: UploadBeginCallbackResult) => void // deprecated
	progressCallback?: (res: UploadProgressCallbackResult) => void // deprecated
	begin?: (res: UploadBeginCallbackResult) => void
	progress?: (res: UploadProgressCallbackResult) => void
}

type UploadFileItem = {
	name: string // Name of the file, if not defined then filename is used
	filename: string // Name of file
	filepath: string // Path to file
	filetype: string // The mimetype of the file to be uploaded, if not defined it will get mimetype from `filepath` extension
}

type UploadBeginCallbackResult = {
	jobId: number // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
}

type UploadProgressCallbackResult = {
	jobId: number // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
	totalBytesExpectedToSend: number // The total number of bytes that will be sent to the server
	totalBytesSent: number // The number of bytes sent to the server
}

type UploadResult = {
	jobId: number // The upload job ID, required if one wishes to cancel the upload. See `stopUpload`.
	statusCode: number // The HTTP status code
	headers: Headers // The HTTP response headers from the server
	body: string // The HTTP response body
}

type FSInfoResult = {
	totalSpace: number // The total amount of storage space on the device (in bytes).
	freeSpace: number // The amount of available storage space on the device (in bytes).
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
