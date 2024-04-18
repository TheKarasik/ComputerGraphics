#pragma once
#include "Display32.h"
#include "InputDevice.h"
#include "d3dcommon.h"
#include <wrl.h>
#include <d3d11.h>
#include "d3dcompiler.h"
#include"GoldenRectangleComponent.h"
#include <chrono>

#define GOLDEN_RATIO 1.618
#define MS_PER_UPDATE 16

class TriangleComponent;
class GoldenRectangleComponent;

class Game
{
public:
    void Initialize();
    Microsoft::WRL::ComPtr<ID3D11Device> device() {return device_;}
    InputDevice* Input() {return Input_;}
    Display32* Display() {return Display_;}
    ID3D11DeviceContext* Context() {context_;}
    void GameLoop();
private:
    void SetUpRender();
    void Render();
    void Update();
    void ProcessInput();
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    ID3D11DeviceContext* context_;
    InputDevice* Input_;
    Display32* Display_;
    //std::vector<TriangleComponent*> triangles;
    bool IsEscapePressed = false;
    GoldenRectangleComponent* GRectangle;
    float lag;
    ID3D11RasterizerState* rastState;
    ID3D11InputLayout* layout;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv;
};
