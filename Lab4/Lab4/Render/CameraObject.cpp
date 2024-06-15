#include "CameraObject.h"

#include "InputDevice.h"
#include "Structs.h"

CameraObject::CameraObject(DirectX::SimpleMath::Vector3 position,
                           DirectX::SimpleMath::Vector3 look_direction,
                           DirectX::SimpleMath::Vector3 up_direction) :
    position_(position), look_direction_(look_direction), up_direction_(up_direction),Controllable(InputDevice::singleton)
{
    SetUpMatricies();
    InputDevice::singleton->MouseMove.AddRaw(this, &CameraObject::RotateCamera);
}

/*void CameraObject::RotateCamera(DirectX::SimpleMath::Vector2 step)
{
    look_direction_ += DirectX::SimpleMath::Vector3(step.x, step.y, 0);
    look_direction_.Normalize();
    up_direction_.y += step.y;
    up_direction_.Normalize();
    SetUpMatricies();
}*/

DirectX::SimpleMath::Matrix CameraObject::ViewPerspectiveMatrix() const
{
    return (view_matrix_*perspective_matrix_);
    //return (view_matrix_*perspective_matrix_).Transpose();
    //return (perspective_matrix_*view_matrix_);
    //return (perspective_matrix_*view_matrix_).Transpose();
}

void CameraObject::update()
{
    SetUpMatricies();
}

void CameraObject::RotateCamera(const MouseMoveEventArgs& event)
{
    if (event.Offset.x != 0)
    {
        DirectX::XMVECTOR up_rotation_quaternion = XMQuaternionRotationAxis
            (DirectX::SimpleMath::Vector3(0,1, 0), event.Offset.x*step_angular);
        look_direction_ = XMVector3Rotate(look_direction_, up_rotation_quaternion);
        up_direction_ = XMVector3Rotate(up_direction_, up_rotation_quaternion);  
    }
    if (event.Offset.y != 0)
    {
        DirectX::XMVECTOR right_rotation_quaternion = XMQuaternionRotationAxis
            (right_direction()*step_linear, event.Offset.y*step_angular);
        look_direction_ = XMVector3Rotate(look_direction_, right_rotation_quaternion);   
        up_direction_ = XMVector3Rotate(up_direction_, right_rotation_quaternion);  
    }
}

void CameraObject::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, look_direction_, up_direction_);
    perspective_matrix_ = DirectX::XMMatrixPerspectiveFovLH(fov_, aspect_ratio_, near_, far_);
}

void CameraObject::Control()
{
    if (InputDevice::singleton->IsKeyDown(Keys::W)) position_ += look_direction_*step_linear;
    if (InputDevice::singleton->IsKeyDown(Keys::S)) position_ -= look_direction_*step_linear;
    if (InputDevice::singleton->IsKeyDown(Keys::A)) position_ -= right_direction()*step_linear;
    if (InputDevice::singleton->IsKeyDown(Keys::D)) position_ += right_direction()*step_linear;
}