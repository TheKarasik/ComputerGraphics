#pragma once

#pragma once
#include <SimpleMath.h>

#include "PerspectiveCamera.h"
#include "Controllable.h"
#include "Structs.h"
#include "Updatable.h"

struct MouseMoveEventArgs;

class OrthographicCamera : public PerspectiveCamera
{
public:
    OrthographicCamera(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3);
    OrthographicCamera(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3, float, float, float, float);
    DirectX::SimpleMath::Matrix view_matrix() override {return view_matrix_;}
    DirectX::SimpleMath::Matrix perspective_matrix() override {return ortographic_matrix_;}
    DirectX::SimpleMath::Matrix ViewPerspectiveMatrix() const;
    void set_position(DirectX::SimpleMath::Vector3 position) {position_ = position;}
    void set_up_direction(DirectX::SimpleMath::Vector3 up_direction) {up_direction_ = up_direction;}
    void set_focus_position(DirectX::SimpleMath::Vector3 focus_position) {direction_ = focus_position;}
    void set_camera_props(float width, float height, float near_plane, float far_plane);
    DirectX::SimpleMath::Vector3 position() {return  position_;}
    DirectX::SimpleMath::Vector3 focus_position() {return direction_;}
    DirectX::SimpleMath::Vector3 up_direction() {return up_direction_;}
    void SetPlayer(Mesh* Player) {Player_ = Player;}
    void SetUpMatricies();
    void update() override;
    //void RotateCamera(const MouseMoveEventArgs& event);
protected:
    float near_ = -100;
    float far_ = 100;
    float width_ = 100;
    float height_ = 100;

    /*float near_;
    float far_;
    float width_;
    float height_;*/
    
    Mesh* Player_;

    DirectX::SimpleMath::Matrix view_matrix_;
    DirectX::SimpleMath::Matrix ortographic_matrix_;
  
    DirectX::SimpleMath::Vector3 position_;
    DirectX::SimpleMath::Vector3 direction_;
    DirectX::SimpleMath::Vector3 up_direction_;



public:
    //void Control() override;
    DirectX::SimpleMath::Vector3 right_direction() {return  XMVector3Cross(direction_-position_, up_direction_);}
};
