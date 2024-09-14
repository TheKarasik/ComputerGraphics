#pragma once
#include <d3d11.h>

#include "ConstantBuffer.h"

class GeometryShader;
struct ConstantBufferTransformMatricies;
class PixelShader;
class VertexShader;
class Renderer;

class CascadeShadowmapShader
{
public:
    CascadeShadowmapShader(Renderer* renderer);
    void Activate();
    void ProvideMeshData(Mesh* mesh);
    void ProvideLightData(LightComponent* light);
    ID3D11ShaderResourceView** SRVShadow() {return &SRVShadow_;}
    ID3D11SamplerState** SamplerState() {return  &SamplerState_;}
private:
    ConstantBuffer<DirectX::SimpleMath::Matrix>* constant_buffer_transform;
    ConstantBuffer<CascadeData>* constant_buffer_cascade;
    LightComponent* light_component_;
    ID3D11Texture2D* RTTextureShadow;
    ID3D11Texture2D* DSTextureShadowArray;
    ID3D11DepthStencilView* DSVShadow;
    ID3D11RenderTargetView* RTVShadow;
    ID3D11ShaderResourceView* SRVShadow_;
    ID3D11SamplerState* SamplerState_;
    Renderer* renderer_;
    D3D11_VIEWPORT viewport_shadowmap;
    VertexShader* vertex_shader_;
    GeometryShader* geometry_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
};
