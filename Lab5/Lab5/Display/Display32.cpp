﻿#include "Display32.h"

#include "InputDevice.h"



LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    switch (umessage)
    {
    case WM_INPUT:
    {
            UINT dwSize;

            GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam),
                RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if (lpb == NULL) 
            {
                return 0;
            } 

            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam),
                RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
                OutputDebugString (TEXT("GetRawInputData does not return correct size !\n")); 

            RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

            if (raw->header.dwType == RIM_TYPEKEYBOARD) 
            {
                InputDevice::singleton->OnKeyDown({
                    raw->data.keyboard.MakeCode,
                    raw->data.keyboard.Flags,
                    raw->data.keyboard.VKey,
                    raw->data.keyboard.Message
                });
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                //printf(" Mouse: X=%04d Y:%04d \n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                InputDevice::singleton->OnMouseMove({
                    raw->data.mouse.usFlags,
                    raw->data.mouse.usButtonFlags,
                    static_cast<int>(raw->data.mouse.ulExtraInformation),
                    static_cast<int>(raw->data.mouse.ulRawButtons),
                    static_cast<short>(raw->data.mouse.usButtonData),
                    raw->data.mouse.lLastX,
                    raw->data.mouse.lLastY
                    });
            }

            delete[] lpb; 
            return 0;
    }
    case WM_KEYDOWN:
        {
            // If a key is pressed send it to the input object so it can record that state.
            //std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

            if (static_cast<unsigned int>(wparam) == 27)
            {
                PostQuitMessage(0);
            }
            return 0;
        }
    default:
        {
            return DefWindowProc(hwnd, umessage, wparam, lparam);
        }
    }
}

Display32::Display32(LPCWSTR applicationName, int screenWidth, int screenHeight, InputDevice* input_device) :
screenWidth_(screenWidth), screenHeight_(screenHeight)
{
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    
    WNDCLASSEX wc;

    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
    wc.hIconSm = wc.hIcon;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = applicationName;
    wc.cbSize = sizeof(WNDCLASSEX);

    // Register the window class.
    RegisterClassEx(&wc);

    RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

    auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
    auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
        dwStyle,
        posX, posY,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, SW_SHOW);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);

    ShowCursor(true);
}

void Display32::Destroy()
{
    DestroyWindow(hWnd);
}
