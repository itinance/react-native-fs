using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.Modules.Core;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;

namespace RNFS.Tests
{
    [TestClass]
    public class RNFSManagerTests
    {
        [TestMethod]
        public void RNFSManager_Constants()
        {
            var context = new ReactContext();
            var manager = new RNFSManager(context);
            var constants = manager.Constants;

            Assert.IsTrue(constants.Keys.Contains("RNFSCachesDirectoryPath"));
            Assert.IsTrue(constants.Keys.Contains("RNFSRoamingDirectoryPath"));
            Assert.IsTrue(constants.Keys.Contains("RNFSDocumentDirectoryPath"));
            Assert.IsTrue(constants.Keys.Contains("RNFSTemporaryDirectoryPath"));
            Assert.IsTrue(constants.Keys.Contains("RNFSFileTypeRegular"));
            Assert.IsTrue(constants.Keys.Contains("RNFSFileTypeDirectory"));
            Assert.AreNotEqual(constants["RNFSFileTypeRegular"], constants["RNFSFileTypeDirectory"]);
        }

        [TestMethod]
        public async Task RNFSManager_writeFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            var promise = new MockPromise();
            manager.writeFile(path, base64Content, new JObject(), promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual(hello, contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_writeFile_ExistingFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello Hello Earth");

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            var promise = new MockPromise();
            manager.writeFile(path, base64Content, new JObject(), promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual("Hello World Earth", contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_writeFile_ExistingDirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            var promise = new MockPromise();
            manager.writeFile(path, base64Content, new JObject(), promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_writeFile_Fail()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var promise = new MockPromise();
            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            manager.writeFile(path, "", new JObject(), promise);

            await AssertRejectAsync(
                promise,
                ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));
        }

        [TestMethod]
        public async Task RNFSManager_write()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            var promise = new MockPromise();
            manager.write(path, base64Content, 0, promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual(hello, contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_NegativeSeek_Ignored()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            var promise = new MockPromise();
            manager.write(path, base64Content, -1, promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual(hello, contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_ExistingFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            var promise = new MockPromise();
            manager.write(path, base64Content, bytes.Length, promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual(hello + hello, contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_ExistingDirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            var promise = new MockPromise();
            manager.write(path, base64Content, 0, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_NewFile_Seek()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            var promise = new MockPromise();
            manager.write(path, base64Content, 4, promise);
            await promise.Task;

            // Assert
            var actualBytes = File.ReadAllBytes(path);
            Assert.IsTrue(actualBytes.Skip(4).SequenceEqual(bytes));

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_Overwrite()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var earth = "Earth";
            var bytes = Encoding.UTF8.GetBytes(earth);
            var base64Content = Convert.ToBase64String(bytes);
            var promise = new MockPromise();
            manager.write(path, base64Content, 6, promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(path);
            Assert.AreEqual("Hello Earth", contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_write_Fail()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var promise = new MockPromise();
            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            manager.write(path, "", 0, promise);
            await AssertRejectAsync(
                promise,
                ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));
        }

        [TestMethod]
        public async Task RNFSManager_exists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test (true)
            var promise = new MockPromise();
            manager.exists(path, promise);
            var exists = await promise.Task;
            Assert.IsTrue(exists is bool);
            Assert.IsTrue((bool)exists);

            // Run test (false)
            var fakePath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            promise = new MockPromise();
            manager.exists(fakePath, promise);
            exists = await promise.Task;
            Assert.IsTrue(exists is bool);
            Assert.IsFalse((bool)exists);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_exists_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test (true)
            var promise = new MockPromise();
            manager.exists(path, promise);
            var exists = await promise.Task;
            Assert.IsTrue(exists is bool);
            Assert.IsTrue((bool)exists);

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_exists_InvalidPath()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test (false)
            var promise = new MockPromise();
            manager.exists(string.Concat(Path.GetInvalidPathChars()), promise);
            var exists = await promise.Task;
            Assert.IsTrue(exists is bool);
            Assert.IsFalse((bool)exists);
        }

        [TestMethod]
        public async Task RNFSManager_readFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test 
            var promise = new MockPromise();
            manager.readFile(path, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            var base64Content = (string)result;
            var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual(hello, contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_readFile_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.readFile(path, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_readFile_InvalidPath()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.readFile("badPath", promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_readFile_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test 
            var promise = new MockPromise();
            manager.readFile(path, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_read()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test (part 1)
            var promise = new MockPromise();
            manager.read(path, 5, 0, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            var base64Content = (string)result;
            var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual("Hello", contents);

            // Run test (part 2)
            promise = new MockPromise();
            manager.read(path, 5, 6, promise);
            result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            base64Content = (string)result;
            contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual("World", contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_read_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.read(path, 0, 0, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_read_InvalidPath()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.read("badPath", 0, 0, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_read_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var promise = new MockPromise();
            manager.read(path, 10, 0, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_read_MoreThanContent()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var promise = new MockPromise();
            manager.read(path, 100, 0, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            var base64Content = (string)result;
            var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual(
                hello + string.Join("", Enumerable.Repeat('\0', 100 - hello.Length)),
                contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_hash()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var promise = new MockPromise();
            manager.hash(path, "sha256", promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            var hash = (string)result;
            using (var algorithm = SHA256.Create())
            {
                algorithm.Initialize();
                var expectedHash = algorithm.ComputeHash(Encoding.UTF8.GetBytes(hello));
                var expectedHashString = string.Join("", expectedHash.Select(b => string.Format("{0:x2}", b)));
                Assert.AreEqual(expectedHashString, hash);
            }

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_hash_InvalidAlgorithm()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.hash(path, "foo", promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("Invalid hash algorithm.", ex.Message));
        }

        [TestMethod]
        public async Task RNFSManager_hash_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.hash(path, "sha256", promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManger_hash_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var promise = new MockPromise();
            manager.hash(path, "sha256", promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_moveFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.moveFile(path, newPath, new JObject(), promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(newPath);
            Assert.AreEqual(hello, contents);
            Assert.IsFalse(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(newPath);
        }

        [TestMethod]
        public async Task RNFSManager_moveFile_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.moveFile(path, newPath, new JObject(), promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_copyFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.copyFile(path, newPath, new JObject(), promise);
            await promise.Task;

            // Assert
            var contents = File.ReadAllText(newPath);
            Assert.AreEqual(hello, contents);
            Assert.IsTrue(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(path);
            File.Delete(newPath);
        }

        [TestMethod]
        public async Task RNFSManager_copyFile_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.copyFile(path, newPath, new JObject(), promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("ENOENT", ex.Code));
        }

        [TestMethod]
        public async Task RNFSManager_readDir()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var path = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var promise = new MockPromise();
            manager.readDir(dirpath, promise);
            var result = await promise.Task;
            var fileInfos = (JArray)result;
            Assert.AreEqual(1, fileInfos.Count);
            var item = fileInfos[0];
            var fileInfo = (JObject)item;
            Assert.AreEqual(0, fileInfo.Value<int>("type"));
            Assert.AreEqual(11, fileInfo.Value<int>("size"));
            Assert.AreEqual(path, fileInfo.Value<string>("path"));
            Assert.AreEqual(Path.GetFileName(path), fileInfo.Value<string>("name"));
            Assert.AreEqual(
                ConvertToUnixTimestamp(new FileInfo(path).LastWriteTimeUtc),
                fileInfo.Value<double>("mtime"));

            // Cleanup
            File.Delete(path);
            Directory.Delete(dirpath);
        }

        [TestMethod]
        public async Task RNFSManager_readDir_Empty()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);

            // Run test
            var promise = new MockPromise();
            manager.readDir(dirpath, promise);
            var result = await promise.Task;
            var fileInfos = (JArray)result;
            Assert.AreEqual(0, fileInfos.Count);

            // Cleanup
            Directory.Delete(dirpath);
        }

        [TestMethod]
        public async Task RNFSManager_readDir_Multiple()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var path1 = Path.Combine(dirpath, Guid.NewGuid().ToString());
            var path2 = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            var hello = "Hello World";
            File.WriteAllText(path1, hello);
            File.WriteAllText(path2, hello);

            // Run test
            var promise = new MockPromise();
            manager.readDir(dirpath, promise);
            var result = await promise.Task;
            var fileInfos = (JArray)result;
            Assert.AreEqual(2, fileInfos.Count);

            // Cleanup
            File.Delete(path1);
            File.Delete(path2);
            Directory.Delete(dirpath);
        }

        [TestMethod]
        public async Task RNFSManager_readDir_Subdirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            Directory.CreateDirectory(subdirpath);

            // Run test
            var promise = new MockPromise();
            manager.readDir(dirpath, promise);
            var result = await promise.Task;
            var fileInfos = (JArray)result;
            Assert.AreEqual(1, fileInfos.Count);
            var item = fileInfos[0];
            var fileInfo = (IDictionary<string, JToken>)item;
            Assert.IsTrue(fileInfo.ContainsKey("type"));
            Assert.AreEqual(1, fileInfo["type"].Value<int>());
            Assert.AreEqual(0, fileInfo["size"].Value<int>());
            Assert.AreEqual(subdirpath, fileInfo["path"].Value<string>());
            Assert.AreEqual(subdirpath.Split(Path.DirectorySeparatorChar).Last(), fileInfo["name"].Value<string>());
            Assert.AreEqual(
                ConvertToUnixTimestamp(new DirectoryInfo(subdirpath).LastWriteTimeUtc),
                fileInfo["mtime"].Value<double>());

            // Cleanup
            Directory.Delete(subdirpath);
            Directory.Delete(dirpath);
        }

        [TestMethod]
        public async Task RNFSManager_stat()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var promise = new MockPromise();
            manager.stat(path, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(JObject));
            var stat = (JObject)result;
            Assert.AreEqual(0, stat.Value<int>("type"));
            Assert.AreEqual(11, stat.Value<int>("size"));
            Assert.AreEqual(
                ConvertToUnixTimestamp(new FileInfo(path).CreationTimeUtc),
                stat.Value<double>("ctime"));
            Assert.AreEqual(
                ConvertToUnixTimestamp(new FileInfo(path).LastWriteTimeUtc),
                stat.Value<double>("mtime"));

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_stat_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.stat(path, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("File does not exist.", ex.Message));
        }

        [TestMethod]
        public async Task RNFSManager_stat_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var promise = new MockPromise();
            manager.stat(path, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(JObject));
            var stat = (JObject)result;
            Assert.AreEqual(1, stat.Value<int>("type"));
            Assert.AreEqual(0, stat.Value<int>("size"));
            Assert.AreEqual(
                ConvertToUnixTimestamp(new DirectoryInfo(path).CreationTimeUtc),
                stat.Value<double>("ctime"));
            Assert.AreEqual(
                ConvertToUnixTimestamp(new DirectoryInfo(path).LastWriteTimeUtc),
                stat.Value<double>("mtime"));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_unlink_Directory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var path1 = Path.Combine(dirpath, Guid.NewGuid().ToString());
            var path2 = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            var hello = "Hello World";
            File.WriteAllText(path1, hello);
            File.WriteAllText(path2, hello);

            // Run test
            var promise = new MockPromise();
            manager.unlink(dirpath, promise);
            await promise.Task;
            Assert.IsFalse(new DirectoryInfo(dirpath).Exists);
        }

        [TestMethod]
        public async Task RNFSManager_unlink_File()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var promise = new MockPromise();
            manager.unlink(path, promise);
            await promise.Task;
            Assert.IsFalse(new FileInfo(path).Exists);
        }

        [TestMethod]
        public async Task RNFSManager_unlink_Recursive()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(dirpath, Guid.NewGuid().ToString());
            var path = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(subdirpath);
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var promise = new MockPromise();
            manager.unlink(dirpath, promise);
            await promise.Task;
            Assert.IsFalse(new FileInfo(path).Exists);
            Assert.IsFalse(new DirectoryInfo(subdirpath).Exists);
            Assert.IsFalse(new DirectoryInfo(dirpath).Exists);
        }

        [TestMethod]
        public async Task RNFSManager_unlink_NotExists()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.unlink(path, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("File does not exist.", ex.Message));
        }

        [TestMethod]
        public async Task RNFSManager_mkdir()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.mkdir(path, null, promise);
            await promise.Task;

            // Assert
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_mkdir_Intermediate()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(path, Guid.NewGuid().ToString());
            var promise = new MockPromise();
            manager.mkdir(subdirpath, null, promise);
            await promise.Task;

            // Assert
            Assert.IsTrue(new DirectoryInfo(subdirpath).Exists);
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(subdirpath);
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_mkdir_ExistingFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var promise = new MockPromise();
            manager.mkdir(path, null, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual(ex.Message, $"Cannot create '{path}' because a file or directory with the same name already exists.", $"Message was {ex.Message}"));

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_mkdir_ExistingDirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var promise = new MockPromise();
            manager.mkdir(path, null, promise);
            await promise.Task;

            // Assert
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_mkdir_Fail()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var promise = new MockPromise();
            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            manager.mkdir(path, null, promise);

            await AssertRejectAsync(
                promise,
                ex => Assert.AreEqual($"Access to the path '{path}' is denied.", ex.Message, $"Message was {ex.Message}"));
        }

        [TestMethod]
        public async Task RNFSManager_getFSInfo()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var promise = new MockPromise();
            manager.getFSInfo(promise);
            var result = await promise.Task;
            var dictionary = result as IDictionary<string, JToken>;
            Assert.IsNotNull(dictionary);
            Assert.IsTrue(dictionary.ContainsKey("freeSpace"));
            Assert.IsTrue(dictionary.ContainsKey("totalSpace"));
        }

        [TestMethod]
        public async Task RNFSManager_touch()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));
            var promise = new MockPromise();
            manager.touch(
                path,
                ConvertToUnixTimestamp(mtime),
                ConvertToUnixTimestamp(ctime),
                promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            Assert.AreEqual(path, (string)result);
            var fileInfo = new FileInfo(path);
            Assert.IsTrue(fileInfo.Exists);
            Assert.AreEqual(ctime, fileInfo.CreationTime);
            Assert.AreEqual(mtime, fileInfo.LastWriteTime);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_touch_ExistingFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));
            var promise = new MockPromise();
            manager.touch(
                path,
                ConvertToUnixTimestamp(mtime),
                ConvertToUnixTimestamp(ctime),
                promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(string));
            Assert.AreEqual(path, (string)result);
            var fileInfo = new FileInfo(path);
            Assert.IsTrue(fileInfo.Exists);
            Assert.AreEqual(ctime, fileInfo.CreationTime);
            Assert.AreEqual(mtime, fileInfo.LastWriteTime);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public async Task RNFSManager_touch_ExistingDirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));
            var promise = new MockPromise();
            manager.touch(
                path,
                ConvertToUnixTimestamp(mtime),
                ConvertToUnixTimestamp(ctime),
                promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(JObject));
            var json = (JObject)result;
            Assert.AreEqual(1, json["jobId"].Value<int>());
            Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Value<long>());
            Assert.AreEqual(200, json["statusCode"].Value<long>());

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile_Progress()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context);
            var waitHandle = new AutoResetEvent(false);
            var progressCount = 0;
            manager.Emitter = CreateEventEmitter((name, data) =>
            {
                if (name == "DownloadProgress-1")
                {
                    progressCount++;
                }
            });

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/bytes/102400" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 10 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            await promise.Task;
            Assert.IsTrue(progressCount >= 10);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile_Headers()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Run test
            var expectedHeaders = new JObject
            {
                { "Accept", "foo/bar" },
                { "ContentLength", 0 },
                { "X-Custom-Header", "qux" }
            };

            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/headers" },
                { "jobId", 1 },
                { "headers", expectedHeaders },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            await promise.Task;
            var actualHeaders = (JObject)JObject.Parse(File.ReadAllText(path))["headers"];
            Assert.AreEqual(0, actualHeaders.Value<int>("ContentLength"));
            Assert.AreEqual("qux", actualHeaders.Value<string>("X-Custom-Header"));
            Assert.AreEqual("foo/bar", actualHeaders.Value<string>("Accept"));

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile_ExistingFile()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(JObject));
            var json = (JObject)result;
            Assert.AreEqual(1, json["jobId"].Value<int>());
            Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Value<long>());
            Assert.AreEqual(200, json["statusCode"].Value<long>());

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile_ExistingDirectory()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            await AssertRejectAsync(promise, ex => Assert.AreEqual(ex.Message, $"Access to the path '{path}' is denied.", $"Message was {ex.Message}"));

            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_downloadFile_Redirect()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/redirect/1" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            var result = await promise.Task;
            Assert.IsInstanceOfType(result, typeof(JObject));
            var json = (JObject)result;
            Assert.AreEqual(1, json["jobId"].Value<int>());
            Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Value<long>());
            Assert.AreEqual(200, json["statusCode"].Value<long>());

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_stopDownload()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/delay/10" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            manager.stopDownload(1);
            await AssertRejectAsync(promise, ex => Assert.AreEqual("A task was canceled.", ex.Message, ex.Message));
            Assert.IsFalse(new FileInfo(path).Exists);
        }

        [TestMethod]
        [TestCategory("Network")]
        public async Task RNFSManager_stopDownload_AfterComplete()
        {
            // Initialize module
            var context = new ReactContext();
            var manager = new RNFSManager(context)
            {
                Emitter = CreateEventEmitter((_, __) => { }),
            };

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new JObject
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/delay/1" },
                { "jobId", 1 },
                { "headers", new JObject() },
                { "progressDivider", 100 },
            };

            var promise = new MockPromise();
            manager.downloadFile(options, promise);
            await promise.Task;
            manager.stopDownload(1);
            Assert.IsTrue(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(path);
        }

        private static async Task AssertRejectAsync(MockPromise promise, Action<RejectException> assert)
        {
            try
            {
                await promise.Task;
            }
            catch (RejectException ex)
            {
                assert(ex);
                return;
            }

            Assert.Fail("No exception thrown.");
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

        public static RCTNativeAppEventEmitter CreateEventEmitter(Action<string, object> onEmit)
        {
            return new RCTNativeAppEventEmitter
            {
                InvocationHandler = new MockInvocationHandler((name, args) =>
                {
                    if (name == "emit")
                    {
                        var eventName = (string)args[0];
                        var eventData = args[1];
                        onEmit(eventName, eventData);
                        return;
                    }

                    throw new NotSupportedException();
                }),
            };
        }

        class MockPromise : IPromise
        {
            private readonly TaskCompletionSource<object> _tcs = new TaskCompletionSource<object>();
            public Task<object> Task
            {
                get
                {
                    return _tcs.Task;
                }
            }

            public void Reject(string code, string message)
            {
                _tcs.SetException(new RejectException(code, message));
            }

            public void Reject(string code, Exception exception)
            {
                _tcs.SetException(new RejectException(code, null, exception));
            }

            public void Reject(string code, string message, Exception exception)
            {
                _tcs.SetException(new RejectException(code, message, exception));
            }

            public void Reject(string message)
            {
                _tcs.SetException(new RejectException(null, message));
            }

            public void Reject(Exception exception)
            {
                _tcs.SetException(new RejectException(null, null, exception));
            }

            public void Reject(string code, string message, string stack, JToken userInfo)
            {
              _tcs.SetException(new RejectException(code, message));
            }

            public void Resolve(object value)
            {
              _tcs.SetResult(value);
            }
    }

        class RejectException : Exception
        {
            public RejectException(string code, string message, Exception innerException)
                : base(message, innerException)
            {
                Code = code; 
            }

            public RejectException(string code, string message)
                : base(message)
            {
                Code = code;
            }

            public string Code { get; }
        }

        class MockInvocationHandler : IInvocationHandler
        {
            private readonly Action<string, object[]> _onInvoke;
            public MockInvocationHandler(Action<string, object[]>  onInvoke)
            {
                _onInvoke = onInvoke;
            }

            public void Invoke(string name, object[] args)
            {
                _onInvoke(name, args);
            }
        }
    }
}