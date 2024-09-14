#pragma once
#include "ConstantBuffer.h"

class Renderer;
class GBufferShader;

class LightingStageShader
{
public:
    LightingStageShader(Renderer* renderer, GBufferShader* GBuffer, Display32* display_);
    void Activate();
    void ProvideLightData(LightComponent* light);
    void ProvideMeshData(Mesh* mesh);
private:
    GBufferShader* GBuffer_;
    Renderer* renderer_;
    //ConstantBuffer<MaterialStructCB>* MaterialCB;
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    ConstantBuffer<ScrenToViewStructCB>* ScreenToViewCB;
    ConstantBuffer<LightDataStruct>* LightDataCB;
    //ConstantBuffer<uint16_t>* LightIndexCB;
    DirectX::SimpleMath::Vector2 ScreenDimensions;

    VertexShader* vertex_shader_;
    PixelShader* pixel_shader_;
    ID3D11InputLayout* layout;
    D3D11_VIEWPORT viewport;
    ID3D11Texture2D* DepthStencilTexture;
    ID3D11DepthStencilView* DSV;
    ID3D11RasterizerState* rastStateUP;
    ID3D11RasterizerState* rastStateSP;
    ID3D11DepthStencilState* DepthStencilStateUP;
    ID3D11DepthStencilState* DepthStencilStateSP;
    ID3D11BlendState* BlendState;
};
