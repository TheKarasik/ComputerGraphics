#include "Water.h"

#include "OrthographicCamera.h"
#include "ReflectionShader.h"
#include "WaterShader.h"

Water::Water(Renderer* renderer) : renderer_(renderer)
{
}

void Water::Initialize()
{
    //IsDrawable = false;
    for(int i = 0; i < 2; i++)
    {
        reflection_shaders_[i] = new ReflectionShader(renderer_);
        reflection_shaders_[i]->SetWater(this);
    }
    water_shader_ = new WaterShader(renderer_);
    water_shader_->ProvideWater(this);
}

void Water::render()
{
    DirectX::SimpleMath::Vector3 reflected_position = DirectX::SimpleMath::Vector3(
        renderer_->camera()->position().x,
        -renderer_->camera()->position().y + (water_mesh_->location().y * 2.0f * 1),
        renderer_->camera()->position().z);
    DirectX::SimpleMath::Vector3 reflected_focus_position = DirectX::SimpleMath::Vector3(
        renderer_->camera()->focus_position().x,
        -renderer_->camera()->focus_position().y + (water_mesh_->location().y * 2.0f * 1),
        renderer_->camera()->focus_position().z);
    reflection_view_matrix_ = XMMatrixLookAtLH(
        reflected_position,
        //reflected_focus_position,
        renderer_->camera()->focus_position(),
        renderer_->camera()->up_direction());
    renderer_->SetRenderingState(ReflectionState);
    current_reflection_ = 0;
    reflection_shaders_[0]->Activate(Refration);
    //reflection_shaders_[0]->Activate(renderer_->camera()->view_matrix(), DirectX::SimpleMath::Vector4(0.0f, -1.0f, 0.0f, water_mesh_->location().y));
    current_reflection_ = 1;
    reflection_shaders_[1]->Activate(Reflection);
    //reflection_shaders_[1]->Activate(reflection_view_matrix_, DirectX::SimpleMath::Vector4(0.0f, 1.0f, 0.0f, -water_mesh_->location().y));
    water_shader_->Activate();
    
}

void Water::update()
{
    //Mesh::update();
    water_translation_ = water_translation_ > 1.0f ? water_translation_ - 1.0f : water_translation_ + 0.03f;
}


