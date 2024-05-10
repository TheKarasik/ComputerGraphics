#include "Renderer.h"

#include "ConstantBuffer.h"


#include "PixelShader.h"
#include "GameComponent.h"
#include "Shader.h"
#include "VertexShader.h"

#include "Renderer.h"

#include "ConstantBuffer.h"
#include "Display32.h"
#include "AbstractBuffer.h"

Renderer::Renderer(Display32& Display) : Display_(Display)
{
    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = Display_.screenWidth_;
    swapDesc.BufferDesc.Height = Display_.screenHeight_;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = Display_.hWnd;
    swapDesc.Windowed = true;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.SampleDesc.Quality = 0;

    auto res = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_DEBUG,
        featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapDesc,
        &swapChain,
        &device_,
        nullptr,
        &context_);

    if (FAILED(res))
    {
        // Well, that was unexpected
    }

    ID3D11Texture2D* backTex;
    res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
    res = device_->CreateRenderTargetView(backTex, nullptr, &rtv);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };
    
    pixel_shader = new PixelShader(*this);
    vertex_shader = new VertexShader(*this);

    
    
    device_->CreateInputLayout(
        inputElements,
        2,
        vertex_shader->byte_code()->GetBufferPointer(),
        vertex_shader->byte_code()->GetBufferSize(),
        &layout);

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    res = device_->CreateRasterizerState(&rastDesc, &rastState);

    context_->RSSetState(rastState);
}

void Renderer::Render(std::vector<GameComponent*> visual_objects)
{
    context_->ClearState();
   
    context_->RSSetState(rastState);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display_.screenWidth_);
    viewport.Height = static_cast<float>(Display_.screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context_->RSSetViewports(1, &viewport);

    context_->IASetInputLayout(layout);
    context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    context_->VSSetShader(vertex_shader->vertex_shader(), nullptr, 0);
    context_->PSSetShader(pixel_shader->pixel_shader(), nullptr, 0);

    context_->OMSetRenderTargets(1, &rtv, nullptr);

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    context_->ClearRenderTargetView(rtv, color);

    for(auto vo : visual_objects)
    {
        (*vo).draw();
    }

    //ID3D11RenderTargetView* nullViews [] = { nullptr };
    //context_->OMSetRenderTargets(1, nullViews, 0);
    context_->OMSetRenderTargets(0, nullptr, nullptr);
    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}
