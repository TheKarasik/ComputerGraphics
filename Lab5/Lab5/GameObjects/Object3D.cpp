#include "Object3D.h"

#include <random>

#include "ConstantBuffer.h"


Object3D::Object3D(Renderer* renderer, DirectX::SimpleMath::Matrix transformation, Object3D* parent) : Drawable(renderer),
    parent_(parent), world_transformation_matrix_(transformation)
{
    constant_buffer_transform = new ConstantBuffer<ConstantBufferTransformMatricies>(renderer_);
}

void Object3D::set_local_transformation(DirectX::SimpleMath::Matrix m)
{
    DirectX::SimpleMath::Vector3 s;
    DirectX::SimpleMath::Quaternion q;
    DirectX::SimpleMath::Vector3 t;
    if (m.Decompose(s, q, t))
    {
        /*t.Normalize();
        t*=2;*/
        //s*=0.1;
        set_location(t);
        set_rotation(q);
        set_scale(s);
        GenerateWorldMatricies();
    }
}

void Object3D::moveForBackwards(float linear_step, float angular_step)
{
    this->add_location(DirectX::SimpleMath::Vector3(0,0,1)*linear_step);
    this->add_rotation(DirectX::XMFLOAT3(1,0,0), angular_step);
}

void Object3D::moveRightLeft(float linear_step, float angular_step)
{
    this->add_location(DirectX::SimpleMath::Vector3(-1,0,0)*linear_step);
    this->add_rotation(DirectX::XMFLOAT3(0,0,1), angular_step);
}

void Object3D::update()
{
    transform_matricies_buffer_data.projection_view = renderer_->camera()->perspective_matrix().Transpose() * renderer_->camera()->view_matrix().Transpose();

    GenerateGlobalWorldMatrix();

    transform_matricies_buffer_data.world = world_transformation_matrix_.Transpose();

    DirectX::SimpleMath::Vector3 cam_pos = renderer_->camera()->position();
    transform_matricies_buffer_data.camera_position = DirectX::SimpleMath::Vector4(cam_pos.x, cam_pos.y, cam_pos.z, 1);
    transform_matricies_buffer_data.IsPerspective = renderer_->IsRenderingMain;
    
    constant_buffer_transform->UpdateBuffer(&transform_matricies_buffer_data);

    //rotation_.Normalize();
    GenerateWorldMatricies();
}

void Object3D::draw()
{
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->buffer());
    renderer_->Context()->PSSetConstantBuffers(0, 1, constant_buffer_transform->buffer());
}

int Object3D::GetParentLevel()
{
    int level = 0;
    Object3D* current_parent = parent_;
    while (current_parent)
    {
        level++;
        current_parent = current_parent->parent();
    }
    return level;
}

void Object3D::GenerateWorldMatricies()
{
    rotation_.Normalize();
    GenerateLocalWorldMatrix();
    GenerateGlobalWorldMatrix();
}

void Object3D::GenerateLocalWorldMatrix()
{
    local_transformation_matrix_ = XMMatrixScalingFromVector(scale_);
    local_transformation_matrix_ *= XMMatrixRotationQuaternion(rotation_);
    local_transformation_matrix_ *= XMMatrixTranslationFromVector(location_);
    
    /*local_world_matrix_ = DirectX::SimpleMath::Matrix::CreateScale(scale_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateTranslation(location_);*/
}

void Object3D::GenerateGlobalWorldMatrix()
{
    Object3D* current_parent = parent_;
    world_transformation_matrix_ = local_transformation_matrix_;
    compostion_transformation_matrix_ = DirectX::SimpleMath::Matrix::Identity;
    while (current_parent)
    {
        compostion_transformation_matrix_ *= current_parent->local_transformation_matrix_;
        //parent_transform_matrix = current_parent->global_world_matrix().Transpose() * parent_transform_matrix;
        current_parent = current_parent->parent();
    }
    world_transformation_matrix_ *= compostion_transformation_matrix_;

    //world_transformation_matrix_ = world_transformation_matrix_;
}


