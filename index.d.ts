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
export function getAllExternalFilesDirs(): Promise<string>
export function unlink(filepath: string): Promise<void>
export function exists(filepath: string): Promise<boolean>
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

export function touch(
	filepath: string,
	mtime?: Date,
	ctime?: Date
): Promise<void>

// not accurate on ios
export function canOpenFile(
	filepath: string,
	scheme?: string,
): Promise<void>

export function openFile(
	filepath: string,
	scheme?: string,
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
