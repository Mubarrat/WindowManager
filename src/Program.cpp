#include <iostream>
#include <Windows.h>
#include "Window.h"
#include "MainWindow.h"

#define WIN32_LEAN_AND_MEAN

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    MainWindow window;
    if (window.Initialize(hInstance, nCmdShow))
    {
        MSG msg = {};
        while (window.ProcessMessages(&msg))
        {
            // Your application logic goes here
        }
        return (int)msg.wParam;
    }

    return 0;
}
