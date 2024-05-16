#pragma once
#include "Display32.h"
#include "InputDevice.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <chrono>

#include "Controllable.h"
#include "Keys.h"
#include "Renderer.h"
#include "RectangleObject.h"

#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxguid.lib")

#define S_PER_UPDATE 0.016

//class TriangleComponent;
//class GoldenRectangleComponent;

class RectangleObject;

class Game
{
public:
    Game();
    InputDevice* Input() {return input_;}
    Display32* Display() {return display_;}
    void GameLoop();
private:
    bool single_player = false;
    void Render();
    void Update();
    void ProcessInput();
    void CleanUp();
    Renderer* renderer_;
    RectangleObject* ball;
    RectangleObject* left_platform;
    RectangleObject* right_platform;
    InputDevice* input_;
    Display32* display_;
    std::chrono::time_point<std::chrono::steady_clock> start;
    bool IsEscapePressed = false;
    //GoldenRectangleComponent* GRectangle;
    float lag;
    float elapsed;
    MSG msg = {};
};
