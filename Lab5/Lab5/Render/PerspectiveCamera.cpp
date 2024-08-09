#include "PerspectiveCamera.h"

#include "InputDevice.h"
#include "Structs.h"

PerspectiveCamera::PerspectiveCamera(DirectX::SimpleMath::Vector3 position,
                           DirectX::SimpleMath::Vector3 focus_position,
                           DirectX::SimpleMath::Vector3 up_direction) :
    position_(position), focus_position_(focus_position), up_direction_(up_direction)
{
    SetUpMatricies();
    //InputDevice::singleton->MouseMove.AddRaw(this, &CameraObject::RotateCamera);
}
DirectX::SimpleMath::Matrix PerspectiveCamera::ViewPerspectiveMatrix() const
{
    return (view_matrix_*perspective_matrix_);
    //return (view_matrix_*perspective_matrix_).Transpose();
    //return (perspective_matrix_*view_matrix_);
    //return (perspective_matrix_*view_matrix_).Transpose();
}

void PerspectiveCamera::update()
{
    SetUpMatricies();
}

void PerspectiveCamera::RotateCamera(const MouseMoveEventArgs& event)
{
    if (event.Offset.x != 0)
    {
        DirectX::XMVECTOR up_rotation_quaternion = XMQuaternionRotationAxis
            (DirectX::SimpleMath::Vector3(0,1, 0), event.Offset.x*step_angular);
        //focus_position_ = XMVector3Rotate(focus_position_, up_rotation_quaternion);
        up_direction_ = XMVector3Rotate(up_direction_, up_rotation_quaternion);  
    }
    if (event.Offset.y != 0)
    {
        DirectX::XMVECTOR right_rotation_quaternion = XMQuaternionRotationAxis
            (right_direction()/**step_linear*/, event.Offset.y*step_angular);
        //focus_position_ = XMVector3Rotate(focus_position_, right_rotation_quaternion);   
        up_direction_ = XMVector3Rotate(up_direction_, right_rotation_quaternion);  
    }
}

void PerspectiveCamera::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookAtLH(position_, focus_position_, up_direction_);
    perspective_matrix_ = DirectX::XMMatrixPerspectiveFovLH(fov_, aspect_ratio_, near_, far_);
}

