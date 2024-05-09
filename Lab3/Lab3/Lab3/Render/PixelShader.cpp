#include "PixelShader.h"

PixelShader::PixelShader(Renderer& renderer) : Shader(renderer)
{
    entry_point_ = "PSMain";
    target_ = "ps_5_0";
    //shader_macro_ ("TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr) ;
    CompileShader();
    PixelShader::CreateShader();
}

void PixelShader::CreateShader()
{
    renderer_.device()->CreatePixelShader(
        byte_code_->GetBufferPointer(),
        byte_code_->GetBufferSize(),
        nullptr, &pixel_shader_);
}
