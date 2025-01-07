#pragma once
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <chrono>
#include <vector>

//#include "PerspectiveCamera.h"
#include "Structs.h"

#define SHADOWMAP_WIDTH 800
#define SHADOWMAP_HEIGHT 800

class Water;
class LightComponent;
class MiniMapCamera;
class Mesh;
class PerspectiveCamera;
class ParticleSystem;
class LightingStageShader;
class GBufferShader;
class ShadowmapShader;
class MainShader;
class Display32;
class VertexShader;
class PixelShader;
//template <typename T>
//class AbstractBuffer<T>;

//struct TransformCBStruct;
class Drawable;
class Game;

class Renderer
{
public:
    Renderer(Display32& Display, PerspectiveCamera* camera);
    ~Renderer();
    Microsoft::WRL::ComPtr<ID3D11Device> device() {return device_;}
    ID3D11DeviceContext* Context() {return context_;}
    ID3D11RenderTargetView* const* RTVMain() {return &rtv_main;}
    ID3D11DepthStencilView** DSVMain() {return  &dsv_main;}
    std::vector<ParticleSystem*>* particle_systems() {return particle_systems_;}
    //DirectionalLightComponent* currentDirectionalLight() const {return  current_directional_light;}
    void RenderParticles(ID3D11RenderTargetView* rtv);
    void DrawEverything();
    void Render();
    void ProvideMeshData(Mesh* mesh);
    void ProvideParticleSystem(ParticleSystem* particle_system) {particle_systems_->push_back(particle_system);}
    void provideWater(Water* water) {water_ = water;}
    void SetRenderingState(RenderStateEnum State) {RenderingState = State;}
    ID3D11ShaderResourceView** GetDepthSRV() {return &SRVDepth;}
    GBufferShader& GBuffer() {return *gbuffer_shader_;}
    MainShader* main_shader() {return main_shader_;}
    PerspectiveCamera* camera();
    void SetMiniMapCamera (MiniMapCamera* mini_map_camera) {mini_map_camera_ = mini_map_camera;}
    RenderStateEnum RenderingState = MainRenderState;
private:
    ID3D11ShaderResourceView* SRVDepth;
    ID3D11Texture2D* RTDepthStencilTexture;
    //DirectionalLightComponent* current_directional_light;
    Microsoft::WRL::ComPtr<ID3D11Device> device_ = nullptr;
    ID3D11DeviceContext* context_ = nullptr;
    Display32& Display_;
    IDXGISwapChain* swapChain;
    MainShader* main_shader_;
    GBufferShader* gbuffer_shader_;
    LightingStageShader* lighting_stage_shader_;
    //ParticleSystem* particle_system_;
    //ShadowmapShader* shadowmap_shader_;
    ID3D11RenderTargetView* rtv_main;
    ID3D11DepthStencilView* dsv_main;
    ID3D11RenderTargetView* rtv_mini_map;
    ID3D11RasterizerState* rastState;
    PerspectiveCamera* camera_;
    MiniMapCamera* mini_map_camera_;
    //D3D11DepthStencilView* dsv;
    ID3D11ShaderResourceView* srv_mini_map;
    ID3D11Texture2D* depth_stencil_buffer_;
    ID3D11Texture2D* mini_map_buffer_;
    //ID3D11DepthStencilState* depth_stencil_state_;
    LightComponent* light_;
    std::vector<ParticleSystem*>* particle_systems_;
    Water* water_;
};
