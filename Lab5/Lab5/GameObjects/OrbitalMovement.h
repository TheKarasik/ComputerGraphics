#pragma once
#include <SimpleMath.h>
#include <vector>

#include "Renderer.h"
#include "Updatable.h"

class Object3D;

class OrbitalMovement : public Updatable
{
public:
    OrbitalMovement(Object3D* object, DirectX::SimpleMath::Vector3 normal = DirectX::SimpleMath::Vector3 (0, 1, 0),
        float radius = 10, float angular_speed = Pi/180*1, float angular_speed_around_axis = Pi/180*1, bool random_orbit = false);
    void update() override;
    void multiply_angular_speed(float speed);
private:
    DirectX::SimpleMath::Vector3 normal_;
    DirectX::SimpleMath::Vector3 direction_;
    float radius_;
    float angular_speed_;
    float angular_speed_around_axis_;
    bool random_orbit_;
    Object3D* object_;
    bool is_satelite = false;
    float angle_around_axis = 0;
    DirectX::XMFLOAT2 minmax_radius_ = DirectX::XMFLOAT2(10,20);
    DirectX::XMFLOAT2 minmax_angular_speed_ = DirectX::XMFLOAT2(Pi/180*0.1, Pi/180*1);
    //std::vector<Object3D*> objects;
};
