#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader(Renderer& renderer);
    virtual void CreateShader() override;
private:
    ID3D11VertexShader* vertex_shader_;
};
