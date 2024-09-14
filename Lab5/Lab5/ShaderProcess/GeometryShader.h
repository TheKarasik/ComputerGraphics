#pragma once
#include "Shader.h"

class GeometryShader : public Shader
{
public:
    GeometryShader(Renderer& renderer, LPCWSTR file_name);
    virtual void CreateShader() override;
    ID3D11GeometryShader* geometry_shader() {return  geometry_shader_;}
private:
    ID3D11GeometryShader* geometry_shader_;
};
