#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <chrono>
#include <vector>

#include "AbstractBuffer.h"
#include "CameraObject.h"
#include "Structs.h"

class Display32;
class VertexShader;
class PixelShader;
//template <typename T>
//class AbstractBuffer<T>;



//struct TransformCBStruct;
class Drawable;
class Game;

class Renderer
{
public:
    Renderer(Display32& Display, CameraObject* camera);
    ~Renderer();
    Microsoft::WRL::ComPtr<ID3D11Device> device() {return device_;}
    ID3D11DeviceContext* Context() {return context_;}
    void Render();
    VertexShader* vertex_shader() {return vertex_shader_;}
    PixelShader* pixel_shader() {return pixel_shader_;}
    CameraObject* camera();
    void SetLight(LightComponent* light) {light_ = light;}
    void SetMiniMapCamera (MiniMapCamera* mini_map_camera) {mini_map_camera_ = mini_map_camera;}
    bool IsRenderingMain = true;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    Display32& Display_;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* rtv_main;
    ID3D11RenderTargetView* rtv_mini_map;
    VertexShader* vertex_shader_;
    PixelShader* pixel_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
    CameraObject* camera_;
    MiniMapCamera* mini_map_camera_;
    ID3D11DepthStencilView* dsv;
    ID3D11ShaderResourceView* srv_mini_map;
    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11Texture2D* mini_map_buffer_;
    ID3D11DepthStencilState* depth_stencil_state_;
    LightComponent* light_;
};
