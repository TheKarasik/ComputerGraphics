#include "Texture.h"

#include <WICTextureLoader.h>

#include "Renderer.h"

Texture::Texture(Renderer* renderer, const wchar_t* pFile) : renderer_(renderer)
{
    //DirectX::CreateWICTextureFromFile(renderer_->device().Get(), renderer_->Context(), pFile, nullptr, &srv_);
    DirectX::CreateWICTextureFromFile(renderer_->device().Get(), pFile, nullptr, &srv_);
    //sampler_desc_ = new D3D11_SAMPLER_DESC();
    sampler_desc_.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;    
    sampler_desc_.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;         
    sampler_desc_.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc_.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc_.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampler_desc_.MinLOD = 0;
    sampler_desc_.MaxLOD = D3D11_FLOAT32_MAX;

    renderer_->device()->CreateSamplerState( &sampler_desc_, &sampler_ );
}
