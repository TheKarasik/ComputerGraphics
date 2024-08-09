#pragma once
#include "Shader.h"

class PixelShader : public Shader
{
public:
    PixelShader(Renderer& renderer, LPCWSTR file_name);
    virtual void CreateShader() override;
    ID3D11PixelShader* pixel_shader() {return  pixel_shader_;}
private:
    ID3D11PixelShader* pixel_shader_;
};
