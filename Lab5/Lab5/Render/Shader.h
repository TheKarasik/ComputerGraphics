#pragma once
#include <d3dcommon.h>
#include <d3dcompiler.h>

#include "Renderer.h"

class Shader
{
public:
    virtual ~Shader() = default;
    ID3DBlob* byte_code() {return byte_code_;}
protected:
    Shader(Renderer& renderer, LPCWSTR file_name);
    virtual void CompileShader() final;
    virtual void CreateShader() = 0;

    Renderer& renderer_;
    
    ID3DBlob* byte_code_;
    ID3DBlob* error_byte_code_;
    
    LPCWSTR file_name_;
    D3D_SHADER_MACRO* shader_macro_ = nullptr;
    LPCSTR entry_point_ = nullptr;
    LPCSTR target_ = nullptr;
};
