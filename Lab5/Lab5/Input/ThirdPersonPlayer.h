﻿#pragma once
#include "CameraObject.h"
#include "Controllable.h"
#include "Structs.h"

class CameraObject;
class Mesh;

class ThirdPersonPlayer : public Controllable, public Updatable
{
public:
    ThirdPersonPlayer(Mesh* controllable_obj, CameraObject* camera);
    void RotateCamera(const MouseMoveEventArgs& event);
    void Control() override;
    void update() override;
private:
    DirectX::SimpleMath::Vector3 direction_ = DirectX::XMFLOAT3(-1,0,0);
    DirectX::SimpleMath::Vector3 up_ = DirectX::XMFLOAT3(0,1,0);
    float distance_ = 15;
    float linear_step = 0.05;
    float angular_step = Pi/180*1;
    float angular_step_camera_x = Pi/180*0.5;
    float angular_step_camera_y = Pi/180*0.1;
    Mesh* controllable_obj_;
    CameraObject* camera_;
};
