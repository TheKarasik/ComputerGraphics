#include "LightComponent.h"

#include "OrthographicCamera.h"

LightComponent::LightComponent(Renderer* renderer)
{
    renderer_ = renderer;
    light_data.directionWS = DirectX::SimpleMath::Vector4(direction_.x, direction_.y, direction_.z, 0);
    ShadowmapCamera = new OrthographicCamera(position_, direction_, up_direction_);
}

void LightComponent::update()
{
    SetUpMatricies();
    light_data.mViewProj = ortographic_matrix_.Transpose()*view_matrix_.Transpose();
    constant_buffer_light->UpdateBuffer(&light_data);
}

void LightComponent::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, direction_, up_direction_);
    ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}

void LightComponent::Activate()
{
    renderer_->Context()->PSSetConstantBuffers(2,1,constant_buffer_light->buffer());
}

void LightComponent::InitializeShadows()
{
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
    if( FAILED(renderer_->device()->CreateTexture2D(&textureDesc, NULL, &RTTextureShadow)) )
        return;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateRenderTargetView(RTTextureShadow, &renderTargetViewDesc, &RTVShadow)) )
        return ;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(RTTextureShadow, &shaderResourceViewDesc, &SRVShadow)) )
        return ;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = SHADOWMAP_WIDTH;
    depthBufferDesc.Height = SHADOWMAP_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&depthBufferDesc, NULL, &DSTextureShadow)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = depthBufferDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DSTextureShadow, &depthStencilViewDesc, &DSVShadow)) )
        return ;

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    if( FAILED(renderer_->device()->CreateSamplerState(&samplerDesc, &SamplerState)) )
        return ;
}
