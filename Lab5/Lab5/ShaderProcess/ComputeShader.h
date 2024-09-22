#pragma once
#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader(Renderer& renderer, LPCWSTR file_name);
    void CreateShader() override;
    ID3D11ComputeShader* compute_shader() {return compute_shader_;}
private:
    ID3D11ComputeShader* compute_shader_;
};
