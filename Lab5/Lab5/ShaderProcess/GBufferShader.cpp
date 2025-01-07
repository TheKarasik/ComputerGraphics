#include "GBufferShader.h"

#include <d3d11.h>

#include "CascadeShadowmapShader.h"
#include "DirectionalLightComponent.h"
#include "LightComponent.h"

#include "Mesh.h"
#include "Renderer.h"
#include "ShadowmapShader.h"
#include "VertexShader.h"
#include "..\Render\FileTexture.h"
#include "OrthographicCamera.h"
#include "ParticleSystem.h"

GBufferShader::GBufferShader(Renderer* renderer, Display32* display) : renderer_(renderer), display_(display)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
    constant_buffer_texture = new ConstantBuffer<MaterialStructCB>(renderer_);
    //constant_buffer_dir_lights_num = new ConstantBuffer<StructuredBufferSize>(renderer_);
    constant_buffer_cascade = new ConstantBuffer<CascadeData>(renderer_);
    constant_buffer_screen_to_view = new ConstantBuffer<ScrenToViewStructCB>(renderer_);
    ScreenDimensions = DirectX::SimpleMath::Vector2(display_->screenWidth_, display_->screenHeight_);

    //Создаем текстурки
    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(TextureDesc));
    TextureDesc.Width = display->screenWidth_;
    TextureDesc.Height = display->screenHeight_;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    //TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;
    for (int i = 0; i < BUFFER_NUM-1; i++)
    {
        if (i == 1) TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        else TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        if( FAILED(renderer_->device()->CreateTexture2D(&TextureDesc, NULL, &LightTextures[i])) )
            return;
    }
    
    //Создаем виды целей рендера
    D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
    //RTVDesc.Format = TextureDesc.Format;
    RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    RTVDesc.Texture2D.MipSlice = 0;
    for (int i = 0; i < BUFFER_NUM-1; i++)
    {
        if (i == 1) RTVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        else RTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        if( FAILED(renderer_->device()->CreateRenderTargetView(LightTextures[i], &RTVDesc, &RTV[i])) )
            return;
    }
    RTV[BUFFER_NUM-1] = *renderer_->RTVMain();

    //Создаем виды ресурсов шейдера
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    //SRVDesc.Format = TextureDesc.Format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;	
    for (int i = 0; i < BUFFER_NUM-1; i++)
    {
        if (i == 1) SRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        else SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        if( FAILED(renderer_->device()->CreateShaderResourceView(LightTextures[i], &SRVDesc, &SRV[i])) )
            return;
    }

    //Буффер глубины

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
    
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyVertexShader.hlsl");
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyGBufferShader.hlsl");
    
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
    viewport.Width = static_cast<float>(display_->screenWidth_);
    viewport.Height = static_cast<float>(display_->screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;
}

void GBufferShader::Activate()
{
    renderer_->Context()->ClearState();

    DirectX::SimpleMath::Vector3 CamPos = renderer_->camera()->position();
    ScrenToViewStructCB ScrenToView = {};
    ScrenToView.inverse_projection = renderer_->camera()->projection_matrix().Transpose().Invert();
    ScrenToView.screen_dimensions = ScreenDimensions;
    constant_buffer_screen_to_view->UpdateBuffer(&ScrenToView);
    //StructuredBufferSize temp = {};
    //temp.size = DirectionalLightComponent::DirectionalLightsNum();
    
    renderer_->Context()->PSSetConstantBuffers(2, 1, constant_buffer_screen_to_view->p_buffer());

    constant_buffer_cascade->UpdateBuffer(renderer_->camera()->cascade_data());
    renderer_->Context()->PSSetConstantBuffers(3,1, constant_buffer_cascade->p_buffer());
   
    renderer_->Context()->RSSetState(rastState);

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    if (constant_buffer_directional_light) 
    {
        constant_buffer_directional_light->UpdateBuffer(DirectionalLightComponent::DirectionalLightData());
        renderer_->Context()->PSSetShaderResources(1,1,DirectionalLightComponent::GetDirectionalLight()->shadowmap_shader()->SRVShadow());
        renderer_->Context()->PSSetShaderResources(2,1,DirectionalLightComponent::GetDirectionalLight()->cascade_shadowmap_shader()->SRVShadow());
        renderer_->Context()->PSSetSamplers(1,1,DirectionalLightComponent::GetDirectionalLight()->shadowmap_shader()->SamplerState());
        renderer_->Context()->PSSetConstantBuffers(1, 1, constant_buffer_directional_light->p_buffer());
        //renderer_->Context()->PSSetShaderResources(2, 1, directional_lights->SRV());
    }
    
    //constant_buffer_dir_lights_num->UpdateBuffer(&temp);
    //renderer_->Context()->PSSetConstantBuffers(1,1, constant_buffer_dir_lights_num->buffer());

    renderer_->Context()->OMSetRenderTargets(BUFFER_NUM, RTV, *renderer_->DSVMain());
    renderer_->Context()->RSSetViewports(1, &viewport);
    
    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    //float color[] = { 0.39f, 0.58f, 0.93f, 1.0f };
    for (int i = 0; i < BUFFER_NUM; i++)
    {
        renderer_->Context()->ClearRenderTargetView(RTV[i], color);
    }
    renderer_->Context()->ClearDepthStencilView(*renderer_->DSVMain(), D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    renderer_->DrawEverything();

    /*if(particle_systems_)
    {
        for(const auto ps : *particle_systems_)
        {
            ps->draw();
        }
    }*/
}

void GBufferShader::ProvideMeshData(Mesh* mesh)
{
    constant_buffer_transform->UpdateBuffer(mesh->transform_matricies_buffer_data);
    
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());

    constant_buffer_texture->UpdateBuffer(&mesh->texture()->MatProp);
    
    renderer_->Context()->PSSetConstantBuffers(0,1,constant_buffer_texture->p_buffer());

    renderer_->Context()->PSSetShaderResources(0,1,mesh->texture()->srv());
    renderer_->Context()->PSSetSamplers(0,1,mesh->texture()->sampler());
}

void GBufferShader::UpdateDirectionalLights()
{
    delete constant_buffer_directional_light;
    constant_buffer_directional_light = new ConstantBuffer<DirectionalLightDataStruct>(renderer_);
}

