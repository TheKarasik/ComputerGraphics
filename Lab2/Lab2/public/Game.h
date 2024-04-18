#pragma once
#include "Display32.h"
#include "InputDevice.h"
#include "d3dcommon.h"
#include <wrl.h>
#include <d3d11.h>
#include "d3dcompiler.h"
#include"GoldenRectangleComponent.h"

#define GOLDEN_RATIO 1.618

class TriangleComponent;

class Game
{
public:
    void Initialize();
private:
    void SetUpRender();
    InputDevice* Input;
    Display32* Display;
    std::vector<TriangleComponent*> triangles;
};
