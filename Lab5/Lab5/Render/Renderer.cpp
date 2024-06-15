#include "Renderer.h"

#include "ConstantBuffer.h"


#include "PixelShader.h"
#include "Drawable.h"
#include "Shader.h"
#include "VertexShader.h"

#include "Renderer.h"

#include "ConstantBuffer.h"
#include "Display32.h"
#include "AbstractBuffer.h"
#include "LightComponent.h"
#include "MiniMapCamera.h"

std::vector<Drawable*> Drawable::visual_objects;

Renderer::Renderer(Display32& Display, CameraObject* camera) : Display_(Display), camera_(camera)
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
    res = device_->CreateRenderTargetView(backTex, nullptr, &rtv_main);

    D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc;
    ZeroMemory( &depth_stencil_buffer_desc, sizeof(D3D11_TEXTURE2D_DESC) );
 
    depth_stencil_buffer_desc.ArraySize = 1;
    depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_buffer_desc.CPUAccessFlags = 0; // No CPU access required.
    depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_buffer_desc.Width = Display_.screenWidth_;
    depth_stencil_buffer_desc.Height = Display_.screenHeight_;
    depth_stencil_buffer_desc.MipLevels = 1;
    depth_stencil_buffer_desc.SampleDesc.Count = 1;
    depth_stencil_buffer_desc.SampleDesc.Quality = 0;
    depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    
    device_->CreateTexture2D( &depth_stencil_buffer_desc, nullptr, &depth_stencil_buffer_);
    device_->CreateDepthStencilView(depth_stencil_buffer_, nullptr, &dsv);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc;
    ZeroMemory( &depth_stencil_state_desc, sizeof(D3D11_DEPTH_STENCIL_DESC) );
 
    depth_stencil_state_desc.DepthEnable = TRUE;
    depth_stencil_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_state_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stencil_state_desc.StencilEnable = FALSE;
 
    device_->CreateDepthStencilState( &depth_stencil_state_desc, &depth_stencil_state_);

    D3D11_TEXTURE2D_DESC black_white_buffer_desc;
    ZeroMemory( &black_white_buffer_desc, sizeof(D3D11_TEXTURE2D_DESC) );
 
    black_white_buffer_desc.ArraySize = 1;
    black_white_buffer_desc.BindFlags = D3D11_BIND_RENDER_TARGET;
    black_white_buffer_desc.CPUAccessFlags = 0; // No CPU access required.
    black_white_buffer_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    black_white_buffer_desc.Width = Display_.screenWidth_*0.1;
    black_white_buffer_desc.Height = Display_.screenHeight_*0.1;
    black_white_buffer_desc.MipLevels = 1;
    black_white_buffer_desc.SampleDesc.Count = 1;
    black_white_buffer_desc.SampleDesc.Quality = 0;
    black_white_buffer_desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
    renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
    shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    
    device_->CreateTexture2D( &black_white_buffer_desc, nullptr, &mini_map_buffer_);
    res = device_->CreateRenderTargetView(mini_map_buffer_, &renderTargetViewDesc, &rtv_mini_map);
    device()->CreateShaderResourceView(mini_map_buffer_, &shaderResourceViewDesc, &srv_mini_map);

    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        /*D3D11_INPUT_ELEMENT_DESC {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}*/
        D3D11_INPUT_ELEMENT_DESC{
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC{
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0}
    };
    
    pixel_shader_ = new PixelShader(*this);
    vertex_shader_ = new VertexShader(*this);

    
    
    device_->CreateInputLayout(
        inputElements,
        3,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    res = device_->CreateRasterizerState(&rastDesc, &rastState);

    context_->RSSetState(rastState);
    
}

Renderer::~Renderer()
{
    context_->ClearState();
    layout->Release();
    pixel_shader()->pixel_shader()->Release();
    vertex_shader()->vertex_shader()->Release();
    rtv_main->Release();
    swapChain->Release();
    context_->Release();
    device()->Release();
}

void Renderer::Render()
{
    context_->ClearState();
   
    context_->RSSetState(rastState);
    for (auto co : Updatable::changing_objects) co->update();
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display_.screenWidth_);
    viewport.Height = static_cast<float>(Display_.screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context_->IASetInputLayout(layout);
    context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    context_->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    context_->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    context_->OMSetRenderTargets(1, &rtv_main, dsv);
    context_->OMSetDepthStencilState(depth_stencil_state_, 1);

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    context_->ClearRenderTargetView(rtv_main, color);
    context_->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    context_->RSSetViewports(1, &viewport);
    
    light_->Activate();
    for(auto vo : Drawable::visual_objects)
    {
        (*vo).draw();
    }
    IsRenderingMain = false;

    for (auto co : Updatable::changing_objects) co->update();
    
    D3D11_VIEWPORT viewport_mini_map = {};
    viewport_mini_map.Width = static_cast<float>(Display_.screenWidth_*0.2);
    viewport_mini_map.Height = static_cast<float>(Display_.screenHeight_*0.2);
    viewport_mini_map.TopLeftX = 0;
    viewport_mini_map.TopLeftY = 0;
    viewport_mini_map.MinDepth = 0;
    viewport_mini_map.MaxDepth = 1.0f;
    
    //context_->ClearRenderTargetView(rtv_main, color);
    //context_->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);

    context_->RSSetViewports(1, &viewport_mini_map);
    for(auto vo : Drawable::visual_objects)
    {
        (*vo).draw();
    }
    
    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    IsRenderingMain = true;
}

CameraObject* Renderer::camera()
{
    if(IsRenderingMain) return camera_;
    return mini_map_camera_;
}



