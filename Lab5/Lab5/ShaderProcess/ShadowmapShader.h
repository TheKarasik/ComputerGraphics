#pragma once
#include <d3d11.h>

#include "ConstantBuffer.h"

struct ConstantBufferTransformMatricies;
class PixelShader;
class VertexShader;
class Renderer;

class ShadowmapShader
{
public:
    ShadowmapShader(Renderer* renderer);
    void Activate();
    void ProvideMeshData(Mesh* mesh);
    void ProvideLightData(LightComponent* light);
    ID3D11ShaderResourceView** SRVShadow() {return &SRVShadow_;}
    ID3D11SamplerState** SamplerState() {return  &SamplerState_;}
private:
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    LightComponent* light_component_;
    ID3D11Texture2D* RTTextureShadow;
    ID3D11Texture2D* DSTextureShadow;
    ID3D11DepthStencilView* DSVShadow;
    ID3D11RenderTargetView* RTVShadow;
    ID3D11ShaderResourceView* SRVShadow_;
    ID3D11SamplerState* SamplerState_;
    Renderer* renderer_;
    D3D11_VIEWPORT viewport_shadowmap;
    PixelShader* pixel_shader_;
    VertexShader* vertex_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
};
