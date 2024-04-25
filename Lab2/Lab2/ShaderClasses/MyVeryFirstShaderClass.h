#pragma once
#include "d3dcommon.h"
#include "d3dcompiler.h"
#include "d3d11.h"
#include "winnt.h"
#include "iostream"
#include <wrl.h>

class MyVeryFirstShaderClass
{
public:
    MyVeryFirstShaderClass(HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> device);
};
