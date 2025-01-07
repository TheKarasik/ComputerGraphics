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
#include "CascadeShadowmapShader.h"
#include "DirectionalLightComponent.h"
#include "GBufferShader.h"
#include "LightComponent.h"
#include "LightingStageShader.h"
#include "MainShader.h"
#include "MiniMapCamera.h"
#include "OrthographicCamera.h"
#include "ParticleSystem.h"
#include "ReflectionShader.h"
#include "ShadowmapShader.h"
#include "Water.h"

//std::vector<Drawable*> Drawable::visual_objects;

Renderer::Renderer(Display32& Display, PerspectiveCamera* camera) : Display_(Display), camera_(camera)
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

    
    D3D11_TEXTURE2D_DESC TextureDepthStencilDesc;
    ZeroMemory(&TextureDepthStencilDesc, sizeof(TextureDepthStencilDesc));
    TextureDepthStencilDesc.Width = Display_.screenWidth_;
    TextureDepthStencilDesc.Height = Display_.screenHeight_;
    TextureDepthStencilDesc.MipLevels = 1;
    TextureDepthStencilDesc.ArraySize = 1;
    TextureDepthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    TextureDepthStencilDesc.SampleDesc.Count = 1;
    TextureDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
    TextureDepthStencilDesc.CPUAccessFlags = 0;
    TextureDepthStencilDesc.MiscFlags = 0;
    if( FAILED(device()->CreateTexture2D(&TextureDepthStencilDesc, NULL, &RTDepthStencilTexture)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT ;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(device()->CreateDepthStencilView(RTDepthStencilTexture, &depthStencilViewDesc, &dsv_main)) )
        return ;
    
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDepthDesc;
    SRVDepthDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    SRVDepthDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDepthDesc.Texture2D.MostDetailedMip = 0;
    SRVDepthDesc.Texture2D.MipLevels = 1;
    if( FAILED(device()->CreateShaderResourceView(RTDepthStencilTexture, &SRVDepthDesc, &SRVDepth)) )
        return;
    
    //main_shader_ = new MainShader(this, swapChain, device_, Display_);
    gbuffer_shader_ = new GBufferShader(this, &Display_);
    lighting_stage_shader_ = new LightingStageShader(this, gbuffer_shader_, &Display_);

    particle_systems_ = new std::vector<ParticleSystem*>;

    //gbuffer_shader_->ProvideParticles(particle_systems_);
    
    /*particle_system_ = new ParticleSystem(100, DirectX::SimpleMath::Vector3(0,0,0), this);
    particle_system_->initialize_system();*/
    
    //Mini map creation

    /*D3D11_TEXTURE2D_DESC black_white_buffer_desc;
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
    device()->CreateShaderResourceView(mini_map_buffer_, &shaderResourceViewDesc, &srv_mini_map);*/
    
}

Renderer::~Renderer()
{
    context_->ClearState();
    //layout->Release();
    //pixel_shader_->pixel_shader()->Release();
    //vertex_shader_->vertex_shader()->Release();
    //rtv_main->Release();
    swapChain->Release();
    context_->Release();
    device()->Release();
}

void Renderer::RenderParticles(ID3D11RenderTargetView* rtv)
{
    for(const auto ps : *particle_systems_)
    {
        ps->Render(rtv);
    }
}

void Renderer::DrawEverything()
{
    for(auto vo : Drawable::visual_objects)
    {
        (*vo).draw();
    }
}

void Renderer::Render()
{
    
    //Рендер сцены для Cascade ShadowMap
    if (DirectionalLightComponent::GetDirectionalLight())
    {
        RenderingState = CascadeShadowmapRenderState;
        DirectionalLightComponent::GetDirectionalLight()->cascade_shadowmap_shader()->Activate();
    }
    
    //Рендер сцены для ShadowMap
    /*if (DirectionalLightComponent::GetDirectionalLight())
    {
        RenderingState = ShadowmapRenderState;
        DirectionalLightComponent::GetDirectionalLight()->shadowmap_shader()->Activate();
    }*/
    
    //Рендер сцены для GBuffer
    RenderingState = GBufferRenderState;
    gbuffer_shader_->Activate();

    //Рендер сцены для LightStage
    RenderingState = LightingStageState;
    lighting_stage_shader_->Activate();
    
    
    if(particle_systems_)
    {
        for(const auto ps : *particle_systems_)
        {
            ps->ProcessFrame();
        }
    }
    
    water_->render();
    
    //Рендер сцены для игрока
    
    //RenderingState = MainRenderState;
    //main_shader_->Activate();
    
    //Рендер мини карты
    /*RenderingState = MinimapRenderState;
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
    }*/
    
    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
    RenderingState = NoRenderState;
}

void Renderer::ProvideMeshData(Mesh* mesh)
{
    switch (RenderingState)
    {
    case MainRenderState:
        main_shader_->ProvideMeshData(mesh);
        break;
    case CascadeShadowmapRenderState:
        DirectionalLightComponent::GetDirectionalLight()->cascade_shadowmap_shader()->ProvideMeshData(mesh);
        break;
    case ShadowmapRenderState:
        DirectionalLightComponent::GetDirectionalLight()->shadowmap_shader()->ProvideMeshData(mesh);
        break;
    case GBufferRenderState:
        gbuffer_shader_->ProvideMeshData(mesh);
        break;
    case LightingStageState:
        lighting_stage_shader_->ProvideMeshData(mesh);
        break;
    case ReflectionState:
        water_->GetCurrentReflectionShader()->ProvideMeshData(mesh);
    }
}

PerspectiveCamera* Renderer::camera()
{
    //if(RenderingState == ShadowmapRenderState) return DirectionalLightComponent::GetDirectionalLight()->Camera();
    return camera_;
    //if(RenderingState == MinimapRenderState) return mini_map_camera_;
}

/*void Renderer::UpdateTransformData(Object3D::ConstantBufferTransformMatricies* TransformData)
{
    
}*/



