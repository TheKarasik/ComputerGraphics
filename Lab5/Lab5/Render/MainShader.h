#pragma once
#include <d3d11.h>

#include "ConstantBuffer.h"
#include "Structs.h"


class LightComponent;
class VertexShader;
class PixelShader;
class Renderer;

class MainShader
{
public:
    MainShader(Renderer* renderer, IDXGISwapChain* swap_chain, Microsoft::WRL::ComPtr<ID3D11Device> device, Display32 display32);
    void Activate();
    void ProvideMeshData(Mesh* mesh);
    void ProvideLightData(LightComponent* light);
    VertexShader* vertex_shader() {return vertex_shader_;}
    PixelShader* pixel_shader() {return pixel_shader_;}

private:
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    ConstantBuffer<MaterialProperties>* constant_buffer_texture;
    ConstantBuffer<LightDataStruct>* constant_buffer_light;
    LightComponent* light;
    ID3D11RenderTargetView* rtv_main;
    ID3D11DepthStencilView* dsv;
    ID3D11DepthStencilState* depth_stencil_state_;
    Renderer* renderer_;
    D3D11_VIEWPORT viewport;
    PixelShader* pixel_shader_;
    VertexShader* vertex_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
};
