#include "ReflectionShader.h"

#include <iostream>

#include "Mesh.h"
#include "PerspectiveCamera.h"
#include "PixelShader.h"
#include "Renderer.h"
#include "VertexShader.h"
#include "DirectionalLightComponent.h"
#include "FileTexture.h"
#include "ParticleSystem.h"
#include "Water.h"

ReflectionShader::ReflectionShader(Renderer* renderer) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
    constant_buffer_clip_plane = new ConstantBuffer<DirectX::SimpleMath::Vector4>(renderer_);
    constant_buffer_directional_light = new ConstantBuffer<DirectX::SimpleMath::Vector4>(renderer_);
    cbtm = new ConstantBufferTransformMatricies();
    
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = SHADOWMAP_WIDTH;
    textureDesc.Height = SHADOWMAP_HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&textureDesc, NULL, &RTTextureReflection)) )
     return;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateRenderTargetView(RTTextureReflection, &renderTargetViewDesc, &RTVReflection)) )
        return ;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(RTTextureReflection, &shaderResourceViewDesc, &SRVReflection_)) )
        return ;

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if( FAILED(renderer_->device()->CreateSamplerState(&samplerDesc, &SamplerState_)) )
        return ;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = SHADOWMAP_WIDTH;
    depthBufferDesc.Height = SHADOWMAP_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    //depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&depthBufferDesc, NULL, &DSTextureReflection)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DSTextureReflection, &depthStencilViewDesc, &DSVReflection)) )
        return ;
    
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
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
            0},
        D3D11_INPUT_ELEMENT_DESC{
            "TANGENT",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC{
            "BINORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };
    
    //pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyReflectionShader.hlsl");
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyReflectionShader.hlsl");

    normal_texture_ = new FileTexture(renderer_, L"Models/textures/default_normal.jpg");
    
    renderer_->device()->CreateInputLayout(
        inputElements,
        5,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    viewport_shadowmap = {};
    viewport_shadowmap.Width = SHADOWMAP_WIDTH;
    viewport_shadowmap.Height = SHADOWMAP_HEIGHT;
    viewport_shadowmap.TopLeftX = 0;
    viewport_shadowmap.TopLeftY = 0;
    viewport_shadowmap.MinDepth = 0;
    viewport_shadowmap.MaxDepth = 1.0f;
    
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    
    renderer_->device()->CreateRasterizerState(&rastDesc, &rastState);
}

//void ReflectionShader::Activate(DirectX::SimpleMath::Matrix viewMatrix, DirectX::SimpleMath::Vector4 clipPlane)
void ReflectionShader::Activate(WaterRenderType RenderType)
{
    DirectX::SimpleMath::Matrix viewMatrix;
    DirectX::SimpleMath::Vector4 clipPlane;
    DirectX::SimpleMath::Vector4 lightDir;
    switch (RenderType)
    {
    case Refration:
        viewMatrix = renderer_->camera()->view_matrix();
        clipPlane = DirectX::SimpleMath::Vector4(0.0f, -1.0f, 0.0f, water_->GetMesh()->location().y);
        lightDir = DirectionalLightComponent::GetDirectionalLight()->directional_light_data_->DirectionWS;
        break;
    case Reflection:
        /*viewMatrix = renderer_->camera()->view_matrix();
        clipPlane = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, -water_->GetMesh()->location().y);
        lightDir = -DirectionalLightComponent::GetDirectionalLight()->directional_light_data_->DirectionWS;*/
        viewMatrix = water_->reflection_view_matrix();
        clipPlane = DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, -water_->GetMesh()->location().y);
        lightDir = -DirectionalLightComponent::GetDirectionalLight()->directional_light_data_->DirectionWS;
        break;
    }
    
    renderer_->Context()->ClearState();

    

    cbtm->view = viewMatrix.Transpose();
    cbtm->projection = renderer_->camera()->projection_matrix().Transpose();
    cbtm->camera_position = DirectX::SimpleMath::Vector4(
        renderer_->camera()->position().x,
        renderer_->camera()->position().y,
        renderer_->camera()->position().z,
        1);

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    //renderer_->Context()->PSSetShaderResources(1, 1, water_->GetMesh()->/*ground()->*/normal_texture()->srv());
    renderer_->Context()->PSSetShaderResources(1, 1, normal_texture_->srv());
    
    //renderer_->Context()->VSSetConstantBuffers(0, 1, water_->GetMesh()->transform_matricies_buffer_data);
    constant_buffer_clip_plane->UpdateBuffer(&clipPlane);
    renderer_->Context()->VSSetConstantBuffers(1, 1, constant_buffer_clip_plane->p_buffer());

    constant_buffer_directional_light->UpdateBuffer(&lightDir);
    renderer_->Context()->PSSetConstantBuffers(2,1, constant_buffer_directional_light->p_buffer());

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    renderer_->Context()->OMSetRenderTargets(1, &RTVReflection, DSVReflection);
    renderer_->Context()->RSSetState(rastState);
    renderer_->Context()->RSSetViewports(1, &viewport_shadowmap);
    renderer_->Context()->ClearDepthStencilView(DSVReflection, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);
    renderer_->Context()->ClearRenderTargetView(RTVReflection, color);

    renderer_->DrawEverything();

    if (RenderType == Reflection)
    {
        renderer_->RenderParticles(RTVReflection);
    }
}

void ReflectionShader::ProvideMeshData(Mesh* mesh)
{
    if (mesh->texture())
    {
        cbtm->world = mesh->transform_matricies_buffer_data->world;
        constant_buffer_transform->UpdateBuffer(cbtm);
        renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());
    
        renderer_->Context()->PSSetShaderResources(0, 1, mesh->texture()->srv());
        renderer_->Context()->PSSetSamplers(0, 1, mesh->texture()->sampler());
    }
} 