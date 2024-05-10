#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader(Renderer& renderer);
    virtual void CreateShader() override;
    ID3D11VertexShader* vertex_shader() {return vertex_shader_;}
private:
    ID3D11VertexShader* vertex_shader_;
};
