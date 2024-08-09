#include "MainShader.h"

#include <d3d11.h>
#include "LightComponent.h"
#include "Renderer.h"
#include "PixelShader.h"
#include "VertexShader.h"

MainShader::MainShader(Renderer* renderer) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
    texture_constant_buffer_ = new ConstantBuffer<MaterialProperties>(renderer_);
    constant_buffer_light = new ConstantBuffer<LightDataStruct>(renderer_);
    
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        /*D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}*/
        D3D11_INPUT_ELEMENT_DESC{
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC{
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };
    
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyAdvancedShader.hlsl");
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyAdvancedShader.hlsl");
    
    renderer_->device()->CreateInputLayout(
        inputElements,
        3,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    renderer_->device()->CreateRasterizerState(&rastDesc, &rastState);
}

void MainShader::Activate()
{

    //Рендер сцены для игрока
    renderer_->Context()->ClearState();
   
    renderer_->Context()->RSSetState(rastState);
    
    /*for (auto co : Updatable::changing_objects) co->update();
    
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display_.screenWidth_);
    viewport.Height = static_cast<float>(Display_.screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;*/

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);
    
    renderer_->Context()->PSSetShaderResources(1,1,&light_->SRVShadow);
    renderer_->Context()->PSSetSamplers(1,1,&light_->SamplerState);

    /*renderer_->Context()->OMSetRenderTargets(1, &rtv_main, dsv);
    renderer_->Context()->OMSetDepthStencilState(depth_stencil_state_, 1);

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderer_->Context()->ClearRenderTargetView(rtv_main, color);
    renderer_->Context()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    renderer_->Context()->RSSetViewports(1, &viewport);*/
}

void MainShader::UpdateTransformCB(ConstantBufferTransformMatricies CBSubresource)
{
    constant_buffer_transform->UpdateBuffer(&CBSubresource);
}
