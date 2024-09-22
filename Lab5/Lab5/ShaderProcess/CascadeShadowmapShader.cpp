#include "CascadeShadowmapShader.h"

#include <d3d11.h>

#include "DirectionalLightComponent.h"
#include "GeometryShader.h"
#include "Mesh.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "LightComponent.h"
#include "OrthographicCamera.h"

CascadeShadowmapShader::CascadeShadowmapShader(Renderer* renderer) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<DirectX::SimpleMath::Matrix>(renderer_);
    constant_buffer_cascade = new ConstantBuffer<CascadeData>(renderer_);
    
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = SHADOWMAP_WIDTH;
    depthBufferDesc.Height = SHADOWMAP_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = CASCADE_COUNT;
    //depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&depthBufferDesc, NULL, &DSTextureShadowArray)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    //depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
    depthStencilViewDesc.Texture2DArray.ArraySize = CASCADE_COUNT;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DSTextureShadowArray, &depthStencilViewDesc, &DSVShadow)) )
        return ;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    //shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
    shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;	
    shaderResourceViewDesc.Texture2DArray.ArraySize = 4;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(DSTextureShadowArray, &shaderResourceViewDesc, &SRVShadow_)) )
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
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyCascadeShadowmapShader.hlsl");
    geometry_shader_ = new GeometryShader(*renderer_, L"./Shaders/MyCascadeShadowmapShader.hlsl");
    
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

void CascadeShadowmapShader::Activate()
{
    //Рендер shadowmap в текстуру
    renderer_->Context()->ClearState();
    
    //for (auto co : Updatable::changing_objects) co->update();  !!!ВОЗМОЖНО ПРИДЁТСЯ РАСКОММЕНТИРОВАТЬ!!!

    //renderer_->Context()

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->GSSetShader(geometry_shader_->geometry_shader(), nullptr, 0);

    renderer_->Context()->OMSetRenderTargets(0, nullptr, DSVShadow);
    renderer_->Context()->RSSetState(rastState);
    renderer_->Context()->RSSetViewports(1, &viewport_shadowmap);
    renderer_->Context()->ClearDepthStencilView(DSVShadow, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    renderer_->DrawEverything();
}

void CascadeShadowmapShader::ProvideMeshData(Mesh* mesh)
{
    constant_buffer_transform->UpdateBuffer(&mesh->transform_matricies_buffer_data->world);
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());

    const auto a = renderer_->camera()->cascade_data();
    constant_buffer_cascade->UpdateBuffer(a);
    renderer_->Context()->GSSetConstantBuffers(1,1, constant_buffer_cascade->p_buffer());
}

void CascadeShadowmapShader::ProvideLightData(LightComponent* light)
{
    light_component_ = light;
}
