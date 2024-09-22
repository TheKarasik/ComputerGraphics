#include "ComputeShader.h"

ComputeShader::ComputeShader(Renderer& renderer, LPCWSTR file_name) : Shader(renderer, file_name)
{
    entry_point_ = "CSMain";
    target_ = "cs_5_0";
    //shader_macro_ ("TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr) ;
    CompileShader();
    ComputeShader::CreateShader();
}

void ComputeShader::CreateShader()
{
    renderer_.device()->CreateComputeShader(
        byte_code_->GetBufferPointer(),
        byte_code_->GetBufferSize(),
        nullptr, &compute_shader_);
}