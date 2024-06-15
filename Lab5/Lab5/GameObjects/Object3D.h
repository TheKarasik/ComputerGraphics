#pragma once
#include "ConstantBuffer.h"
#include "Drawable.h"
#include "Updatable.h"
#include "GeometricPrimitive.h"

//typedef ConstantBuffer<TransformCBStruct> TransformCB;
/*template <typename T>
class ConstantBuffer;*/

class Object3D : public Updatable, public Drawable
{
public:
    struct ConstantBufferTransformMatricies
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Matrix projection_view;
        DirectX::SimpleMath::Vector4 camera_position;
        alignas (16) bool IsPerspective;
    } transform_matricies_buffer_data;
    DirectX::SimpleMath::Matrix all_transform_matrix;
    
    Object3D(Renderer* renderer, DirectX::SimpleMath::Matrix transformation = DirectX::SimpleMath::Matrix::Identity,
        Object3D* parent = nullptr);
    
    void set_location(DirectX::XMFLOAT3 location) {location_ = location;}
    void set_rotation(DirectX::SimpleMath::Quaternion rotation) {rotation_ = rotation;}
    void set_rotation(DirectX::XMFLOAT3 axis, float angle){rotation_ = DirectX::SimpleMath::Quaternion
        (axis.x * sin(angle/2), axis.y * sin(angle/2),axis.z * sin(angle/2), cos(angle/2));}
    void set_scale(DirectX::XMFLOAT3 scale) {scale_ = scale;}

    void add_location(DirectX::XMFLOAT3 location) {location_ += location;}
    void add_rotation(DirectX::SimpleMath::Quaternion rotation) {rotation *= rotation;}
    void add_rotation(DirectX::XMFLOAT3 axis, float angle){rotation_ *= DirectX::SimpleMath::Quaternion
        (axis.x * sin(angle/2), axis.y * sin(angle/2),axis.z * sin(angle/2), cos(angle/2));}
    void add_scale(DirectX::XMFLOAT3 scale) {scale_ *= scale;}

    DirectX::SimpleMath::Vector3 forward(){return world_transformation_matrix_.Forward();}
    DirectX::SimpleMath::Vector3 up(){return world_transformation_matrix_.Up();}

    void set_local_transformation(DirectX::SimpleMath::Matrix m);

    void moveForBackwards(float linear_step, float angular_step);
    void moveRightLeft(float linear_step, float angular_step);

    DirectX::SimpleMath::Vector3 location() {return location_;}
    DirectX::SimpleMath::Quaternion rotation() {return rotation_;}
    DirectX::XMFLOAT3 scale() {return scale_;}

    void set_parent(Object3D* parent) {parent_ = parent; GenerateWorldMatricies();}
    
    void update() override;
    void draw() override;
    
    DirectX::SimpleMath::Matrix global_world_matrix(){return world_transformation_matrix_;}
    DirectX::SimpleMath::Matrix compostion_transformation_matrix(){return compostion_transformation_matrix_;}
    Object3D* parent() {return parent_;}
    DirectX::SimpleMath::Matrix GenerateLocalWorldMatrixReturn();
    int GetParentLevel();
private:
    
    DirectX::SimpleMath::Vector3 location_{0, 0, 0};
    DirectX::SimpleMath::Quaternion rotation_;
    DirectX::SimpleMath::Vector3 scale_{1, 1, 1};

    DirectX::SimpleMath::Matrix local_transformation_matrix_;
    DirectX::SimpleMath::Matrix compostion_transformation_matrix_;
    DirectX::SimpleMath::Matrix world_transformation_matrix_;
    void GenerateWorldMatricies();
    void GenerateLocalWorldMatrix();
    void GenerateGlobalWorldMatrix();
    
    bool random_color = true;
    DirectX::SimpleMath::Vector4 color;
protected:
    Object3D* parent_ = nullptr;
    //ConstantBuffer<DirectX::SimpleMath::Matrix>* constant_buffer_transform;
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
};
