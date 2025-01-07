#pragma once
#include "Mesh.h"

class WaterShader;
class ReflectionShader;
class OrthographicCamera;

class Water : public Updatable
{
public:
    Water(Renderer* renderer);
    void Initialize();
    void render();
    void update() override;
    void provide_water_mesh(Mesh* water_mesh) {water_mesh_ = water_mesh;}
    Mesh* GetMesh() {return water_mesh_;}
    ReflectionShader** reflection_shaders() {return reflection_shaders_;}
    DirectX::SimpleMath::Matrix reflection_view_matrix() {return reflection_view_matrix_;}
    ReflectionShader* GetCurrentReflectionShader() {return reflection_shaders_[current_reflection_];}
    void SetGround(Mesh* ground) {ground_ = ground;}
    Mesh* ground() {return ground_;}
    float water_translation() {return water_translation_;}
    Mesh* water_mesh_;
private:
    //PerspectiveCamera* reflection_camera_;
    Renderer* renderer_;
    long namecheck1 = 0xBAADF00D;
    DirectX::SimpleMath::Matrix reflection_view_matrix_;
    ReflectionShader* reflection_shaders_[2];
    WaterShader* water_shader_;
    Mesh* ground_;
    
    float water_translation_ = 0;
    int current_reflection_ = 0;
    long namecheck2 = 0xBAADF00D;
};
