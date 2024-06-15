#pragma once

#pragma once
#include <SimpleMath.h>

#include "CameraObject.h"
#include "Controllable.h"
#include "Structs.h"
#include "Updatable.h"

struct MouseMoveEventArgs;

class MiniMapCamera : public CameraObject
{
public:
    MiniMapCamera(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3);
    DirectX::SimpleMath::Matrix view_matrix() override {return view_matrix_;}
    DirectX::SimpleMath::Matrix perspective_matrix() override {return ortographic_matrix_;}
    DirectX::SimpleMath::Matrix ViewPerspectiveMatrix() const;
    void set_position(DirectX::SimpleMath::Vector3 position) {position_ = position;}
    void set_up_direction(DirectX::SimpleMath::Vector3 up_direction) {up_direction_ = up_direction;}
    void set_focus_position(DirectX::SimpleMath::Vector3 focus_position) {direction_ = focus_position;}
    DirectX::SimpleMath::Vector3 position() {return  position_;}
    DirectX::SimpleMath::Vector3 focus_position() {return direction_;}
    DirectX::SimpleMath::Vector3 up_direction() {return up_direction_;}
    void SetPlayer(Mesh* Player) {Player_ = Player;}
    void SetUpMatricies();
    void update() override;
    //void RotateCamera(const MouseMoveEventArgs& event);
private:
    float fov_ = Pi/4;
    float aspect_ratio_ = 1;
    float near_ = -100;
    float far_ = 100;

    float step_linear = 0.1;
    float step_angular = Pi/180/10;
    float width_ = 20;
    float height_ = 20;
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
