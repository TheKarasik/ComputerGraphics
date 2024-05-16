#pragma once
//#include "minwindef.h"
//#include <winuser.h>
#include <iostream>

#include "Delegates.h"
#include "InputDevice.h"
#include "windows.h"
//#include <wingdi.h>

class InputDevice;

class Display32
{
public:
    Display32(LPCWSTR applicationName, int screenWidth, int screenHeight, InputDevice* input_device);
    void Destroy();
    HWND hWnd;
    int screenWidth_;
    int screenHeight_;
};
