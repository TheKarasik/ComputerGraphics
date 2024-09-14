#include "VertexShader.h"

VertexShader::VertexShader(Renderer& renderer, LPCWSTR file_name) : Shader(renderer, file_name)
{
    entry_point_ = "VSMain";
    target_ = "vs_5_0";
    CompileShader();
    VertexShader::CreateShader();
}

void VertexShader::CreateShader()
{
    renderer_.device()->CreateVertexShader(
        byte_code_->GetBufferPointer(),
        byte_code_->GetBufferSize(),
        nullptr, &vertex_shader_);
}
