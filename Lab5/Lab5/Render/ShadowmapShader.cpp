#include "ShadowmapShader.h"

#include <d3d11.h>

#include "PixelShader.h"
#include "VertexShader.h"

ShadowmapShader::ShadowmapShader(Renderer* renderer) : renderer_(renderer)
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };
    
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    
    renderer_->device()->CreateInputLayout(
        inputElements,
        1,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    
    /*CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    renderer_->device()->CreateRasterizerState(&rastDesc, &rastState);*/
}

void ShadowmapShader::Activate()
{
    
}
