#include "MyVeryFirstShaderClass.h"

MyVeryFirstShaderClass::MyVeryFirstShaderClass(HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    HRESULT res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexBC,
        &errorVertexCode);

    if (FAILED(res)) {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode) {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
        }

        return;
    }

    D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

    ID3DBlob* pixelBC;
    ID3DBlob* errorPixelCode;
    res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        Shader_Macros /*macros*/,
        nullptr /*include*/,
        "PSMain",
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &pixelBC,
        &errorPixelCode);

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    device->CreateVertexShader(
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        nullptr, &vertexShader);

    device->CreatePixelShader(
        pixelBC->GetBufferPointer(),
        pixelBC->GetBufferSize(),
        nullptr, &pixelShader);
}
