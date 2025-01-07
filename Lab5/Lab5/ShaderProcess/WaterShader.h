#pragma once
#include "ConstantBuffer.h"

class WaterShader
{
public:
    WaterShader(Renderer* renderer);
    void ProvideWater(Water* water) {water_ = water;}
    void Activate();
private:
    long namecheck1 = 0xBAADF00D;
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    ConstantBuffer<WaterAdditionalDataStruct>* constant_buffer_water_additional_data;
    ConstantBuffer<WaterBuffer>* constant_buffer_water_buffer;
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
    ID3D11BlendState* BS_opaque;
    ID3D11BlendState* BS_not_premultiplied;
    ID3D11BlendState* BS_additive;
    ID3D11DepthStencilState* DSS_depth_default;
    ID3D11DepthStencilState* DSS_depth_read;
    ID3D11DepthStencilState* DSS_none;
    Water* water_;
    long namecheck2 = 0xBAADF00D;
    int blendmode = 0;
};
