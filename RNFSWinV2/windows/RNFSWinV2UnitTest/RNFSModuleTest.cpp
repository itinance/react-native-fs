#include "pch.h"
#include "ReactModuleBuilderMock.h"

#include <sstream>
#include "Point.h"
#include "future/futureWait.h"
#include "RNFSManager.h"

namespace ReactNativeTests {

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
                "c:/Github/Hello", React::JSValueObject{}));
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
                "c:/Github/Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_existsUnseccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"exists",
                std::function<void(bool)>([](bool result) noexcept { TestCheck(!result); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to check if file or directory exists."); }),
                "c:/Github/Hello/World"));
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
                "c:/Github/toRead.txt"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readfileSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh"
                ); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                "c:/Github/toRead"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readfileUnsuccessful) {
            std::string toCompare("");
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                "c:/Github/Hello"));
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
                "c:/Github/TestWrite.txt", "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh", React::JSValueObject{}));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_writefileUnsuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"writeFile",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                "c:/Github/TestWrite/Testwrite.txt", "MmIgfHwgITJiIGJ5IEJpbGwgU2hha2V5DQoNCg0KYWFh", React::JSValueObject{}));
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
                "C:\\Github\\wait"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readDirSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readDir",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read directory."); }),
                "C:/Github/Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readDirUnsuccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readDir",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read directory."); }),
                "C:/Github/Hello/World/Toast/Bro"));
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
                "C:/Github/toWriteTo.txt", "YWFh", 0));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_writeToNegativeIndex) {
            Mso::FutureWait(m_builderMock.Call2(
                L"write",
                std::function<void(React::JSValueObject&)>([](React::JSValueObject&) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to write to file."); }),
                "C:/Github/toWriteTo.txt", "YmJiCg==", -1));
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
                "c:/Github/TestWrite.txt", 1, 1));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "fHwgITJiIGJ5IEJpbGwgU2hha2U="); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                "c:/Github/toRead.txt", 20, 3));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful1) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                "c:/Github/toRead.txt", 9999, -1));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                "c:/Github/toRead.txt", -1, 9999));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful3) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                "c:/Github/toRead.txt", 9999, 9999));
            TestCheck(m_builderMock.IsRejectCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readUnsuccessful4) {
            Mso::FutureWait(m_builderMock.Call2(
                L"read",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read from file."); }),
                "c:/Github/toRead.txt", -9999, -9999));
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
                "c:/Github/TestWrite.txt", 1593561600, 1593561600));
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
                "c:/Github/toMove.rtf"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
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
                "c:/Github/Hello"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_unlinkUnseccessful) {
            Mso::FutureWait(m_builderMock.Call2(
                L"unlink",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to unlink."); }),
                "c:/Github/Hello"));
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