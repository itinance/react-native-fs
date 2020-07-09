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
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "2b || !2b\n\nby Bill Shakey\n"
                ); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Failed to read file."); }),
                "c:/Github/toRead.txt"));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
        }

        TEST_METHOD(TestMethodCall_readfileSuccessful2) {
            Mso::FutureWait(m_builderMock.Call2(
                L"readFile",
                std::function<void(std::string)>([](std::string value) noexcept { TestCheck(value == "2b || !2b\n\nby Bill Shakey\n"
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
            unlink() tests
        */

        TEST_METHOD(TestMethodCall_unlinkSuccessful) {
            std::string toCompare("");
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