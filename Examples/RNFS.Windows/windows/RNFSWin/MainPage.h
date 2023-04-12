#pragma once
#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>


namespace winrt::RNFSWin::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::RNFSWin::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}


