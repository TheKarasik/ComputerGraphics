#include "ParticleSystem.h"

#include <random>

#include "ComputeShader.h"
#include "GeometryShader.h"
#include "IndexBuffer.h"
#include "ParticleAttractor.h"
#include "PixelShader.h"
#include "Renderer.h"
#include "SortLib.h"
#include "UnorderedAccessView.h"
#include "VertexShader.h"
#include "Structs.h"
#include "PerspectiveCamera.h"

std::vector<AttractorStruct> ParticleAttractor::Attractors;

ParticleSystem::ParticleSystem(EmitterSphere& emitter_data, Renderer* renderer) :
    renderer_(renderer),
    //Drawable(renderer),
    //simulate_particles_struct_(simulate_particles_struct),
    emitter_data_(&emitter_data)
{
    //vertex_shader_ = new VertexShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyVertexShader.hlsl");

    UAV_particle_pool = new UnorderedAccessView<Particle>(renderer_, maxParticles);
    UAV_particle_pool->UAV_flag = static_cast<D3D11_BUFFER_UAV_FLAG>(0);
    UAV_particle_pool->CreateBuffer();

    D3D11_RASTERIZER_DESC rasterizer_desc;
    ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
    rasterizer_desc.CullMode = D3D11_CULL_NONE;
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.MultisampleEnable = TRUE;
    renderer_->device()->CreateRasterizerState(&rasterizer_desc, &RS_none);

    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
    ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
    depth_stencil_desc.DepthEnable = FALSE;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

    depth_stencil_desc.StencilEnable = FALSE;
    depth_stencil_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depth_stencil_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

    depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;
    renderer_->device()->CreateDepthStencilState(&depth_stencil_desc, &DSS_none);

    D3D11_BLEND_DESC blend_desc;
    ZeroMemory(&blend_desc, sizeof(blend_desc));
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    renderer_->device()->CreateBlendState(&blend_desc, &BS_additive);

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Buffer.FirstElement = 0;
    SRVDesc.Buffer.NumElements = maxParticles;
    renderer_->device()->CreateShaderResourceView(UAV_particle_pool->buffer(),&SRVDesc, &SRV_particle);
    //renderer_->device()->CreateShaderResourceView(UAV_particle_pool->buffer_.Get(), &SRVDesc, &SRV_particle);
    
    UAV_deadlist = new UnorderedAccessView<UINT>(renderer_, maxParticles);
    UAV_deadlist->UAV_flag = D3D11_BUFFER_UAV_FLAG_APPEND;
    UAV_deadlist->CreateBuffer();

    for (int i = 0; i < 2; i++)
    {
        UAV_alive_index[i] = new UnorderedAccessView<ParticleIndexElement>(renderer_, maxParticles);
        UAV_alive_index[i]->UAV_flag = D3D11_BUFFER_UAV_FLAG_APPEND;
        UAV_alive_index[i]->CreateBuffer();

        UAV_alive_index_sorting[i] = new UnorderedAccessView<ParticleIndexElement>(renderer_, UAV_alive_index[i]);
        UAV_alive_index_sorting[i]->CreateUAV();

        UAV_indirect_dispatch_args[i] = new UnorderedAccessView<UINT>(renderer_, 3);
        UAV_indirect_dispatch_args[i]->misc_flag_ = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
        UAV_indirect_dispatch_args[i]->UAV_format = DXGI_FORMAT_R32_UINT;
        UAV_indirect_dispatch_args[i]->CreateBuffer();

        renderer_->device()->CreateShaderResourceView(UAV_alive_index[i]->buffer(), &SRVDesc, &SRV_alive_index[i]);
    }
    UAV_indirect_draw_args = new UnorderedAccessView<UINT>(renderer_, 5);
    UAV_indirect_draw_args->misc_flag_ = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    UAV_indirect_draw_args->UAV_format = DXGI_FORMAT_R32_UINT;
    UAV_indirect_draw_args->CreateBuffer();

    CB_deadlist_count = new ConstantBuffer<Count>(renderer_);
    CB_alive_count = new ConstantBuffer<Count>(renderer_);

    CB_emitter_sphere = new ConstantBuffer<EmitterSphere>(renderer_);

    CS_emit_particles = new ComputeShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyEmitParticlesShader.hlsl");

    CB_init_simulate_dispatch_args = new ConstantBuffer<InitIndirectComputeArgs1DConstantBuffer>(renderer_);

    CS_init_indirect_compute_args = new ComputeShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyInitIndirectComputeArgsShader.hlsl");

    CB_simulation = new ConstantBuffer<ParticleSimulationStuff>(renderer_);

    SB_attractors = new StructuredBuffer<AttractorStruct>(renderer_, ParticleAttractor::Attractors.size());

    CS_simulate_particles = new ComputeShader(*renderer_, L"./Shaders/ParticleSystemShaders/MySimulateParticlesShader.hlsl");

    sort = new SortLib();
    sort->init(renderer_->device().Get(), renderer_->Context());

    VS_render_particles = new VertexShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyRenderParticlesShader.hlsl");
    GS_render_particles = new GeometryShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyRenderParticlesShader.hlsl");
    PS_render_particles = new PixelShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyRenderParticlesShader.hlsl");

    CB_camera_matricies = new ConstantBuffer<CameraMatricies>(renderer_);

    CS_init_particles = new ComputeShader(*renderer_, L"./Shaders/ParticleSystemShaders/MyInitParticlesShader.hlsl");
 
    //CB_simulate_particles = new ConstantBuffer<SimulateParticlesStruct>(renderer_);
}

void ParticleSystem::update()
{
    emissionRateAccumulation += emissionRate * elapsed;
    if (emissionRateAccumulation > 1.0f)
    {
        double integer_part = 0.0f;
        float fraction = modf(emissionRateAccumulation, &integer_part);
        emitter_data_->max_spawn = integer_part;
        emissionRateAccumulation = fraction;
    }
    else
    {
        emitter_data_->max_spawn = 0;
    }
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::default_random_engine eng(seed);
    std::uniform_real_distribution<> normal_distr(-1, 1);
    emitter_data_->rng_seed = normal_distr(eng);
    
    ParticleSimulationStuff d = {};
    d.elapsed_time = elapsed;
    d.attractorc_num = ParticleAttractor::Attractors.size();
    d.camPosition = renderer_->camera()->position();
    CB_simulation->UpdateBuffer(&d);

    if (ParticleAttractor::Attractors.data()) SB_attractors->UpdateBuffer(ParticleAttractor::Attractors.data());

    CameraMatricies cm = {};
    cm.view = renderer_->camera()->view_matrix().Transpose();
    cm.proj = renderer_->camera()->projection_matrix().Transpose();
    CB_camera_matricies->UpdateBuffer(&cm);
}

void ParticleSystem::draw()
{
    Emit();
    Simulation();
    
    //sort
    //sort->run(maxParticles, *UAV_alive_index_sorting[current_buffer]->UAV(), *CB_alive_count->p_buffer());

    renderer_->Context()->VSSetShader(VS_render_particles->vertex_shader(), nullptr, 0);
    renderer_->Context()->GSSetShader(GS_render_particles->geometry_shader(), nullptr, 0);  
    renderer_->Context()->PSSetShader(PS_render_particles->pixel_shader(), nullptr, 0);

    ID3D11Buffer* nullVertexBuffer = nullptr;
    UINT stride = 0;
    UINT offset = 0;
    renderer_->Context()->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
    renderer_->Context()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    renderer_->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    ID3D11ShaderResourceView* vertexShaderSRVs[] = { SRV_particle, SRV_alive_index[current_buffer]};
    renderer_->Context()->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);
    renderer_->Context()->GSSetConstantBuffers(0, 1, CB_camera_matricies->p_buffer());
    renderer_->Context()->VSSetConstantBuffers(3, 1, CB_alive_count->p_buffer());

    const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    renderer_->Context()->OMSetBlendState(BS_additive, blendFactor, 0xffffffff);
    renderer_->Context()->OMSetDepthStencilState(DSS_none, 0);
    renderer_->Context()->RSSetState(RS_none);
    renderer_->Context()->OMSetRenderTargets(1, renderer_->RTVMain(), *renderer_->DSVMain());

    renderer_->Context()->DrawInstancedIndirect(*UAV_indirect_draw_args->p_buffer(), 0);

    ZeroMemory(vertexShaderSRVs, sizeof(vertexShaderSRVs));
    renderer_->Context()->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);
    renderer_->Context()->GSSetShader(nullptr, nullptr, 0);
}

void ParticleSystem::Emit()
{
    //Emit particles
    UINT initialCounts[] = { (UINT)-1, (UINT)-1, (UINT)-1, (UINT)-1 };
    ID3D11UnorderedAccessView* uavs[] = {*UAV_deadlist->UAV(), *UAV_particle_pool->UAV(), *UAV_alive_index[current_buffer]->UAV(), *UAV_indirect_dispatch_args[current_buffer]->UAV()};
    renderer_->Context()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, initialCounts);

    renderer_->Context()->CopyStructureCount(*CB_deadlist_count->p_buffer(), 0, *UAV_deadlist->UAV());
    renderer_->Context()->CSSetConstantBuffers(2, 1, CB_deadlist_count->p_buffer());

    emitter_data_->rng_seed = static_cast<float>(std::rand());
    CB_emitter_sphere->UpdateBuffer(emitter_data_);
    renderer_->Context()->CSSetConstantBuffers(4, 1, CB_emitter_sphere->p_buffer());
    renderer_->Context()->CSSetShader(CS_emit_particles->compute_shader(), nullptr, 0);

    float thread_group_x = ((emitter_data_->max_spawn + (256 - 1)) & ~(256 - 1)) / 256;
    renderer_->Context()->Dispatch(thread_group_x, 1, 1);
    renderer_->Context()->CSSetShader(nullptr, nullptr, 0);

    ZeroMemory(uavs, sizeof(uavs));
    renderer_->Context()->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

    //init indirect dispatch args (align)
    InitIndirectComputeArgs1DConstantBuffer init_simulate_dispatch_args_data = InitIndirectComputeArgs1DConstantBuffer{256.0f};
    CB_init_simulate_dispatch_args->UpdateBuffer(&init_simulate_dispatch_args_data);
    renderer_->Context()->CSSetConstantBuffers(4, 1, CB_init_simulate_dispatch_args->p_buffer());
    
    UINT initialCount[] = { (UINT)-1 };
    ID3D11UnorderedAccessView* UAV_dummy[1] = {nullptr};
    renderer_->Context()->CSSetUnorderedAccessViews(0, 1, UAV_indirect_dispatch_args[current_buffer]->UAV(), initialCount);
    renderer_->Context()->CSSetShader(CS_init_indirect_compute_args->compute_shader(), nullptr, 0);
    renderer_->Context()->Dispatch(1,1,1);
    renderer_->Context()->CSSetShader(nullptr, nullptr, 0); 
    renderer_->Context()->CSSetUnorderedAccessViews(0, 1, UAV_dummy, nullptr); 

}

void ParticleSystem::Simulation()
{
    //simulation
    UINT initialCount[] = { (UINT)-1 };
    ID3D11UnorderedAccessView* UAV_dummy[6] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    ID3D11ShaderResourceView* SRV_dummy[1] = {nullptr};
    renderer_->Context()->CSSetConstantBuffers(0, 1, CB_simulation->p_buffer());
    renderer_->Context()->CSSetUnorderedAccessViews(0, 1, UAV_indirect_draw_args->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(2, 1, UAV_deadlist->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(3, 1, UAV_particle_pool->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(4, 1, UAV_alive_index[current_buffer]->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(5, 1, UAV_indirect_dispatch_args[(current_buffer+1)%2]->UAV(), initialCount);

    initialCount[0] = 0;
    renderer_->Context()->CSSetUnorderedAccessViews(1, 1, UAV_alive_index[(current_buffer+1)%2]->UAV(), initialCount);
    renderer_->Context()->CSSetShaderResources(0, 1, SB_attractors->SRV());
    renderer_->Context()->CSSetShader(CS_simulate_particles->compute_shader(), nullptr, 0);
    
    renderer_->Context()->DispatchIndirect(*UAV_indirect_dispatch_args[current_buffer]->p_buffer(), 0);
    
    renderer_->Context()->CSSetShader(nullptr, nullptr, 0);
    renderer_->Context()->CSSetUnorderedAccessViews(0, 6, UAV_dummy, nullptr);
    renderer_->Context()->CSSetShaderResources(0, 1, SRV_dummy);
    
    renderer_->Context()->CopyStructureCount(*CB_alive_count->p_buffer(), 0, *UAV_alive_index[(current_buffer+1)%2]->UAV());

    current_buffer = (current_buffer + 1) % 2;


}

/*void ParticleSystem::Sort()
{
    
    for(int subArraySize=2; subArraySize<ArraySize; subArraySize*=2)
    {
        for(int compareDist=subArraySize/2; compareDist>0; compareDist/=2)
        {
            // Begin: GPU part of the sort
            
            // End: GPU part of the sort
        }	
    }

}*/
 
void ParticleSystem::initialize_system()
{
    UINT initialCount[] = { 0 };
    ID3D11UnorderedAccessView* UAV_dummy[3] = {nullptr, nullptr, nullptr};
    renderer_->Context()->CSSetUnorderedAccessViews(0, 1, UAV_deadlist->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(1, 1, UAV_particle_pool->UAV(), initialCount);
    renderer_->Context()->CSSetUnorderedAccessViews(2, 1, UAV_indirect_dispatch_args[current_buffer]->UAV(), initialCount);
    renderer_->Context()->CSSetShader(CS_init_particles->compute_shader(), nullptr, 0);
    renderer_->Context()->Dispatch(((maxParticles + (256 - 1)) & ~(256 - 1)) / 256, 1, 1);
    renderer_->Context()->CSSetShader(nullptr, nullptr, 0);
    renderer_->Context()->CSSetUnorderedAccessViews(0, 3, UAV_dummy, nullptr);
}

void ParticleSystem::KillSystem()
{
}

void ParticleSystem::InitializeParticle(int index)
{
}
