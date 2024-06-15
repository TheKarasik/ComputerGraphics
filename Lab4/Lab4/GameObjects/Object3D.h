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
    struct ConstantBufferDataStruct
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
    } constant_buffer_data_;
    DirectX::SimpleMath::Matrix all_transform_matrix;
    
    Object3D(Renderer* renderer, Object3D* parent = nullptr, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1,1,1,1));
    /*Object3D::Object3D(Renderer* renderer) : Drawable(renderer) 
    {
        new ConstantBuffer<TransformCBStruct>(renderer_);
    }*/
    void set_location(DirectX::XMFLOAT3 location) {location_ = location;}
    void set_rotation(DirectX::SimpleMath::Quaternion rotation) {rotation_ = rotation;}
    void set_rotation(DirectX::XMFLOAT3 axis, float angle){rotation_ = DirectX::SimpleMath::Quaternion
        (axis.x * sin(angle/2), axis.y * sin(angle/2),axis.z * sin(angle/2), cos(angle/2));}
    void set_scale(DirectX::XMFLOAT3 scale) {scale_ = scale;}

    void add_location(DirectX::XMFLOAT3 location) {location_ += location;}
    void add_rotation(DirectX::SimpleMath::Quaternion rotation) {rotation_ *= rotation;}
    void add_rotation(DirectX::XMFLOAT3 axis, float angle){rotation_ *= DirectX::SimpleMath::Quaternion
        (axis.x * sin(angle/2), axis.y * sin(angle/2),axis.z * sin(angle/2), cos(angle/2));}
    void add_scale(DirectX::XMFLOAT3 scale) {scale_ *= scale;}
    
    void update() override;
    void draw() override;
    
    DirectX::SimpleMath::Matrix global_world_matrix(){return global_world_matrix_;}
    Object3D* parent() {return parent_;}
    DirectX::SimpleMath::Matrix GenerateLocalWorldMatrixReturn();
    int GetParentLevel();
private:
    Object3D* parent_ = nullptr;
    
    DirectX::SimpleMath::Vector3 location_{0, 0, 0};
    DirectX::SimpleMath::Quaternion rotation_;
    DirectX::SimpleMath::Vector3 scale_{1, 1, 1};

    DirectX::SimpleMath::Matrix local_world_matrix_;
    DirectX::SimpleMath::Matrix global_world_matrix_;
    void GenerateWorldMatricies();
    void GenerateLocalWorldMatrix();
    void GenerateGlobalWorldMatrix();
    
    bool random_color = true;
    DirectX::SimpleMath::Vector4 color;
protected:
    ConstantBuffer<DirectX::SimpleMath::Matrix>* constant_buffer_transform;
    //ConstantBuffer<ConstantBufferDataStruct>* constant_buffer_transform;
    DirectX::XMFLOAT4 color_ = DirectX::XMFLOAT4(1,1,1,1);
};
