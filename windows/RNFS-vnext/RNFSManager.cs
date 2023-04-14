using Microsoft.ReactNative.Managed;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Storage;

namespace RNFSvnext
{
    [ReactModule]
    class RNFSManager
    {

        private static readonly IReadOnlyDictionary<string, Func<HashAlgorithm>> s_hashAlgorithms =
            new Dictionary<string, Func<HashAlgorithm>>
            {
                { "md5", () => MD5.Create() },
                { "sha1", () => SHA1.Create() },
                { "sha256", () => SHA256.Create() },
                { "sha384", () => SHA384.Create() },
                { "sha512", () => SHA512.Create() },
            };

        private readonly TaskCancellationManager<int> _tasks = new TaskCancellationManager<int>();
        private readonly HttpClient _httpClient = new HttpClient();

        [ReactConstant]
        public string RNFSMainBundlePath = Package.Current.InstalledLocation.Path;

        [ReactConstant]
        public string RNFSCachesDirectoryPath = ApplicationData.Current.LocalCacheFolder.Path;

        [ReactConstant]
        public string RNFSRoamingDirectoryPath = ApplicationData.Current.RoamingFolder.Path;

        [ReactConstant]
        public string RNFSDocumentDirectoryPath = ApplicationData.Current.LocalFolder.Path;

        [ReactConstant]
        public string RNFSTemporaryDirectoryPath = ApplicationData.Current.TemporaryFolder.Path;

        [ReactConstant]
        public int RNFSFileTypeRegular = 0;

        [ReactConstant]
        public int RNFSFileTypeDirectory = 1;

        [ReactConstant]
        public string RNFSExternalDirectoryPath
        {
            get
            {
                var external = GetFolderPathSafe(() => KnownFolders.RemovableDevices);
                if (external != null)
                {
                    var externalItems = KnownFolders.RemovableDevices.GetItemsAsync().AsTask().Result;
                    if (externalItems.Count > 0)
                    {
                        return externalItems[0].Path;
                    }
                }
                return "";
            }
        }

        [ReactConstant]
        public string[] RNFSExternalDirectoryPaths
        {
            get
            {
                var external = GetFolderPathSafe(() => KnownFolders.RemovableDevices);
                if (external != null)
                {
                    var externalItems = KnownFolders.RemovableDevices.GetItemsAsync().AsTask().Result;
                    return externalItems.Select(i => i.Path).ToArray();
                }
                return new string[] { };
            }
        }

        [ReactConstant]
        public string RNFSPicturesDirectoryPath
        {
            get
            {
                var pictures = GetFolderPathSafe(() => KnownFolders.PicturesLibrary);
                if (pictures != null)
                {
                    return pictures;
                }
                return "";
            }
        }


        [ReactMethod]
        public async void writeFile(string filepath, string base64Content, IReadOnlyDictionary<string, JSValue> options, IReactPromise<int> promise)
        {
            try
            {
                // TODO: open file on background thread?
                using (var file = File.OpenWrite(filepath))
                {
                    var data = Convert.FromBase64String(base64Content);
                    await file.WriteAsync(data, 0, data.Length).ConfigureAwait(false);
                }

                promise.Resolve(0);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void appendFile(string filepath, string base64Content, IReactPromise<int> promise)
        {
            try
            {
                // TODO: open file on background thread?
                using (var file = File.Open(filepath, FileMode.Append))
                {
                    var data = Convert.FromBase64String(base64Content);
                    await file.WriteAsync(data, 0, data.Length).ConfigureAwait(false);
                }

                promise.Resolve(0);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void write(string filepath, string base64Content, int position, IReactPromise<int> promise)
        {
            try
            {
                // TODO: open file on background thread?
                using (var file = File.OpenWrite(filepath))
                {
                    if (position >= 0)
                    {
                        file.Position = position;
                    }

                    var data = Convert.FromBase64String(base64Content);
                    await file.WriteAsync(data, 0, data.Length).ConfigureAwait(false);
                }

                promise.Resolve(0);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public void exists(string filepath, IReactPromise<bool> promise)
        {
            try
            {
                promise.Resolve(File.Exists(filepath) || Directory.Exists(filepath));
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void readFile(string filepath, IReactPromise<string> promise)
        {
            try
            {
                if (!File.Exists(filepath))
                {
                    RejectFileNotFound(promise, filepath);
                    return;
                }

                // TODO: open file on background thread?
                string base64Content;
                using (var file = File.OpenRead(filepath))
                {
                    var length = (int)file.Length;
                    var buffer = new byte[length];
                    await file.ReadAsync(buffer, 0, length).ConfigureAwait(false);
                    base64Content = Convert.ToBase64String(buffer);
                }

                promise.Resolve(base64Content);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void read(string filepath, int length, int position, IReactPromise<string> promise)
        {
            try
            {
                if (!File.Exists(filepath))
                {
                    RejectFileNotFound(promise, filepath);
                    return;
                }

                // TODO: open file on background thread?
                string base64Content;
                using (var file = File.OpenRead(filepath))
                {
                    file.Position = position;
                    var buffer = new byte[length];
                    await file.ReadAsync(buffer, 0, length).ConfigureAwait(false);
                    base64Content = Convert.ToBase64String(buffer);
                }

                promise.Resolve(base64Content);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void hash(string filepath, string algorithm, IReactPromise<string> promise)
        {
            var hashAlgorithmFactory = default(Func<HashAlgorithm>);
            if (!s_hashAlgorithms.TryGetValue(algorithm, out hashAlgorithmFactory))
            {
                ReactError err = new ReactError();
                err.Message = "Invalid hash algorithm";
                promise.Reject(err);
                return;
            }

            try
            {
                if (!File.Exists(filepath))
                {
                    RejectFileNotFound(promise, filepath);
                    return;
                }

                await Task.Run(() =>
                {
                    var hexBuilder = new StringBuilder();
                    using (var hashAlgorithm = hashAlgorithmFactory())
                    {
                        hashAlgorithm.Initialize();
                        var hash = default(byte[]);
                        using (var file = File.OpenRead(filepath))
                        {
                            hash = hashAlgorithm.ComputeHash(file);
                        }

                        foreach (var b in hash)
                        {
                            hexBuilder.Append(string.Format("{0:x2}", b));
                        }
                    }

                    promise.Resolve(hexBuilder.ToString());
                }).ConfigureAwait(false);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public void moveFile(string filepath, string destPath, JSValue options, IReactPromise<bool> promise)
        {
            try
            {
                // TODO: move file on background thread?
                File.Move(filepath, destPath);
                promise.Resolve(true);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void copyFile(string filepath, string destPath, JSValue options, IReactPromise<int> promise)
        {
            try
            {
                await Task.Run(() => File.Copy(filepath, destPath)).ConfigureAwait(false);
                promise.Resolve(0);

            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void readDir(string directory, IReactPromise<JSValue> promise)
        {
            try
            {
                await Task.Run(() =>
                {
                    var info = new DirectoryInfo(directory);
                    if (!info.Exists)
                    {
                        var err = new ReactError();
                        err.Message = "Folder does not exist";
                        promise.Reject(err);
                        return;
                    }

                    var fileMaps = new List<JSValue>();
                    foreach (var item in info.EnumerateFileSystemInfos())
                    {
                        var fileMap = new Dictionary<string, JSValue>()
                        {
                            { "mtime", new JSValue(ConvertToUnixTimestamp(item.LastWriteTime)) },
                            { "name", new JSValue(item.Name) },
                            { "path", new JSValue(item.FullName) },
                        };

                        var fileItem = item as FileInfo;
                        if (fileItem != null)
                        {
                            fileMap.Add("type", new JSValue(RNFSFileTypeRegular));
                            fileMap.Add("size", new JSValue(fileItem.Length));
                        }
                        else
                        {
                            fileMap.Add("type", new JSValue(RNFSFileTypeDirectory));
                            fileMap.Add("size", new JSValue(0));
                        }

                        fileMaps.Add(new JSValue(fileMap));
                    }

                    promise.Resolve(new JSValue(fileMaps));
                });
            }
            catch (Exception ex)
            {
                Reject(promise, directory, ex);
            }
        }

        [ReactMethod]
        public void stat(string filepath, IReactPromise<JSValue> promise)
        {
            try
            {
                FileSystemInfo fileSystemInfo = new FileInfo(filepath);
                if (!fileSystemInfo.Exists)
                {
                    fileSystemInfo = new DirectoryInfo(filepath);
                    if (!fileSystemInfo.Exists)
                    {
                        ReactError err = new ReactError();
                        err.Message = "File does not exist.";
                        promise.Reject(err);
                        return;
                    }
                }

                var fileInfo = fileSystemInfo as FileInfo;
                var statMap = new Dictionary<string, JSValue>
                {
                    { "ctime", new JSValue(ConvertToUnixTimestamp(fileSystemInfo.CreationTime)) },
                    { "mtime", new JSValue(ConvertToUnixTimestamp(fileSystemInfo.LastWriteTime)) },
                    { "size", new JSValue(fileInfo?.Length ?? 0) },
                    { "type", new JSValue(fileInfo != null ? RNFSFileTypeRegular: RNFSFileTypeDirectory) },
                };

                promise.Resolve(new JSValue(statMap));
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void unlink(string filepath, IReactPromise<int> promise)
        {
            try
            {
                var directoryInfo = new DirectoryInfo(filepath);
                var fileInfo = default(FileInfo);
                if (directoryInfo.Exists)
                {
                    await Task.Run(() => Directory.Delete(filepath, true)).ConfigureAwait(false);
                }
                else if ((fileInfo = new FileInfo(filepath)).Exists)
                {
                    await Task.Run(() => File.Delete(filepath)).ConfigureAwait(false);
                }
                else
                {
                    var err = new ReactError();
                    err.Message = "File does not exist.";
                    promise.Reject(err);
                    return;
                }

                promise.Resolve(0);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void mkdir(string filepath, JSValue options, IReactPromise<int> promise)
        {
            try
            {
                await Task.Run(() => Directory.CreateDirectory(filepath)).ConfigureAwait(false);
                promise.Resolve(0);
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public async void downloadFile(IReadOnlyDictionary<string, JSValue> options, IReactPromise<IReadOnlyDictionary<string, JSValue>> promise)
        {

            var filepath = options["toFile"].To<string>();

            try
            {
                var url = new Uri(options["fromUrl"].To<string>());
                var jobId = options["jobId"].To<int>();
                var headers = options["headers"].To<IReadOnlyDictionary<string, JSValue>>();
                var progressDivider = options["progressDivider"].To<int>();

                var request = new HttpRequestMessage(HttpMethod.Get, url);
                foreach (var header in headers)
                {
                    request.Headers.Add(header.Key, header.Value.To<string>());
                }

                await _tasks.AddAndInvokeAsync(jobId, token =>
                    ProcessRequestAsync(promise, request, filepath, jobId, progressDivider, token));
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        [ReactMethod]
        public void stopDownload(int jobId)
        {
            _tasks.Cancel(jobId);
        }

        [ReactMethod]
        public async void getFSInfo(IReactPromise<IReadOnlyDictionary<string, JSValue>> promise)
        {
            try
            {
                var properties = await ApplicationData.Current.LocalFolder.Properties.RetrievePropertiesAsync(
                    new[]
                    {
                        "System.FreeSpace",
                        "System.Capacity",
                    })
                    .AsTask()
                    .ConfigureAwait(false);

                promise.Resolve(new Dictionary<string, JSValue>
                {
                    { "freeSpace", new JSValue((ulong)properties["System.FreeSpace"]) },
                    { "totalSpace", new JSValue((ulong)properties["System.Capacity"]) },
                });
            }
            catch (Exception ex)
            {
                var err = new ReactError();
                err.Exception = ex;

                promise.Reject(err);
            }
        }

        [ReactMethod]
        public async void touch(string filepath, double mtime, double ctime, IReactPromise<string> promise)
        {
            try
            {
                await Task.Run(() =>
                {
                    var fileInfo = new FileInfo(filepath);
                    if (!fileInfo.Exists)
                    {
                        using (File.Create(filepath)) { }
                    }

                    fileInfo.CreationTimeUtc = ConvertFromUnixTimestamp(ctime);
                    fileInfo.LastWriteTimeUtc = ConvertFromUnixTimestamp(mtime);

                    promise.Resolve(fileInfo.FullName);
                });
            }
            catch (Exception ex)
            {
                Reject(promise, filepath, ex);
            }
        }

        
        ~RNFSManager()
        {
            _tasks.CancelAllTasks();
            _httpClient.Dispose();
        }

        [ReactEvent]
        public Action<JSValue> DownloadBegin = null;

        [ReactEvent]
        public Action<JSValue> DownloadProgress = null;

        private async Task ProcessRequestAsync(IReactPromise<IReadOnlyDictionary<string, JSValue>> promise, HttpRequestMessage request, string filepath, int jobId, int progressIncrement, CancellationToken token)
        {
            try
            {
                using (var response = await _httpClient.SendAsync(request, HttpCompletionOption.ResponseHeadersRead, token))
                {
                    var headersMap = new Dictionary<string, JSValue>();
                    foreach (var header in response.Headers)
                    {
                        headersMap.Add(header.Key, new JSValue(string.Join(",", header.Value)));
                    }

                    var contentLength = response.Content.Headers.ContentLength ;
                    var beginEvent = new JSValue(new Dictionary<string, JSValue>
                    {
                        { "jobId", new JSValue(jobId) },
                        { "statusCode", new JSValue((int)response.StatusCode) },
                        { "contentLength", contentLength.HasValue ? new JSValue( contentLength.Value ) : JSValue.Null },
                        { "headers", new JSValue(headersMap) }
                    });
                    
                    // TODO this is throwing exception
                    DownloadBegin(beginEvent);
                    

                    // TODO: open file on background thread?
                    long totalRead = 0;
                    using (var fileStream = File.OpenWrite(filepath))
                    using (var stream = await response.Content.ReadAsStreamAsync())
                    {
                        var contentLengthForProgress = contentLength ?? -1;
                        var nextProgressIncrement = progressIncrement;
                        var buffer = new byte[8 * 1024];
                        var read = 0;
                        while ((read = await stream.ReadAsync(buffer, 0, buffer.Length)) > 0)
                        {
                            token.ThrowIfCancellationRequested();

                            await fileStream.WriteAsync(buffer, 0, read);
                            if (contentLengthForProgress >= 0)
                            {
                                totalRead += read;
                                if (totalRead * 100 / contentLengthForProgress >= nextProgressIncrement ||
                                    totalRead == contentLengthForProgress)
                                {
                                    var progressEvent = new JSValue(
                                        new Dictionary<string, JSValue>

                                        {
                                            { "jobId", new JSValue(jobId) },
                                            { "contentLength", contentLength.HasValue ? new JSValue( contentLength.Value ) : JSValue.Null },
                                            { "bytesWritten", new JSValue(totalRead) }
                                        }
                                    );

                                    // TODO this is throwing exception
                                    DownloadProgress(progressEvent);

                                    nextProgressIncrement += progressIncrement;
                                }
                            }
                        }
                    }

                    promise.Resolve(new Dictionary<string, JSValue>()
                    {
                        { "jobId", new JSValue(jobId) },
                        { "statusCode", new JSValue((int)response.StatusCode) },
                        { "bytesWritten", new JSValue(totalRead) },
                    });
                }
            }
            catch (OperationCanceledException ex)
            {
                var err = new ReactError();
                err.Exception = ex;
                promise.Reject(err);
            }
            finally
            {
                request.Dispose();
            }
        }

        private void Reject<T>(IReactPromise<T> promise, String filepath, Exception ex)
        {
            if (ex is FileNotFoundException) {
                RejectFileNotFound(promise, filepath);
                return;
            }
            ReactError err = new ReactError{ Exception = ex };
            promise.Reject(err);
        }

        private void RejectFileNotFound<T>(IReactPromise<T> promise, String filepath)
        {
            ReactError err = new ReactError { Message = "ENOENT: no such file or directory, open '" + filepath + "'" };
            promise.Reject(err);
        }

        private static string GetFolderPathSafe(Func<StorageFolder> getFolder)
        {
            try
            {
                return getFolder().Path;
            }
            catch (UnauthorizedAccessException)
            {
                return null;
            }
        }

        public static double ConvertToUnixTimestamp(DateTime date)
        {
            var origin = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
            var diff = date.ToUniversalTime() - origin;
            return Math.Floor(diff.TotalSeconds);
        }

        public static DateTime ConvertFromUnixTimestamp(double timestamp)
        {
            var origin = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
            var diff = TimeSpan.FromSeconds(timestamp);
            var dateTimeUtc = origin + diff;
            return dateTimeUtc.ToLocalTime();
        }
    }
}
