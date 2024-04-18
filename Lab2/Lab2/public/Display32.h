#pragma once
#include "minwindef.h"
#include <winuser.h>
#include <iostream>
#include "windef.h"
#include <wingdi.h>

class Display32
{
public:
    Display32(LPCWSTR applicationName, int screenWidth, int screenHeight);
    void Destroy();
    HWND hWnd;
    int screenWidth_;
    int screenHeight_;
};
