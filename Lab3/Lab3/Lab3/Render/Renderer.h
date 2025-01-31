#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <chrono>
#include <vector>

#include "AbstractBuffer.h"
#include "Structs.h"

class Display32;
class VertexShader;
class PixelShader;
//template <typename T>
//class AbstractBuffer<T>;



struct ConstantDataVertexShader;
class Drawable;
class Game;

class Renderer
{
public:
    Renderer(Display32& Display);
    ~Renderer();
    Microsoft::WRL::ComPtr<ID3D11Device> device() {return device_;}
    ID3D11DeviceContext* Context() {return context_;}
    void Render();
    VertexShader* vertex_shader() {return vertex_shader_;}
    PixelShader* pixel_shader() {return pixel_shader_;}
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    Display32& Display_;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv;
    VertexShader* vertex_shader_;
    PixelShader* pixel_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
    
};
