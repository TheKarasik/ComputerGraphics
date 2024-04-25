#include "Game.h"

void Game::Initialize()
{
    Display_ = new Display32(L"Lab 2", 800*GOLDEN_RATIO, 800);
    Input_ = new InputDevice(this);

    SetUpRender();
    /*
    uint8_t depth = 15;
    
    LineSegment XLineSegment = LineSegment(-1, 1);
    LineSegment YLineSegment = LineSegment(1, -1);
    GRectangle = new GoldenRectangleComponent(this, XLineSegment, YLineSegment, true, depth);
    */
}

void Game::GameLoop()
{
    start = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> previous = start;
    lag = 0.0f;
    while (true)
    {
        std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - previous).count() / 1000000.0f ;
        //printf("elapsed: %f\n", elapsed);
        previous = current;
        lag += elapsed;
        
        ProcessInput();
        if (IsEscapePressed) return;

        while (lag >= S_PER_UPDATE)
        {
            //printf("lag: %f\n", lag);
            Update();
            lag -= S_PER_UPDATE;
        }
        Render();
    }
}

void Game::SetUpRender()
{
    
    
    D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

    DXGI_SWAP_CHAIN_DESC swapDesc = {};
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Width = Display_->screenWidth_;
    swapDesc.BufferDesc.Height = Display_->screenHeight_;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = Display_->hWnd;
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

    ID3DBlob* vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    res = D3DCompileFromFile(L"./Shaders/MyVeryFirstShader.hlsl",
        nullptr /*macros*/,
        nullptr /*include*/,
        "VSMain",
        "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        &vertexBC,
        &errorVertexCode);

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

    
    device_->CreateVertexShader(
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        nullptr, &vertexShader);

    device_->CreatePixelShader(
        pixelBC->GetBufferPointer(),
        pixelBC->GetBufferSize(),
        nullptr, &pixelShader);

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

    
    device_->CreateInputLayout(
        inputElements,
        2,
        vertexBC->GetBufferPointer(),
        vertexBC->GetBufferSize(),
        &layout);

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;

    res = device_->CreateRasterizerState(&rastDesc, &rastState);

    context_->RSSetState(rastState);
}

void Game::Render()
{
    context_->ClearState();
   
    context_->RSSetState(rastState);

    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(Display_->screenWidth_);
    viewport.Height = static_cast<float>(Display_->screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;

    context_->RSSetViewports(1, &viewport);

    context_->IASetInputLayout(layout);
    context_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    context_->VSSetShader(vertexShader, nullptr, 0);
    context_->PSSetShader(pixelShader, nullptr, 0);

    context_->OMSetRenderTargets(1, &rtv, nullptr);

    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    context_->ClearRenderTargetView(rtv, color);

    //GRectangle->Draw();

    context_->OMSetRenderTargets(0, nullptr, nullptr);

    swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::Update()
{
    /*
    std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
    float t = std::chrono::duration_cast<std::chrono::microseconds>(current - start).count() / 1000000.0f ;
    GRectangle->Update(t*0.1f);
    */
}

void Game::ProcessInput()
{
    
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (Input_->IsKeyDown(Keys::Escape) || msg.message == WM_QUIT) IsEscapePressed = true;
}
