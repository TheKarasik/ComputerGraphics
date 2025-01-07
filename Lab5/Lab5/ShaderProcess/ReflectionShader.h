#pragma once
#include <SimpleMath.h>

#include "ConstantBuffer.h"

class FileTexture;
struct CascadeData;
class Renderer;

class ReflectionShader
{
public:
    ReflectionShader(Renderer* renderer);
    //void Activate(DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Vector4 clipPlane);
    void Activate(WaterRenderType RenderType);
    void ProvideMeshData(Mesh* mesh);
    void SetWater(Water* water) {water_ = water;}
    //void SetNormalTexture(FileTexture* normal_texture) {normal_texture_ = normal_texture;}
    ID3D11ShaderResourceView** SRVReflection() {return &SRVReflection_;}
private:
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    ConstantBuffer<DirectX::SimpleMath::Vector4>* constant_buffer_clip_plane;
    ConstantBuffer<DirectX::SimpleMath::Vector4>* constant_buffer_directional_light;
    LightComponent* light_component_;
    ID3D11Texture2D* RTTextureReflection;
    ID3D11Texture2D* DSTextureReflection;
    ID3D11DepthStencilView* DSVReflection;
    ID3D11RenderTargetView* RTVReflection;
    ID3D11ShaderResourceView* SRVReflection_;
    ID3D11SamplerState* SamplerState_;
    Renderer* renderer_;
    D3D11_VIEWPORT viewport_shadowmap;
    VertexShader* vertex_shader_;
    PixelShader* pixel_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
    FileTexture* normal_texture_;
    Water* water_;
    ConstantBufferTransformMatricies* cbtm;
};
