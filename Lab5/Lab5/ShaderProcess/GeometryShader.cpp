#include "GeometryShader.h"

GeometryShader::GeometryShader(Renderer& renderer, LPCWSTR file_name) : Shader(renderer, file_name)
{
    entry_point_ = "GSMain";
    target_ = "gs_5_0";
    CompileShader();
    GeometryShader::CreateShader();
}

void GeometryShader::CreateShader()
{
    renderer_.device()->CreateGeometryShader(
        byte_code_->GetBufferPointer(),
        byte_code_->GetBufferSize(),
        nullptr, &geometry_shader_);
}
