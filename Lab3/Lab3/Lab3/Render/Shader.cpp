#include "Shader.h"

#include <fstream>
#include <iostream>

Shader::Shader(Renderer& renderer) : renderer_(renderer) {}

void Shader::CompileShader()
{
    /*std::ifstream ifile;
    ifile.open(L"./Shaders/MyVeryFirstShader.hlsl");
    if(ifile) {
        std::cout<<"file exists";
    } else {
        std::cout<<"file doesn't exist";
    }*/
    
    if (!(entry_point_ && target_)) return;
    HRESULT res = D3DCompileFromFile(file_name_,
        shader_macro_ /*macros*/,
        nullptr /*include*/,
        entry_point_,
        target_,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &byte_code_,
        &error_byte_code_);

    
}
