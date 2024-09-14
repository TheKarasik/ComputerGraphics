#include "LightingStageShader.h"

#include "FileTexture.h"
#include "GBufferShader.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"

std::vector<LightComponent*> LightComponent::Lights;

LightingStageShader::LightingStageShader(Renderer* renderer, GBufferShader* GBuffer, Display32* display_) :
    renderer_(renderer), GBuffer_(GBuffer)
{
    //MaterialCB = new ConstantBuffer<MaterialStructCB>(renderer);
    ScreenToViewCB = new ConstantBuffer<ScrenToViewStructCB>(renderer);
    LightDataCB = new ConstantBuffer<LightDataStruct>(renderer);
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer);

    ScreenDimensions = DirectX::SimpleMath::Vector2(display_->screenWidth_, display_->screenHeight_);

    D3D11_TEXTURE2D_DESC TextureDepthStencilDesc;
    ZeroMemory(&TextureDepthStencilDesc, sizeof(TextureDepthStencilDesc));
    TextureDepthStencilDesc.Width = display_->screenWidth_;
    TextureDepthStencilDesc.Height = display_->screenHeight_;
    TextureDepthStencilDesc.MipLevels = 1;
    TextureDepthStencilDesc.ArraySize = 1;
    TextureDepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    TextureDepthStencilDesc.SampleDesc.Count = 1;
    TextureDepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    TextureDepthStencilDesc.CPUAccessFlags = 0;
    TextureDepthStencilDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&TextureDepthStencilDesc, NULL, &DepthStencilTexture)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = TextureDepthStencilDesc.Format;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DepthStencilTexture, &depthStencilViewDesc, &DSV)) )
        return ;

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

    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyVertexShader.hlsl");
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyLightingStageShader.hlsl");

    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_FRONT;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    //rastDesc.DepthClipEnable = true;
    renderer_->device()->CreateRasterizerState(&rastDesc, &rastStateUP);

    rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_BACK;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.DepthClipEnable = false;
    renderer_->device()->CreateRasterizerState(&rastDesc, &rastStateSP);

    D3D11_DEPTH_STENCILOP_DESC DepthStencilOpFrontDesc = {};
    D3D11_DEPTH_STENCILOP_DESC DepthStencilOpBackDesc = {};
    
    DepthStencilOpFrontDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpFrontDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpFrontDesc.StencilPassOp = D3D11_STENCIL_OP_DECR_SAT;
    DepthStencilOpFrontDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;

    DepthStencilOpBackDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;

    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = true;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    DepthStencilDesc.StencilEnable = true;
    DepthStencilDesc.StencilReadMask = 0xff;
    DepthStencilDesc.StencilWriteMask = 0xff;
    DepthStencilDesc.FrontFace = DepthStencilOpFrontDesc;
    DepthStencilDesc.BackFace = DepthStencilOpBackDesc;

    renderer_->device()->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilStateUP);

    DepthStencilOpFrontDesc = {};
    DepthStencilOpBackDesc = {};
    
    DepthStencilOpFrontDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpFrontDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpFrontDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpFrontDesc.StencilFunc = D3D11_COMPARISON_ALWAYS;
    
    DepthStencilOpBackDesc.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilOpBackDesc.StencilFunc = D3D11_COMPARISON_EQUAL;

    DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = true;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    DepthStencilDesc.StencilEnable = true; 
    DepthStencilDesc.StencilReadMask = 0xff;
    DepthStencilDesc.StencilWriteMask = 0xff;
    DepthStencilDesc.FrontFace = DepthStencilOpFrontDesc;
    DepthStencilDesc.BackFace = DepthStencilOpBackDesc;

    renderer_->device()->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilStateSP);

    D3D11_BLEND_DESC BlendDesc = {};
    D3D11_RENDER_TARGET_BLEND_DESC& RTBlendDesc = BlendDesc.RenderTarget[0];
    BlendDesc.IndependentBlendEnable = false;
    BlendDesc.AlphaToCoverageEnable = false;
    RTBlendDesc.BlendEnable = true;
    RTBlendDesc.SrcBlend = D3D11_BLEND_ONE;
    RTBlendDesc.DestBlend = D3D11_BLEND_ONE;
    RTBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
    RTBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
    RTBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO; 
    RTBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    RTBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    renderer_->device()->CreateBlendState(&BlendDesc, &BlendState);

    renderer_->device()->CreateInputLayout(
        inputElements,
        3,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    viewport = {};
    viewport.Width = static_cast<float>(display_->screenWidth_);
    viewport.Height = static_cast<float>(display_->screenHeight_);
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1.0f;
}

void LightingStageShader::Activate()
{
    renderer_->Context()->ClearState();     

    //Расставим константные буферы, текстурки, мб что ещё
    renderer_->Context()->PSSetShaderResources(0, 1, GBuffer_->GetDiffuseSRV());
    renderer_->Context()->PSSetShaderResources(1, 1, GBuffer_->GetNormalSRV());
    renderer_->Context()->PSSetShaderResources(2, 1, GBuffer_->GetDepthSRV());
    renderer_->Context()->PSSetShaderResources(3, 1, GBuffer_->GetSpecularSRV());

    DirectX::SimpleMath::Vector3 CamPos = renderer_->camera()->position();
    ScrenToViewStructCB ScrenToView = {};
    ScrenToView.inverse_projection = renderer_->camera()->projection_matrix().Transpose().Invert();
    ScrenToView.screen_dimensions = ScreenDimensions;
    ScreenToViewCB->UpdateBuffer(&ScrenToView);
    renderer_->Context()->PSSetConstantBuffers(0, 1, ScreenToViewCB->buffer());

    renderer_->Context()->ClearDepthStencilView(DSV, /*D3D11_CLEAR_DEPTH|*/D3D11_CLEAR_STENCIL, 0, 1);
    //Unmark pixels

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    
    renderer_->Context()->RSSetState(rastStateUP);
    renderer_->Context()->OMSetDepthStencilState(DepthStencilStateUP, 1);

    renderer_->Context()->OMSetRenderTargets(0, nullptr, DSV);
    renderer_->Context()->RSSetViewports(1, &viewport);
    
    //renderer_->Context()->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 1);

    for (auto Light : LightComponent::Lights)
    {
        Light->draw_volume();
    }
    //Shade pixels
    
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    renderer_->Context()->RSSetState(rastStateSP);
    renderer_->Context()->OMSetDepthStencilState(DepthStencilStateSP, 1);
    
    //float blend_factor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    float blend_factor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    renderer_->Context()->OMSetBlendState(BlendState, blend_factor, 0xffffffff);

    renderer_->Context()->OMSetRenderTargets(1, renderer_->RTVMain(), DSV);
    renderer_->Context()->RSSetViewports(1, &viewport);
    
    //renderer_->Context()->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 1);

    for (auto Light : LightComponent::Lights)
    {
        ProvideLightData(Light);
        Light->draw_volume();
        //renderer_->DrawEverything();
    }
    //renderer_->Context()->OMSetRenderTargets(1)
}

void LightingStageShader::ProvideLightData(LightComponent* Light)
{
    LightDataCB->UpdateBuffer(Light->LightData());
    renderer_->Context()->PSSetConstantBuffers(1, 1, LightDataCB->buffer());
}

void LightingStageShader::ProvideMeshData(Mesh* mesh)
{
    constant_buffer_transform->UpdateBuffer(&mesh->transform_matricies_buffer_data);
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->buffer());
}
