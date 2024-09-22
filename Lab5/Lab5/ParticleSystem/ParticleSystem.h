#pragma once
#include <SimpleMath.h>

//#include "Drawable.h"
#include "AbstractBuffer.h"
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "StructuredBuffer.h"
#include "UnorderedAccessView.h"
#include "Updatable.h"

struct AttractorStruct;
struct CameraMatricies;
struct ParticleSimulationStuff;
struct InitIndirectComputeArgs1DConstantBuffer;
struct Count;
struct ParticleIndexElement;
struct Particle;
struct EmitterSphere;
class GeometryShader;
class SortLib;

class ComputeShader;
class VertexShader;
class Renderer;

class ParticleSystem : public Updatable/*, public Drawable*/
{
public:
    ParticleSystem(EmitterSphere& emitter_data, Renderer* renderer);
    void update() override;
    //void draw() override;
    void draw();
    void set_emitter_data(EmitterSphere* emitter_data) {emitter_data_ = emitter_data;}
    /*void set_simulate_particles_struct_(const SimulateParticlesStruct& simulate_particles_struct)
        {simulate_particles_struct_ = simulate_particles_struct;}*/
    virtual void Emit();
    virtual void Simulation();
    //virtual void Sort();
    virtual void initialize_system();
    virtual void KillSystem();
protected:
    virtual void InitializeParticle(int index);
    Particle* particleList; // particles for this emitter
    //int maxParticles; // maximum number of particles in total
    //int numParticles; // indicies of all free particles
    //DirectX::SimpleMath::Vector3 origin; // center of the particle system
    //float accumulatedTime;//track when waslast particle emitted
    //DirectX::SimpleMath::Vector3 force; // force (gravity, wind, etc.) acting on the system
    EmitterSphere* emitter_data_;
    //SimulateParticlesStruct simulate_particles_struct_;
    
private:
    Renderer* renderer_;
    
    UnorderedAccessView<UINT>* UAV_deadlist;
    UnorderedAccessView<Particle>* UAV_particle_pool;
    UnorderedAccessView<ParticleIndexElement>* UAV_alive_index[2];
    UnorderedAccessView<ParticleIndexElement>* UAV_alive_index_sorting[2];
    UnorderedAccessView<UINT>* UAV_indirect_dispatch_args[2];
    UnorderedAccessView<UINT>* UAV_indirect_draw_args;
    
    ConstantBuffer<Count>* CB_deadlist_count;
    ConstantBuffer<Count>* CB_alive_count;
    ConstantBuffer<EmitterSphere>* CB_emitter_sphere;
    ConstantBuffer<InitIndirectComputeArgs1DConstantBuffer>* CB_init_simulate_dispatch_args;
    ConstantBuffer<ParticleSimulationStuff>* CB_simulation;
    ConstantBuffer<CameraMatricies>* CB_camera_matricies;
    
    ComputeShader* CS_init_particles;
    ComputeShader* CS_emit_particles;
    ComputeShader* CS_init_indirect_compute_args;
    ComputeShader* CS_simulate_particles;

    VertexShader* VS_render_particles;
    GeometryShader* GS_render_particles;
    PixelShader* PS_render_particles;

    ID3D11ShaderResourceView* SRV_particle;
    ID3D11ShaderResourceView* SRV_alive_index[2];

    ID3D11BlendState* BS_additive;
    ID3D11DepthStencilState* DSS_none;
    ID3D11RasterizerState* RS_none;
    
    StructuredBuffer<AttractorStruct>* SB_attractors;
    
    SortLib* sort;

    int maxParticles = 1000*1024;
    float emissionRate = 10000.0f;
    float emissionRateAccumulation = 0.0f;
    int current_buffer = 0;
};
