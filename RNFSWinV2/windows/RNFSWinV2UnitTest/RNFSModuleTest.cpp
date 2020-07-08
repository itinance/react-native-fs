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

        TEST_METHOD(TestMethodCall_NegateAsyncPromise) {
            Mso::FutureWait(m_builderMock.Call2(
                L"mkdir",
                std::function<void()>([]() noexcept { TestCheck(true); }),
                std::function<void(React::JSValue const&)>(
                    [](React::JSValue const& error) noexcept { TestCheck(error["message"] == "Already negative"); }),
                "aaa", React::JSValueObject{}));
            TestCheck(m_builderMock.IsResolveCallbackCalled());
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