#include "Object3D.h"

#include <random>

#include "ConstantBuffer.h"


Object3D::Object3D(Renderer* renderer, Object3D* parent, DirectX::XMFLOAT4 color) : Drawable(renderer) , parent_(parent), color_(color)
{
    constant_buffer_transform = new ConstantBuffer<DirectX::SimpleMath::Matrix>(renderer_);
    if (random_color)
    {
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::default_random_engine eng(seed);
        std::uniform_real_distribution<> color_distr(0, 1);

        color_ = DirectX::XMFLOAT4(static_cast<float>(color_distr(eng)),
            static_cast<float>(color_distr(eng)), static_cast<float>(color_distr(eng)), 1);
    }
    //constant_buffer_transform = new ConstantBuffer<ConstantBufferDataStruct>(renderer_);
    //GenerateWorldMatricies();
    //all_transform_matrix = local_world_matrix_ * renderer_->camera()->view_matrix().Transpose() * renderer_->camera()->perspective_matrix().Transpose();
}

void Object3D::update()
{
    Object3D* current_parent = parent_;

    all_transform_matrix = renderer_->camera()->perspective_matrix().Transpose() *
        renderer_->camera()->view_matrix().Transpose();
    
    while (current_parent)
    {
         all_transform_matrix *= current_parent->GenerateLocalWorldMatrixReturn().Transpose();
        //parent_transform_matrix = current_parent->global_world_matrix().Transpose() * parent_transform_matrix;
        current_parent = current_parent->parent();
    }

    all_transform_matrix *= local_world_matrix_.Transpose();
    
    constant_buffer_transform->UpdateBuffer(&all_transform_matrix);

    //rotation_.Normalize();
    GenerateWorldMatricies();
}

void Object3D::draw()
{
    renderer_->Context()->VSSetConstantBuffers(0, 1, constant_buffer_transform->buffer());
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
    local_world_matrix_ = XMMatrixScalingFromVector(scale_);
    local_world_matrix_ *= XMMatrixRotationQuaternion(rotation_);
    local_world_matrix_ *= XMMatrixTranslationFromVector(location_);
    
    /*local_world_matrix_ = DirectX::SimpleMath::Matrix::CreateScale(scale_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateTranslation(location_);*/
}

DirectX::SimpleMath::Matrix Object3D::GenerateLocalWorldMatrixReturn()
{
    DirectX::SimpleMath::Matrix local_matrix_ = XMMatrixScalingFromVector(scale_);
    //local_matrix_ *= XMMatrixRotationQuaternion(rotation_);
    local_matrix_ *= XMMatrixTranslationFromVector(location_);

    return local_matrix_;
    /*local_world_matrix_ = DirectX::SimpleMath::Matrix::CreateScale(scale_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation_);
    local_world_matrix_ *= DirectX::SimpleMath::Matrix::CreateTranslation(location_);*/
}

void Object3D::GenerateGlobalWorldMatrix()
{
    const DirectX::SimpleMath::Matrix translated_world_matrix_ = DirectX::SimpleMath::Matrix::CreateWorld
    (location_, local_world_matrix_.Forward(), local_world_matrix_.Up());
    //(location_, DirectX::XMFLOAT3(0,0,1), DirectX::XMFLOAT3(0,1,0));
    //const DirectX::SimpleMath::Matrix rotated_world_matrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation_);
    //global_world_matrix_ = translated_world_matrix_*rotated_world_matrix;
    global_world_matrix_ = translated_world_matrix_;
}
