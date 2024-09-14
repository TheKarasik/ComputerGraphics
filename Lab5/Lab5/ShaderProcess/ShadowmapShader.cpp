#include "ShadowmapShader.h"

#include <d3d11.h>

#include "DirectionalLightComponent.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "LightComponent.h"
#include "OrthographicCamera.h"

ShadowmapShader::ShadowmapShader(Renderer* renderer) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);

       /*D3D11_TEXTURE2D_DESC textureDesc;
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
    if( FAILED(renderer_->device()->CreateTexture2D(&textureDesc, NULL, &RTTextureShadow)) )
        return;*/

    /*D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateRenderTargetView(RTTextureShadow, &renderTargetViewDesc, &RTVShadow)) )
        return ;*/

    /*D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(RTTextureShadow, &shaderResourceViewDesc, &SRVShadow_)) )
        return ;*/

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
    if( FAILED(renderer_->device()->CreateTexture2D(&depthBufferDesc, NULL, &DSTextureShadow)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DSTextureShadow, &depthStencilViewDesc, &DSVShadow)) )
        return ;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(DSTextureShadow, &shaderResourceViewDesc, &SRVShadow_)) )
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
    
    //pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    
    renderer_->device()->CreateInputLayout(
        inputElements,
        1,
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

void ShadowmapShader::Activate()
{
    //Рендер shadowmap в текстуру
    renderer_->Context()->ClearState();

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);

    renderer_->Context()->OMSetRenderTargets(0, nullptr, DSVShadow);
    renderer_->Context()->RSSetState(rastState);
    renderer_->Context()->RSSetViewports(1, &viewport_shadowmap);

    renderer_->Context()->ClearDepthStencilView(DSVShadow, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    renderer_->DrawEverything();
}

void ShadowmapShader::ProvideMeshData(Mesh* mesh)
{
    ConstantBufferTransformMatricies cb = {};
    cb.world = mesh->transform_matricies_buffer_data.world;
    cb.view = DirectionalLightComponent::GetDirectionalLight()->Camera()->view_matrix().Transpose();
    cb.projection = DirectionalLightComponent::GetDirectionalLight()->Camera()->projection_matrix().Transpose();
    cb.camera_position = DirectionalLightComponent::GetDirectionalLight()->directional_light_data_.PositionWS;
    
    constant_buffer_transform->UpdateBuffer(&cb);
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->buffer());
}

void ShadowmapShader::ProvideLightData(LightComponent* light)
{
    light_component_ = light;
}
