#include "WaterShader.h"

#include "DirectionalLightComponent.h"
#include "FileTexture.h"
#include "PixelShader.h"
#include "ReflectionShader.h"
#include "VertexShader.h"
#include "Water.h"

WaterShader::WaterShader(Renderer* renderer) : renderer_(renderer)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
    constant_buffer_water_additional_data = new ConstantBuffer<WaterAdditionalDataStruct>(renderer_);
    constant_buffer_water_buffer = new ConstantBuffer<WaterBuffer>(renderer_);
    
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));
    textureDesc.Width = SHADOWMAP_WIDTH;
    textureDesc.Height = SHADOWMAP_HEIGHT;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&textureDesc, NULL, &RTTextureReflection)) )
     return;

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateRenderTargetView(RTTextureReflection, &renderTargetViewDesc, &RTVReflection)) )
        return ;

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;	
    if( FAILED(renderer_->device()->CreateShaderResourceView(RTTextureReflection, &shaderResourceViewDesc, &SRVReflection_)) )
        return ;

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if( FAILED(renderer_->device()->CreateSamplerState(&samplerDesc, &SamplerState_)) )
        return ;

    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
    depthBufferDesc.Width = SHADOWMAP_WIDTH;
    depthBufferDesc.Height = SHADOWMAP_HEIGHT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    //depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE ;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;
    if( FAILED(renderer_->device()->CreateTexture2D(&depthBufferDesc, NULL, &DSTextureReflection)) )
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;
    if( FAILED(renderer_->device()->CreateDepthStencilView(DSTextureReflection, &depthStencilViewDesc, &DSVReflection)) )
        return ;

        D3D11_DEPTH_STENCIL_DESC depth_stencil_desc_default;
    ZeroMemory(&depth_stencil_desc_default, sizeof(depth_stencil_desc_default));
    depth_stencil_desc_default.DepthEnable = TRUE;
    depth_stencil_desc_default.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc_default.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    depth_stencil_desc_default.StencilEnable = FALSE;
    depth_stencil_desc_default.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc_default.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depth_stencil_desc_default.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc_default.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_default.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_default.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depth_stencil_desc_default.BackFace = depth_stencil_desc_default.FrontFace;

    renderer_->device()->CreateDepthStencilState(&depth_stencil_desc_default, &DSS_depth_default);

    D3D11_BLEND_DESC blend_desc_opaque;
    ZeroMemory(&blend_desc_opaque, sizeof(blend_desc_opaque));
    blend_desc_opaque.AlphaToCoverageEnable = FALSE;
    blend_desc_opaque.IndependentBlendEnable = FALSE;
    blend_desc_opaque.RenderTarget[0].BlendEnable = TRUE;
    blend_desc_opaque.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blend_desc_opaque.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc_opaque.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    blend_desc_opaque.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blend_desc_opaque.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc_opaque.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc_opaque.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    renderer_->device()->CreateBlendState(&blend_desc_opaque, &BS_opaque);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc_none;
    ZeroMemory(&depth_stencil_desc_none, sizeof(depth_stencil_desc_none));
    depth_stencil_desc_none.DepthEnable = FALSE;
    depth_stencil_desc_none.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc_none.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    depth_stencil_desc_none.StencilEnable = FALSE;
    depth_stencil_desc_none.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc_none.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depth_stencil_desc_none.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc_none.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_none.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_none.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depth_stencil_desc_none.BackFace = depth_stencil_desc_none.FrontFace;
    renderer_->device()->CreateDepthStencilState(&depth_stencil_desc_none, &DSS_none);

    D3D11_BLEND_DESC blend_desc_additive;
    ZeroMemory(&blend_desc_additive, sizeof(blend_desc_additive));
    blend_desc_additive.AlphaToCoverageEnable = FALSE;
    blend_desc_additive.IndependentBlendEnable = FALSE;
    blend_desc_additive.RenderTarget[0].BlendEnable = TRUE;
    blend_desc_additive.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc_additive.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blend_desc_additive.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blend_desc_additive.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blend_desc_additive.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc_additive.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc_additive.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    renderer_->device()->CreateBlendState(&blend_desc_additive, &BS_additive);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc_depth_read;
    ZeroMemory(&depth_stencil_desc_depth_read, sizeof(depth_stencil_desc_depth_read));
    depth_stencil_desc_depth_read.DepthEnable = TRUE;
    depth_stencil_desc_depth_read.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc_depth_read.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    depth_stencil_desc_depth_read.StencilEnable = FALSE;
    depth_stencil_desc_depth_read.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc_depth_read.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depth_stencil_desc_depth_read.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc_depth_read.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_depth_read.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc_depth_read.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depth_stencil_desc_depth_read.BackFace = depth_stencil_desc_depth_read.FrontFace;

    renderer_->device()->CreateDepthStencilState(&depth_stencil_desc_depth_read, &DSS_depth_read);

    D3D11_BLEND_DESC blend_desc_not_premultiplied;
    ZeroMemory(&blend_desc_not_premultiplied, sizeof(blend_desc_not_premultiplied));
    blend_desc_not_premultiplied.AlphaToCoverageEnable = FALSE;
    blend_desc_not_premultiplied.IndependentBlendEnable = FALSE;
    blend_desc_not_premultiplied.RenderTarget[0].BlendEnable = TRUE;
    blend_desc_not_premultiplied.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc_not_premultiplied.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blend_desc_not_premultiplied.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc_not_premultiplied.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc_not_premultiplied.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc_not_premultiplied.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc_not_premultiplied.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    renderer_->device()->CreateBlendState(&blend_desc_not_premultiplied, &BS_not_premultiplied);
    
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
        D3D11_INPUT_ELEMENT_DESC{
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0},
    };
    
    //pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyShadowmapShader.hlsl");
    vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/MyWaterShader.hlsl");
    pixel_shader_ = new PixelShader(*renderer_, L"./Shaders/MyWaterShader.hlsl");
    
    renderer_->device()->CreateInputLayout(
        inputElements,
        2,
        vertex_shader_->byte_code()->GetBufferPointer(),
        vertex_shader_->byte_code()->GetBufferSize(),
        &layout);

    viewport_shadowmap = {};
    viewport_shadowmap.Width = 800;
    viewport_shadowmap.Height = 800;
    viewport_shadowmap.TopLeftX = 0;
    viewport_shadowmap.TopLeftY = 0;
    viewport_shadowmap.MinDepth = 0;
    viewport_shadowmap.MaxDepth = 1.0f;
    
    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    
    renderer_->device()->CreateRasterizerState(&rastDesc, &rastState);
}

void WaterShader::Activate()
{
    renderer_->Context()->ClearState();

    renderer_->Context()->IASetInputLayout(layout);
    renderer_->Context()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    renderer_->Context()->VSSetShader(vertex_shader_->vertex_shader(), nullptr, 0);
    renderer_->Context()->PSSetShader(pixel_shader_->pixel_shader(), nullptr, 0);

    WaterAdditionalDataStruct wadc;
    wadc.reflection = water_->reflection_view_matrix().Transpose();
    /*wadc.normalMapTiling.x = 0.01f;
    wadc.normalMapTiling.y = 0.02f;*/
    wadc.normalMapTiling.x = 1.0f;
    wadc.normalMapTiling.y = 1.0f;
    float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    WaterBuffer wb;
    wb.refractionTint = DirectX::SimpleMath::Vector4(0.0f, 0.8f, 1.0f, 1.0f);
    wb.lightDirection = DirectionalLightComponent::DirectionalLightData()->DirectionWS;
    wb.waterTranslation = water_->water_translation();
    wb.reflectRefractScale = 0.03f;
    wb.specularShininess = 200.0f;

    constant_buffer_transform->UpdateBuffer(water_->GetMesh()->transform_matricies_buffer_data);
    constant_buffer_water_additional_data->UpdateBuffer(&wadc);
    constant_buffer_water_buffer->UpdateBuffer(&wb);

    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->p_buffer());
    renderer_->Context()->VSSetConstantBuffers(1, 1, constant_buffer_water_additional_data->p_buffer());
    renderer_->Context()->PSSetConstantBuffers(2, 1, constant_buffer_water_buffer->p_buffer());

    renderer_->Context()->PSSetShaderResources(0, 1, water_->reflection_shaders()[0]->SRVReflection());
    renderer_->Context()->PSSetShaderResources(1, 1, water_->reflection_shaders()[1]->SRVReflection());
    renderer_->Context()->PSSetShaderResources(2, 1, water_->GetMesh()->normal_texture()->srv());

    renderer_->Context()->PSSetSamplers(0, 1, water_->GetMesh()->normal_texture()->sampler());

    renderer_->Context()->OMSetRenderTargets(1, renderer_->RTVMain(), *renderer_->DSVMain());
    renderer_->Context()->RSSetState(rastState);
    renderer_->Context()->RSSetViewports(1, &viewport_shadowmap);
    renderer_->Context()->ClearDepthStencilView(DSVReflection, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1, 0);
    renderer_->Context()->ClearRenderTargetView(RTVReflection, color);

    const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    switch (blendmode)
    {
    case 0:
        renderer_->Context()->OMSetBlendState(BS_opaque, blendFactor, 0xffffffff);
        renderer_->Context()->OMSetDepthStencilState(DSS_depth_default, 0);
        break;
    case 1:
        renderer_->Context()->OMSetBlendState(BS_not_premultiplied, blendFactor, 0xffffffff);
        renderer_->Context()->OMSetDepthStencilState(DSS_depth_read, 0);
        break;
    case 2:
        renderer_->Context()->OMSetBlendState(BS_additive, blendFactor, 0xffffffff);
        //renderer_->Context()->OMSetDepthStencilState(DSS_none, 0);
        renderer_->Context()->OMSetDepthStencilState(DSS_depth_read, 0);
        break;   
    }

    water_->GetMesh()->set_drawability(true);
    water_->GetMesh()->draw();
    water_->GetMesh()->set_drawability(false);

}
