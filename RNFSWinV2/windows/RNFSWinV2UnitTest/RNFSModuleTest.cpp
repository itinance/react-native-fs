#include "pch.h"
#include "ReactModuleBuilderMock.h"

#include <sstream>
#include "Point.h"
#include "future/futureWait.h"
#include "RNFSManager.h"

namespace ReactNativeTests {

    std::string testLocation = "C:/react-native-fs-tests\\";

    TEST_CLASS(RNFSManagerTest) {
        React::ReactModuleBuilderMock m_builderMock{};
        React::IReactModuleBuilder m_moduleBuilder;
        Windows::Foundation::IInspectable m_moduleObject{ nullptr };
        RNFSManager* m_module;

        RNFSManagerTest() {
            m_moduleBuilder = winrt::make<React::ReactModuleBuilderImpl>(m_builderMock);
            auto provider = React::MakeModuleProvider<RNFSManager>();
            m_moduleObject = m_builderMock.CreateModule(provider, m_moduleBuilder);
            m_module = React::ReactNonAbiValue<RNFSManager>::GetPtrUnsafe(m_moduleObject);
        }

        /*
            mkdir() tests
        */

        TEST_METHOD(TestMethodCall_mkdirSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"mkdir",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to create directory."); }),
                testLocation + "Hello", React::JSValueObject{}));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_mkdirUnseccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"mkdir",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to create directory."); }),
                "", React::JSValueObject{}));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            exists() tests
        */
        TEST_METHOD(TestMethodCall_existsSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"exists",
                std::function<void(bool)>([](bool result) noexcept { TestCheck(result); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to check if file or directory exists."); }),
                testLocation + "Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_existsUnseccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"exists",
                std::function<void(bool)>([](bool result) noexcept { TestCheck(!result); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to check if file or directory exists."); }),
                testLocation + "Hello/World"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }


        /*
            readfile() tests
        */
        TEST_METHOD(TestMethodCall_readfileSuccessful1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh"
                ); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                testLocation + "toRead.txt"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readfileSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh"
                ); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                testLocation + "toRead"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readfileUnsuccessful) {
            std::string toCompare("");
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                testLocation + "Hello"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            writefile() tests
        */
        TEST_METHOD(TestMethodCall_writefileSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"writeFile",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                testLocation + "TestWrite.txt", "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh", React::JSValueObject{}));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_writefileUnsuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"writeFile",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                testLocation + "TestWrite/Testwrite.txt", "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh", React::JSValueObject{}));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            readDir() tests
        */
        TEST_METHOD(TestMethodCall_readDirSuccessful1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readDir",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read directory."); }),
                testLocation + "wait"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readDirSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readDir",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read directory."); }),
                testLocation + "Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readDirUnsuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readDir",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read directory."); }),
                testLocation + "Hello/World/Toast/Bro"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            write() tests
        */
        TEST_METHOD(TestMethodCall_writeToBeginning) {
            Mso::FutureWait(m_builderMock.Call2(
                L"write",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                testLocation + "toWriteTo.txt", "YWFh", 0));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_writeToNegativeIndex) {
            Mso::FutureWait(m_builderMock.Call2(
                L"write",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                testLocation + "toWriteTo.txt", "YmJiCg==", -1));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        /*
            read() tests
        */
        TEST_METHOD(TestMethodCall_readSuccessful1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "Yg=="); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "TestWrite.txt", 1, 1));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "fHwgITJiIGJ5IEJpbGwgU2hha2U="); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "toRead.txt", 20, 3));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "toRead.txt", 9999, -1));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "toRead.txt", -1, 9999));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful3) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "toRead.txt", 9999, 9999));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful4) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                testLocation + "toRead.txt", -9999, -9999));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            moveFile() tests
        */


        /*
            copyFile() tests
        */


        /*
            touch() tests
        */
        TEST_METHOD(TestMethodCall_touchSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"touch",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to touch file."); }),
                testLocation + "TestWrite.txt", 1593561600, 1593561600));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        /*
            getFSInfo() tests
        */
        //TEST_METHOD(TestMethodCall_getFSInfoSuccessful) {
        //    Mso::FutureWait(m_builderMock.Call2(
        //        L"getFSInfo",
        //        std::function<void()>([]() noexcept { TestCheck(true); }),
        //        std::function<void(React::JSValue const&)>(
        //            [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to retrieve file system info."); })));
        //    TestCheck(m_builderMock.IsResolveCallbackCalled());
        //}

        /*
            stat() tests
        */
        TEST_METHOD(TestMethodCall_statSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"stat",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to retrieve file info."); }),
                testLocation + "toMove.rtf"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        /*
            hash() tests
        */
        TEST_METHOD(TestMethodCall_hashSuccessful_MD5) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "0e988e0e8dec56e3bb331e110109cc24"); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation + "toHash.txt", "md5"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashSuccessful_SHA1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "cb6e9c8e23671c8406179b9e50e8d55d79bb6d1c"); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation + "toHash.txt", "sha1"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashSuccessful_SHA256) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "ba0ed317bfab6eb1f3b59b9ea26efeb5a2afd565f7632fb6ec3fafcd3ee05336"); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation + "toHash.txt", "sha256"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashSuccessful_SHA384) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "ca729e5416a95d4acaf31d398824e782d085283a1fd776a188bb6340904f2205cf5c1e6849e7acfbb7271b2e8135d96f"); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation + "toHash.txt", "sha384"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashSuccessful_SHA512) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "0bd4e1ac2101124ca5efa102c2be200a2573f627c5bc926d0105c0a98fb24064ebed206b47deca5c4d0005c8796fbdc5e256f5f75f603fedc5bf5d4e3d40e79f"); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation + "toHash.txt", "sha512"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashUnsuccessful_SHA224) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "WinRT does not offer sha224 encryption."); }),
                testLocation + "toHash.txt", "sha224"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashUnsuccessful_NonexistantHash) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to find hash algorithm."); }),
                testLocation + "toHash.txt", "squirrels"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_hashUnsuccessful_InvalidFile) {
            Mso::FutureWait(m_builderMock.Call2(
                L"hash",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to get checksum from file."); }),
                testLocation, "sha256"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        /*
            unlink() tests
        */
        TEST_METHOD(TestMethodCall_unlinkSuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"unlink",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to unlink."); }),
                testLocation + "Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_unlinkUnseccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"unlink",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to unlink."); }),
                testLocation + "Hello"));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        //TEST_METHOD(TestMethodCall_NegateAsyncPromiseError) {
        //    Mso::FutureWait(m_builderMock.Call2(
        //        L"NegateAsyncPromise",
        //        std::function<void(int)>([](int result) noexcept { TestCheck(result == -5); }),
        //        std::function<void(React::JSValue const&)>(
        //            [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Already negative"); }),
        //        -5));
        //    TestCheck(m_builderMock.IsRejectCallbackCalled());
        //}
    };
}