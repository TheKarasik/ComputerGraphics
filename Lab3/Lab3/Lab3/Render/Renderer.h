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

//template <typename T>
//class AbstractBuffer<T>;

class VertexShader;
class PixelShader;

struct ConstantDataVertexShader;
class GameComponent;
class Game;

class Renderer
{
public:
    Renderer(Display32& Display);
    Microsoft::WRL::ComPtr<ID3D11Device> device() {return device_;}
    ID3D11DeviceContext* Context() {return context_;}
    void Render(std::vector<GameComponent*> visual_objects);
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    Display32& Display_;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv;
    VertexShader* vertex_shader;
    PixelShader* pixel_shader;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
    
};
