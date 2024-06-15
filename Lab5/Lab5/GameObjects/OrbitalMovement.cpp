#include "OrbitalMovement.h"

#include <chrono>
#include <random>

#include "Cube.h"

OrbitalMovement::OrbitalMovement(Object3D* object, DirectX::SimpleMath::Vector3 normal, float radius, float angular_speed,
    float angular_speed_around_axis, bool random_orbit) :
    object_(object), normal_(normal), radius_(radius), angular_speed_(angular_speed),
    angular_speed_around_axis_(angular_speed_around_axis), random_orbit_(random_orbit)
{
    if (object_->GetParentLevel() > 1)
    {
        is_satelite = true;
        radius_ = 5;
        minmax_radius_ = DirectX::XMFLOAT2(5,10);
        minmax_angular_speed_ = DirectX::XMFLOAT2(Pi/180*1, Pi/180*2);
    }
    
    
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::default_random_engine eng(seed);
    
    if (random_orbit_)
    {
        std::uniform_real_distribution<> normal_distr(-1, 1);

        normal_ = DirectX::SimpleMath::Vector3(static_cast<float>(normal_distr(eng)),
            static_cast<float>(normal_distr(eng)), static_cast<float>(normal_distr(eng)));
        normal_.Normalize();
        
        std::uniform_real_distribution<> radius_distr(minmax_radius_.x, minmax_radius_.y);
        radius_ = static_cast<float>(radius_distr(eng));
    }

    std::uniform_real_distribution<> angular_speed_distr(minmax_angular_speed_.x, minmax_angular_speed_.y);
    angular_speed_ = static_cast<float>(angular_speed_distr(eng));
    angular_speed_around_axis_ = static_cast<float>(angular_speed_distr(eng));

    std::uniform_real_distribution<> direction_distr(-1, 1);
    const float x_direction = static_cast<float>(direction_distr(eng));
    float y_direction;
    float z_direction = 0;
    if(normal_.z != 0)
    {
        y_direction = static_cast<float>(direction_distr(eng));
        z_direction = -(x_direction*normal_.x + y_direction*normal_.y)/normal_.z;
    }
    else
    {
        y_direction = -x_direction*normal_.x/normal_.y;
    }
    direction_ = DirectX::SimpleMath::Vector3(x_direction,y_direction, z_direction);
    direction_.Normalize();
    
    object_->set_location(direction_*radius_);
}

void OrbitalMovement::update()
{
    DirectX::XMVECTOR quaternion = XMQuaternionRotationAxis(normal_, angular_speed_);
    direction_ = XMVector3Rotate(direction_, quaternion);
    //direction_.Normalize();
    object_->set_location(direction_*radius_);
    //angle_around_axis += angular_speed_around_axis_;
    object_->add_rotation(DirectX::XMFLOAT3(0,1,0), angular_speed_around_axis_);
}

void OrbitalMovement::multiply_angular_speed(float speed)
{
    angular_speed_around_axis_ = speed;
}
