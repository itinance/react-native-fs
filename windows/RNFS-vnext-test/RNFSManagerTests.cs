
using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Newtonsoft.Json.Linq;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;
using Microsoft.ReactNative.Managed;

namespace RNFSvnext.Test
{
    [TestClass]
    public class RNFSManagerTests
    {
        private Microsoft.ReactNative.Managed.UnitTests.ReactModuleBuilderMock m_moduleBuilder;
        private ReactModuleInfo m_moduleInfo;
        private RNFSManager m_module;

        [TestInitialize]
        public void Initialize()
        {
            m_moduleBuilder = new Microsoft.ReactNative.Managed.UnitTests.ReactModuleBuilderMock();
            m_moduleInfo = new ReactModuleInfo(typeof(RNFSManager));
            m_module = (RNFSManager)m_moduleBuilder.CreateModule<RNFSManager>(m_moduleInfo);
        }

        [TestMethod]
        public void RNFSManager_Constants()
        {


            Assert.IsTrue(m_module.RNFSCachesDirectoryPath.Length > 0);
            Assert.IsTrue(m_module.RNFSRoamingDirectoryPath.Length > 0);
            Assert.IsTrue(m_module.RNFSDocumentDirectoryPath.Length > 0);
            Assert.IsTrue(m_module.RNFSTemporaryDirectoryPath.Length > 0);

            Assert.AreNotEqual(m_module.RNFSFileTypeRegular, m_module.RNFSFileTypeDirectory);
        }

        [TestMethod]
        public void RNFSManager_writeFile()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            m_moduleBuilder.Call2("writeFile", path, base64Content, new JSValue(0),
                (int result) =>
                {
                    // Assert
                    var contents = File.ReadAllText(path);
                    Assert.AreEqual(hello, contents);

                    // Cleanup
                    File.Delete(path);

                },
                (JSValue error) =>
                {
                    Assert.Fail();
                }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);

        }

        [TestMethod]
        public void RNFSManager_writeFile_ExistingFile()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello Hello Earth");

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));

            m_moduleBuilder.Call2("writeFile", path, base64Content, new JSValue(0),
               (int result) =>
               {
                   var contents = File.ReadAllText(path);
                   Assert.AreEqual("Hello World Earth", contents);

                   // Cleanup
                   File.Delete(path);

               },
               (JSValue error) =>
               {
                   Assert.Fail();

               }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }


        [TestMethod]
        public void RNFSManager_writeFile_ExistingDirectory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            m_moduleBuilder.Call2("writeFile", path, base64Content, new JSValue(0),
                          (int result) =>
                          {
                              Assert.Fail();
                          },
                          (JSValue error) =>
                          {
                              var message = error.Object["message"].String;
                              Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");

                              //Cleanup
                              Directory.Delete(path);

                          }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_writeFile_Fail()
        {

            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            m_moduleBuilder.Call2("writeFile", path, "", new JSValue(0),
                          (int result) =>
                          {
                              Assert.Fail();
                          },
                          (JSValue error) =>
                          {
                              var message = error.Object["message"].String;

                              Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");

                          }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_write()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            m_moduleBuilder.Call2("write", path, base64Content, 0,
                          (int result) =>
                          {
                              // Assert
                              var contents = File.ReadAllText(path);
                              Assert.AreEqual(hello, contents);
                              // Cleanup
                              File.Delete(path);
                          },
                          (JSValue error) =>
                          {
                              Assert.Fail();
                          }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_write_NegativeSeek_Ignored()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            m_moduleBuilder.Call2("write", path, base64Content, -1,
                         (int result) =>
                         {
                             // Assert
                             var contents = File.ReadAllText(path);
                             Assert.AreEqual(hello, contents);
                             // Cleanup
                             File.Delete(path);
                         },
                         (JSValue error) =>
                         {
                             Assert.Fail();
                         }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_write_ExistingFile()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);
            m_moduleBuilder.Call2("write", path, base64Content, bytes.Length,
                        (int result) =>
                        {
                            // Assert
                            var contents = File.ReadAllText(path);
                            Assert.AreEqual(hello + hello, contents);
                            // Cleanup
                            File.Delete(path);
                        },
                        (JSValue error) =>
                        {
                            Assert.Fail();
                        }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);


        }

        [TestMethod]
        public void RNFSManager_write_ExistingDirectory()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var hello = "Hello World";
            var base64Content = Convert.ToBase64String(Encoding.UTF8.GetBytes(hello));
            m_moduleBuilder.Call2("write", path, base64Content, 0,
            (int result) =>
            {
                Assert.Fail();
            },
            (JSValue error) =>
            {
                var message = error.Object["message"].String;
                Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");

                // Cleanup
                Directory.Delete(path);
            }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

        }

        [TestMethod]
        public void RNFSManager_write_NewFile_Seek()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            var bytes = Encoding.UTF8.GetBytes(hello);
            var base64Content = Convert.ToBase64String(bytes);

            m_moduleBuilder.Call2("write", path, base64Content, 4,
            (int result) =>
            {
                // Assert
                var actualBytes = File.ReadAllBytes(path);
                Assert.IsTrue(actualBytes.Skip(4).SequenceEqual(bytes));

                // Cleanup
                File.Delete(path);
            },
            (JSValue error) =>
            {
                Assert.Fail();
            }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);

        }

        [TestMethod]
        public void RNFSManager_write_Overwrite()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var earth = "Earth";
            var bytes = Encoding.UTF8.GetBytes(earth);
            var base64Content = Convert.ToBase64String(bytes);

            m_moduleBuilder.Call2("write", path, base64Content, 6,
            (int result) =>
            {
                // Assert
                var contents = File.ReadAllText(path);
                Assert.AreEqual("Hello Earth", contents);

                // Cleanup
                File.Delete(path);
            },
            (JSValue error) =>
            {
                Assert.Fail();
            }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_write_Fail()
        {
            // Run test
            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            m_moduleBuilder.Call2("write", path, "", 0,
           (int result) =>
           {
               Assert.Fail();
           },
           (JSValue error) =>
           {
               var message = error.Object["message"].String;
               Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");
           }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
        }

        [TestMethod]
        public void RNFSManager_exists()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test (true)
            m_moduleBuilder.Call2("exists", path,
           (bool result) =>
           {
               Assert.IsTrue(result);
               // Cleanup
               File.Delete(path);
           },
           (JSValue error) =>
           {
               Assert.Fail();
           });
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);

            // Run test (false)
            var fakePath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            // Run test (true)
            m_moduleBuilder.Call2("exists", path,
           (bool result) =>
           {
               Assert.IsFalse(result);

           },
           (JSValue error) =>
           {
               Assert.Fail();
           });
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);


        }

        [TestMethod]
        public void RNFSManager_exists_Directory()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test (true)
            m_moduleBuilder.Call2("exists", path,
           (bool result) =>
           {
               Assert.IsTrue(result);
               // Cleanup
               Directory.Delete(path);

           },
           (JSValue error) =>
           {
               Assert.Fail();
           });
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);


        }


        [TestMethod]
        public void RNFSManager_exists_InvalidPath()
        {

            m_moduleBuilder.Call2("exists", string.Concat(Path.GetInvalidPathChars()),
            (bool result) =>
            {
                Assert.IsFalse(result);
            },
            (JSValue error) =>
            {
                Assert.Fail();
            });
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }


        [TestMethod]
        public void RNFSManager_readFile()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);
            m_moduleBuilder.Call2("readFile", path,
                (string base64content) =>
                {
                    var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64content));
                    Assert.AreEqual(hello, contents);
                    // Cleanup
                    File.Delete(path);
                },
                (JSValue error) => Assert.Fail()
           ).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }


        [TestMethod]
        public void RNFSManager_readFile_NotExists()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("readFile", path, (int i) => Assert.Fail(),
                (JSValue error) =>
                {
                    Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");
                }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
        }


        [TestMethod]
        public void RNFSManager_readFile_InvalidPath()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());

            m_moduleBuilder.Call2("readFile", path, (int i) => Assert.Fail(),
                (JSValue error) =>
                {
                    Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");
                }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);
        }


        [TestMethod]
        public void RNFSManager_readFile_Directory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            m_moduleBuilder.Call2("readFile", path, (int i) => Assert.Fail(),
                 (JSValue error) =>
                 {
                     Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");
                 }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

            // Cleanup
            Directory.Delete(path);
        }


        [TestMethod]
        public void RNFSManager_read()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);
            string base64Content = "wrong";
            m_moduleBuilder.Call2("read", path, 5, 0,
                (string r) =>
                {
                    base64Content = r;
                },

               (JSValue error) =>
               {
                   Assert.Fail();
               }).Wait();

            var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual("Hello", contents);

            m_moduleBuilder.Call2("read", path, 5, 6,
                (string r) =>
                {
                    base64Content = r;
                },

               (JSValue error) =>
               {
                   Assert.Fail();
               }).Wait();

            contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual("World", contents);

            // Cleanup
            File.Delete(path);
        }



        [TestMethod]
        public void RNFSManager_read_NotExists()
        {
            // Initialize module
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("read", path, 0, 0,
            (string r) =>
            {
                Assert.Fail();
            },

           (JSValue error) =>
           {
               Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

           }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

        }



        [TestMethod]
        public void RNFSManager_read_InvalidPath()
        {
            m_moduleBuilder.Call2("read", "padPath", 0, 0,
            (string r) =>
            {
                Assert.Fail();
            },

           (JSValue error) =>
           {
               Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

           }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

        }


        [TestMethod]
        public void RNFSManager_read_Directory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);
            m_moduleBuilder.Call2("read", path, 10, 0,
            (string r) =>
            {
                Assert.Fail();
            },

            (JSValue error) =>
            {
                Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

            }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

            // Cleanup
            Directory.Delete(path);
        }


        [TestMethod]
        public void RNFSManager_read_MoreThanContent()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);
            var base64Content = "bad";
            m_moduleBuilder.Call2("read", path, 100, 0,
            (string r) =>
            {
                base64Content = r;
            },

            (JSValue error) =>
            {
                Assert.Fail();

            }).Wait();

            // Run test

            var contents = Encoding.UTF8.GetString(Convert.FromBase64String(base64Content));
            Assert.AreEqual(
                hello + string.Join("", Enumerable.Repeat('\0', 100 - hello.Length)),
                contents);

            // Cleanup
            File.Delete(path);
        }

        [TestMethod]
        public void RNFSManager_hash()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);
            string hash = "bad";

            // Run test
            m_moduleBuilder.Call2("hash", path, "sha256",
           (string r) =>
           {
               hash = r;
           },

           (JSValue error) =>
           {
               Assert.Fail();

           }).Wait();

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
        public void RNFSManager_hash_InvalidAlgorithm()
        {

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("hash", path, "foo",
          (string r) =>
          {
              Assert.Fail();
          },

          (JSValue error) =>
          {
              Assert.AreEqual(error.Object["message"].String, "Invalid hash algorithm");

          }).Wait();

        }


        [TestMethod]
        public void RNFSManager_hash_NotExists()
        {


            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("hash", path, "sha256",
             (string r) =>
             {
                 Assert.Fail();
             },

             (JSValue error) =>
             {
                 Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

             }).Wait();

        }


        [TestMethod]
        public void RNFSManger_hash_Directory()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            m_moduleBuilder.Call2("hash", path, "sha256",
            (string r) =>
            {
                Assert.Fail();
            },

            (JSValue error) =>
            {
                Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

            }).Wait();
            // Cleanup
            Directory.Delete(path);
        }

        [TestMethod]
        public void RNFSManager_moveFile()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("moveFile", path, newPath, new JSValue(),
            (int r) => { },
            (JSValue error) =>
            {
                Assert.Fail();

            }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);


            // Assert
            var contents = File.ReadAllText(newPath);
            Assert.AreEqual(hello, contents);
            Assert.IsFalse(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(newPath);
        }



        [TestMethod]
        public void RNFSManager_moveFile_NotExists()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("moveFile", path, newPath, new JSValue(),
            (int r) =>
            {
                Assert.Fail();
            },

            (JSValue error) =>
            {
                Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

            }).Wait();
        }

        [TestMethod]
        public void RNFSManager_copyFile()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("copyFile", path, newPath, new JSValue(),
            (int r) => { },
            (JSValue error) =>
            {
                Assert.Fail();

            }).Wait();


            // Assert
            var contents = File.ReadAllText(newPath);
            Assert.AreEqual(hello, contents);
            Assert.IsTrue(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(path);
            File.Delete(newPath);
        }


        [TestMethod]
        public void RNFSManager_copyFile_NotExists()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var newPath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("copyFile", path, newPath, new JSValue(),
                        (int r) =>
                        {
                            Assert.Fail();
                        },

                        (JSValue error) =>
                        {
                            Assert.AreEqual(error.Object["message"].String.Substring(0, 6), "ENOENT");

                        }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

        }



        [TestMethod]
        public void RNFSManager_readDir()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var path = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            m_moduleBuilder.Call2("readDir", dirpath,
                (JSValue v) =>
                {
                    var fileInfos = v.Array;
                    Assert.AreEqual(fileInfos.Count, 1);
                    var item = fileInfos[0];
                    var fileInfo = item.Object;
                    Assert.AreEqual(0, fileInfo["type"].Int64);
                    Assert.AreEqual(11, fileInfo["size"].Int64);
                    Assert.AreEqual(path, fileInfo["path"].String);
                    Assert.AreEqual(Path.GetFileName(path), fileInfo["name"].String);
                    Assert.AreEqual(
                        ConvertToUnixTimestamp(new FileInfo(path).LastWriteTimeUtc),
                        fileInfo["mtime"].Double);

                    // Cleanup
                    File.Delete(path);
                    Directory.Delete(dirpath);

                },
                (JSValue err) => { Assert.Fail(); }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);



        }



        [TestMethod]
        public void RNFSManager_readDir_Empty()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);

            // Run test
            m_moduleBuilder.Call2("readDir", dirpath,
               (JSValue v) =>
               {
                   var fileInfos = v.Array;
                   Assert.AreEqual(fileInfos.Count, 0);

                   Directory.Delete(dirpath);

               },
               (JSValue err) => { Assert.Fail(); }).Wait();

        }


        [TestMethod]
        public async Task RNFSManager_readDir_Multiple()
        {
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
            m_moduleBuilder.Call2("readDir", dirpath,
               (JSValue v) =>
               {
                   var fileInfos = v.Array;
                   Assert.AreEqual(fileInfos.Count, 2);

               },
               (JSValue err) => { Assert.Fail(); }).Wait();

            // Cleanup
            File.Delete(path1);
            File.Delete(path2);
            Directory.Delete(dirpath);
        }


        [TestMethod]
        public void RNFSManager_readDir_Subdirectory()
        {
            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(dirpath);
            Directory.CreateDirectory(subdirpath);

            // Run test
            m_moduleBuilder.Call2("readDir", dirpath,
              (JSValue v) =>
              {
                  var fileInfos = v.Array;
                  Assert.AreEqual(fileInfos.Count, 1);
                  var fileInfo = fileInfos[0].Object;

                  Assert.IsTrue(fileInfo.ContainsKey("type"));
                  Assert.AreEqual(1, fileInfo["type"].Int64);
                  Assert.AreEqual(0, fileInfo["size"].Int64);
                  Assert.AreEqual(subdirpath, fileInfo["path"].String);
                  Assert.AreEqual(subdirpath.Split(Path.DirectorySeparatorChar).Last(), fileInfo["name"].String);
                  Assert.AreEqual(
                      ConvertToUnixTimestamp(new DirectoryInfo(subdirpath).LastWriteTimeUtc),
                      fileInfo["mtime"].Double);


              },
              (JSValue err) => { Assert.Fail(); }).Wait();




            // Cleanup
            Directory.Delete(subdirpath);
            Directory.Delete(dirpath);
        }


        [TestMethod]
        public void RNFSManager_stat()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            m_moduleBuilder.Call2("stat", path,
             (JSValue v) =>
             {
                 var stat = v.Object;
                 Assert.AreEqual(0, stat["type"].Int64);
                 Assert.AreEqual(11, stat["size"].Int64);
                 Assert.AreEqual(
                     ConvertToUnixTimestamp(new FileInfo(path).CreationTimeUtc),
                     stat["ctime"].Double);
                 Assert.AreEqual(
                     ConvertToUnixTimestamp(new FileInfo(path).LastWriteTimeUtc),
                     stat["mtime"].Double);


             },
             (JSValue err) => { Assert.Fail(); }).Wait();

            // Cleanup
            File.Delete(path);
        }



        [TestMethod]
        public void RNFSManager_stat_NotExists()
        {


            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            // Run test
            m_moduleBuilder.Call2("stat", path,
             (JSValue v) =>
             Assert.Fail(),
             (JSValue err) =>
             {
                 Assert.AreEqual("File does not exist.", err.Object["message"]);
             }).Wait();

        }


        [TestMethod]
        public void RNFSManager_stat_Directory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test

            m_moduleBuilder.Call2("stat", path,
            (JSValue v) =>
            {
                var stat = v.Object;
                Assert.AreEqual(1, stat["type"].Int64);
                Assert.AreEqual(0, stat["size"].Int64);
                Assert.AreEqual(
                    ConvertToUnixTimestamp(new DirectoryInfo(path).CreationTimeUtc),
                    stat["ctime"].Double);
                Assert.AreEqual(
                    ConvertToUnixTimestamp(new DirectoryInfo(path).LastWriteTimeUtc),
                    stat["mtime"].Double);
            },
            (JSValue err) =>
            {
                Assert.Fail();
            }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);


            // Cleanup
            Directory.Delete(path);
        }


        [TestMethod]
        public void RNFSManager_unlink_Directory()
        {


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
            m_moduleBuilder.Call2("unlink", dirpath,
            (int v) =>
            {
                Assert.IsFalse(new DirectoryInfo(dirpath).Exists);
            },
            (JSValue err) =>
            {
                Assert.Fail();
            }).Wait();

            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);

        }



        [TestMethod]
        public void RNFSManager_unlink_File()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            m_moduleBuilder.Call2("unlink", path,
            (int v) =>
            {
                Assert.IsFalse(new FileInfo(path).Exists);
            },
            (JSValue err) =>
            {
                Assert.Fail();
            }).Wait();

        }


        [TestMethod]
        public void RNFSManager_unlink_Recursive()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var dirpath = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(dirpath, Guid.NewGuid().ToString());
            var path = Path.Combine(dirpath, Guid.NewGuid().ToString());
            Directory.CreateDirectory(subdirpath);
            var hello = "Hello World";
            File.WriteAllText(path, hello);

            // Run test
            m_moduleBuilder.Call2("unlink", dirpath,
            (int v) =>
            {

            },
            (JSValue err) =>
            {
                Assert.Fail();
            }).Wait();

            // Run test
            Assert.IsFalse(new FileInfo(path).Exists);
            Assert.IsFalse(new DirectoryInfo(subdirpath).Exists);
            Assert.IsFalse(new DirectoryInfo(dirpath).Exists);


        }



        [TestMethod]
        public void RNFSManager_unlink_NotExists()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("unlink", path,
            (int v) =>
            {
                Assert.Fail();
            },
            (JSValue err) =>
            {
                Assert.AreEqual("File does not exist.", err.Object["message"].String);
            }).Wait();
            Assert.IsTrue(m_moduleBuilder.IsRejectCallbackCalled);

        }


        [TestMethod]
        public void RNFSManager_mkdir()
        {

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("mkdir", path, new JSValue(),
           (int v) =>
           {

           },
           (JSValue err) =>
           {
               Assert.Fail();
           }).Wait();


            // Assert
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(path);
        }



        [TestMethod]
        public void RNFSManager_mkdir_Intermediate()
        {


            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var subdirpath = Path.Combine(path, Guid.NewGuid().ToString());
            m_moduleBuilder.Call2("mkdir", subdirpath, new JSValue(),
           (int v) =>
           {

           },
           (JSValue err) =>
           {
               Assert.Fail();
           }).Wait();


            // Assert
            Assert.IsTrue(new DirectoryInfo(subdirpath).Exists);
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(subdirpath);
            Directory.Delete(path);
        }


        [TestMethod]
        public void RNFSManager_mkdir_ExistingFile()
        {

            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            m_moduleBuilder.Call2("mkdir", path, new JSValue(),
           (int v) =>
           {
               Assert.Fail();
           },
           (JSValue err) =>
           {
               var message = err.Object["message"].String;
               Assert.AreEqual(message, $"Cannot create '{path}' because a file or directory with the same name already exists.", $"Message was {message}");
           }).Wait();

            // Cleanup
            File.Delete(path);
        }



        [TestMethod]
        public void RNFSManager_mkdir_ExistingDirectory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            m_moduleBuilder.Call2("mkdir", path, new JSValue(),
          (int v) =>
          {

          },
          (JSValue err) =>
          {
              Assert.Fail();
          }).Wait();

            // Assert
            Assert.IsTrue(new DirectoryInfo(path).Exists);

            // Cleanup
            Directory.Delete(path);
        }

        
        [TestMethod]
        public void RNFSManager_mkdir_Fail()
        {

            var path = $"{Windows.ApplicationModel.Package.Current.InstalledLocation.Path}\\badPath";
            // Run test
            m_moduleBuilder.Call2("mkdir", path, new JSValue(),
           (int v) =>
           {
               Assert.Fail();
           },
           (JSValue err) =>
           {
               var message = err.Object["message"].String;
               Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");
           }).Wait();
            
        }

        

        [TestMethod]
        public void RNFSManager_getFSInfo()
        {

            // Run test
            m_moduleBuilder.Call2("getFSInfo", (JSValue v) =>
            {
                Assert.IsNotNull(v.Object);
                Assert.IsTrue(v.Object.ContainsKey("freeSpace"));
                Assert.IsTrue(v.Object.ContainsKey("totalSpace"));
            },
            (JSValue e) => Assert.Fail()).Wait();
            Assert.IsTrue(m_moduleBuilder.IsResolveCallbackCalled);
        }

        

        [TestMethod]
        public void RNFSManager_touch()
        {
            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));
            m_moduleBuilder.Call2("touch", path, ConvertToUnixTimestamp(mtime),
                ConvertToUnixTimestamp(ctime), (string result) =>
                {
                    Assert.AreEqual(path, result);
                    var fileInfo = new FileInfo(path);
                    Assert.IsTrue(fileInfo.Exists);
                    Assert.AreEqual(ctime, fileInfo.CreationTime);
                    Assert.AreEqual(mtime, fileInfo.LastWriteTime);
                },
                (JSValue err) => Assert.Fail()).Wait();
 
            // Cleanup
            File.Delete(path);
        }

        

        [TestMethod]
        public async Task RNFSManager_touch_ExistingFile()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));

            m_moduleBuilder.Call2("touch", path, ConvertToUnixTimestamp(mtime),
               ConvertToUnixTimestamp(ctime), (string result) =>
               {
                   Assert.AreEqual(path, result);
                   var fileInfo = new FileInfo(path);
                   Assert.IsTrue(fileInfo.Exists);
                   Assert.AreEqual(ctime, fileInfo.CreationTime);
                   Assert.AreEqual(mtime, fileInfo.LastWriteTime);
               },
               (JSValue err) => Assert.Fail()).Wait();


            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        public async Task RNFSManager_touch_ExistingDirectory()
        {


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var ctime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddDays(-1)));
            var mtime = ConvertFromUnixTimestamp(ConvertToUnixTimestamp(DateTime.Now.AddHours(-1)));

            m_moduleBuilder.Call2("touch", path, ConvertToUnixTimestamp(mtime),
               ConvertToUnixTimestamp(ctime), (string result) =>
               {
                   Assert.Fail();
               },
               (JSValue err) => 
               {
                   var message = err.Object["message"].String;
                   Assert.AreEqual(message, $"Access to the path '{path}' is denied.", $"Message was {message}");
               }).Wait();
            // Cleanup
            Directory.Delete(path);
        }


        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile()
        {


            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            var eventCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                eventCount++;
                // TODO check
                 m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                 {


                 });
            });


            m_moduleBuilder.Call2("downloadFile", options,
                (JSValue result) =>
                {
                    var json = result.Object;
                    Assert.AreEqual(1, json["jobId"].Int64);
                    Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Int64);
                    Assert.AreEqual(200, json["statusCode"].Int64);
                },
                (JSValue err) =>
                {
                    Assert.Fail();
                }).Wait();

            Assert.AreEqual(eventCount, 1);
            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile_Progress()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/bytes/102400" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 10 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
                (JSValue result) =>
                {
                    var json = result.Object;
                    Assert.AreEqual(1, json["jobId"].Int64);
                    Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Int64);
                    Assert.AreEqual(200, json["statusCode"].Int64);
                },
                (JSValue err) =>
                {
                    Assert.Fail();
                }).Wait();


            Assert.IsTrue(progressCount >= 10);

            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile_Headers()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });


            // Run test
            var expectedHeaders = new Dictionary<string, JSValue>
            {
                { "Accept", "foo/bar" },
                { "ContentLength", 0 },
                { "X-Custom-Header", "qux" }
            };

            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/headers" },
                { "jobId", 1 },
                { "headers", new JSValue(expectedHeaders) },
                { "progressDivider", 100 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
               (JSValue result) =>
               {
                   
               },
               (JSValue err) =>
               {
                   Assert.Fail();
               }).Wait();

            var actualHeaders = (JObject)JObject.Parse(File.ReadAllText(path))["headers"];
            Assert.AreEqual(0, actualHeaders.Value<int>("ContentLength"));
            Assert.AreEqual("qux", actualHeaders.Value<string>("X-Custom-Header"));
            Assert.AreEqual("foo/bar", actualHeaders.Value<string>("Accept"));

            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile_ExistingFile()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            File.WriteAllText(path, "Hello World");

            // Run test
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
             (JSValue result) =>
             {
                 var json = result.Object;
                 Assert.AreEqual(1, json["jobId"].Int64);
                 Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Int64);
                 Assert.AreEqual(200, json["statusCode"].Int64);

             },
             (JSValue err) =>
             {
                 Assert.Fail();
             }).Wait();

            

            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile_ExistingDirectory()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });


            // Setup environment
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            Directory.CreateDirectory(path);

            // Run test
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/get" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
             (JSValue result) =>
             {
                 Assert.Fail();

             },
             (JSValue err) =>
             {
                 var msg = err.Object["message"];
                 Assert.AreEqual(msg, $"Access to the path '{path}' is denied.", $"Message was {msg}");
             }).Wait();


            // Cleanup
            Directory.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_downloadFile_Redirect()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/redirect/1" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
             (JSValue result) =>
             {
                 var json = result.Object;
                 Assert.AreEqual(1, json["jobId"].Int64);
                 Assert.AreEqual(new FileInfo(path).Length, json["bytesWritten"].Int64);
                 Assert.AreEqual(200, json["statusCode"].Int64);

             },
             (JSValue err) =>
             {
                 Assert.Fail();
             }).Wait();


            // Cleanup
            File.Delete(path);
        }

        
        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_stopDownload()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/delay/10" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            var promise = m_moduleBuilder.Call2("downloadFile", options,
            (JSValue result) =>
            {


            },
            (JSValue err) =>
            {
                var msg = err.Object["message"].String;
                Assert.AreEqual("A task was canceled.", msg, msg);
            });

            m_moduleBuilder.Call0("stopDownload", 1);
            promise.Wait();
          

            Assert.IsFalse(new FileInfo(path).Exists);
        }

        

        [TestMethod]
        [TestCategory("Network")]
        public void RNFSManager_stopDownload_AfterComplete()
        {
            int progressCount = 0;
            m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadBegin", (JSValue ev1) =>
            {
                // TODO check
                m_moduleBuilder.ExpectEvent("RCTDeviceEventEmitter", "DownloadProgress", (JSValue ev2) =>
                {
                    progressCount++;

                });
            });

            // Run test
            var tempFolder = ApplicationData.Current.TemporaryFolder.Path;
            var path = Path.Combine(tempFolder, Guid.NewGuid().ToString());
            var options = new Dictionary<string, JSValue>
            {
                { "toFile", path },
                { "fromUrl", "http://httpbin.org/delay/1" },
                { "jobId", 1 },
                { "headers", new JSValue() },
                { "progressDivider", 100 },
            };

            m_moduleBuilder.Call2("downloadFile", options,
           (JSValue result) =>
           {


           },
           (JSValue err) =>
           {

           }).Wait();

            m_moduleBuilder.Call0("stopDownload", 1);

            Assert.IsTrue(new FileInfo(path).Exists);

            // Cleanup
            File.Delete(path);
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

