#pragma once
#include <string>

//#include "ConstantBuffer.h"
#include "PixelShader.h"

class Renderer;

class FileTexture
{
public:
    MaterialStructCB MatProp;
    FileTexture(Renderer* renderer, const wchar_t* pFile);
    ID3D11SamplerState** sampler() {return &sampler_;}
    ID3D11ShaderResourceView** srv() {return &srv_;}
    //ConstantBuffer<MaterialProperties>* texture_constant_buffer_;
private:
    Renderer* renderer_;
    ID3D11ShaderResourceView* srv_;
    D3D11_SAMPLER_DESC sampler_desc_;
    ID3D11SamplerState* sampler_;


};
