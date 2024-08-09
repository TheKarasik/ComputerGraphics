#include "ThirdPersonPlayer.h"

#include "Mesh.h"

ThirdPersonPlayer::ThirdPersonPlayer(Mesh* controllable_obj, PerspectiveCamera* camera) : controllable_obj_(controllable_obj),Controllable(InputDevice::singleton), camera_(camera)
{
    controllable_obj_->PlayerControlled = true;
    
    DirectX::SimpleMath::Vector3 forward = DirectX::SimpleMath::Vector3(0,0,1);
    up_ = DirectX::SimpleMath::Vector3(0,1,0);
    direction_ = -forward;
    
    camera_->set_position(controllable_obj_->location() + direction_*distance_);
    camera_->set_focus_position(controllable_obj_->location());
    camera_->set_up_direction(up_);
    //camera_ = new CameraObject(loc + direction_*distance_, forward, up);

    InputDevice::singleton->MouseMove.AddRaw(this, &ThirdPersonPlayer::RotateCamera);
}

void ThirdPersonPlayer::RotateCamera(const MouseMoveEventArgs& event)
{
    DirectX::SimpleMath::Vector3 new_direction_;
    if (abs(event.Offset.x) > 0)
    {
        DirectX::XMVECTOR up_rotation_quaternion = XMQuaternionRotationAxis
            (camera_->up_direction(), event.Offset.x*angular_step_camera_x);
        new_direction_ = XMVector3Rotate(direction_, up_rotation_quaternion);
        //focus_position_ = XMVector3Rotate(focus_position_, up_rotation_quaternion);
        //up_ = XMVector3Rotate(up_, up_rotation_quaternion);  
    }
    if (abs(event.Offset.y) > 0)
    {
        DirectX::XMVECTOR right_rotation_quaternion = DirectX::XMQuaternionRotationAxis
            (camera_->right_direction(), event.Offset.y*angular_step_camera_y);
        //focus_position_ = XMVector3Rotate(focus_position_, right_rotation_quaternion);
        new_direction_ = XMVector3Rotate(direction_, right_rotation_quaternion);
        //up_ = XMVector3Rotate(up_, right_rotation_quaternion);  
    }
    if (new_direction_.y > 0.9) return;
    if (new_direction_.y < -0.1) return;
    direction_ = new_direction_;
    //update();
}

void ThirdPersonPlayer::Control()
{
    if (InputDevice::singleton->IsKeyDown(Keys::W)) controllable_obj_->moveForBackwards(linear_step, angular_step);
    if (InputDevice::singleton->IsKeyDown(Keys::S)) controllable_obj_->moveForBackwards(-linear_step, -angular_step);
    if (InputDevice::singleton->IsKeyDown(Keys::A)) controllable_obj_->moveRightLeft(linear_step, angular_step);
    if (InputDevice::singleton->IsKeyDown(Keys::D)) controllable_obj_->moveRightLeft(-linear_step, -angular_step);
}

void ThirdPersonPlayer::update()
{
    camera_->set_position(controllable_obj_->location() + direction_*distance_);
    camera_->set_focus_position(controllable_obj_->location());
    camera_->set_up_direction(up_);
    //camera_->SetUpMatricies();
    /*DirectX::SimpleMath::Vector3 loc = controllable_obj_->location();
    DirectX::SimpleMath::Matrix rotation_matrix = XMMatrixRotationQuaternion(controllable_obj_->rotation());
    DirectX::SimpleMath::Vector3 forward = rotation_matrix.Forward();
    DirectX::SimpleMath::Vector3 up = rotation_matrix.Up();*/
}
