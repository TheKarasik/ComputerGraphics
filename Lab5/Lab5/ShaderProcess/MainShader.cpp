#include "MainShader.h"

#include <d3d11.h>

#include "Display32.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "Renderer.h"
#include "PixelShader.h"
#include "ShadowmapShader.h"
#include "FileTexture.h"
#include "VertexShader.h"

MainShader::MainShader(Renderer* renderer, IDXGISwapChain* swap_chain, Microsoft::WRL::ComPtr<ID3D11Device> device, Display32 display32) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
    constant_buffer_texture = new ConstantBuffer<MaterialStructCB>(renderer_);
    constant_buffer_light = new ConstantBuffer<LightDataStruct>(renderer_);

    //Делаем BackBuffer

    //Делаем Z Buffer
    ID3D11Texture2D* depth_stencil_buffer;
    D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
    ZeroMemory( &depth_stencil_buffer_desc, sizeof(D3D11_TEXTURE2D_DESC) );
 
    depth_stencil_buffer_desc.ArraySize = 1;
    depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_buffer_desc.CPUAccessFlags = 0; // No CPU access required.
    depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_buffer_desc.Width = display32.screenWidth_;
    depth_stencil_buffer_desc.Height = display32.screenHeight_;
    depth_stencil_buffer_desc.MipLevels = 1;
    depth_stencil_buffer_desc.SampleDesc.Count = 1;
    depth_stencil_buffer_desc.SampleDesc.Quality = 0;
    depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    
    device->CreateTexture2D( &depth_stencil_buffer_desc, nullptr, &depth_stencil_buffer);
    device->CreateDepthStencilView(depth_stencil_buffer, nullptr, &dsv);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc;
    ZeroMemory( &depth_stencil_state_desc, sizeof(D3D11_DEPTH_STENCIL_DESC) );
 
    depth_stencil_state_desc.DepthEnable = TRUE;
    depth_stencil_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_state_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stencil_state_desc.StencilEnable = FALSE;
 
    device->CreateDepthStencilState( &depth_stencil_state_desc, &depth_stencil_state_);
    
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

    viewport = {};
    viewport.Width = static_cast<float>(display32.screenWidth_);
    viewport.Height = static_cast<float>(display32.screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;
}

void MainShader::Activate()
{

    //Рендер сцены для игрока
    renderer_->Context()->ClearState();
   
    renderer_->Context()->RSSetState(rastState);
    
    //for (auto co : Updatable::changing_objects) co->update();  !!!ВОЗМОЖНО ПРИДЁТСЯ РАСКОММЕНТИРОВАТЬ!!!

    //renderer_->Context()

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    renderer_->Context()->OMSetRenderTargets(1, renderer_->RTVMain(), dsv);
    renderer_->Context()->OMSetDepthStencilState(depth_stencil_state_, 1);

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderer_->Context()->ClearRenderTargetView(*renderer_->RTVMain(), color);
    renderer_->Context()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    renderer_->Context()->RSSetViewports(1, &viewport);

    renderer_->DrawEverything();
}

void MainShader::ProvideMeshData(Mesh* mesh)
{
    constant_buffer_transform->UpdateBuffer(mesh->transform_matricies_buffer_data);
    
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());
    renderer_->Context()->PSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());

    constant_buffer_texture->UpdateBuffer(&mesh->texture()->MatProp);
    
    renderer_->Context()->PSSetConstantBuffers(1,1,constant_buffer_texture->p_buffer());

    renderer_->Context()->PSSetShaderResources(0,1,mesh->texture()->srv());
    renderer_->Context()->PSSetSamplers(0,1,mesh->texture()->sampler());
    
    constant_buffer_light->UpdateBuffer(light->LightData());

    renderer_->Context()->PSSetConstantBuffers(2,1,constant_buffer_light->p_buffer());

    /*if (!light->shadowmap_shader) return;
    
    renderer_->Context()->PSSetShaderResources(1,1,light->shadowmap_shader->SRVShadow());
    renderer_->Context()->PSSetSamplers(1,1,light->shadowmap_shader->SamplerState());*/
}

void MainShader::ProvideLightData(LightComponent* light_)
{
    light = light_;
}
