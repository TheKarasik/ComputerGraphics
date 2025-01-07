#include "PerspectiveCamera.h"

#include "Frustrum.h"
#include "InputDevice.h"
#include "Structs.h"

PerspectiveCamera::PerspectiveCamera(DirectX::SimpleMath::Vector3 position,
                           DirectX::SimpleMath::Vector3 focus_position,
                           DirectX::SimpleMath::Vector3 up_direction) :
    position_(position), focus_position_(focus_position), up_direction_(up_direction)
{
    //reflection_position_ = DirectX::SimpleMath::Vector3(position_.x, -position_.y + (), position_.z);
    SetUpMatricies();
    cascade_data_ = new CascadeData();
    //frustrum_ = new Frustrum(this);
    //InputDevice::singleton->MouseMove.AddRaw(this, &CameraObject::RotateCamera);
}

PerspectiveCamera::PerspectiveCamera(PerspectiveCamera& camera)
{
    fov_ = camera.fov_;
    aspect_ratio_ = camera.aspect_ratio_;
    near_ = camera.near_;
    far_ = camera.far_;

    step_linear = camera.step_linear;
    step_angular = camera.step_angular;

    view_matrix_ = camera.view_matrix_;
    perspective_matrix_ = camera.perspective_matrix_;

    position_ = camera.position_;
    focus_position_ = camera.focus_position_;
    up_direction_ = camera.up_direction_;
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
    Frustrum frustrum(this);
    frustrum.ProvideCascadeData(cascade_data_);
    frustrum.GenerateCascadeData();
}

void PerspectiveCamera::RotateCamera(const MouseMoveEventArgs& event)
{
    /*if (event.Offset.x != 0)
    {
        DirectX::XMVECTOR up_rotation_quaternion = XMQuaternionRotationAxis
            (DirectX::SimpleMath::Vector3(0,1, 0), event.Offset.x*step_angular);
        //focus_position_ = XMVector3Rotate(focus_position_, up_rotation_quaternion);
        up_direction_ = XMVector3Rotate(up_direction_, up_rotation_quaternion);  
    }
    if (event.Offset.y != 0)
    {
        DirectX::XMVECTOR right_rotation_quaternion = XMQuaternionRotationAxis
            (right_direction()/**step_linear#1#, event.Offset.y*step_angular);
        //focus_position_ = XMVector3Rotate(focus_position_, right_rotation_quaternion);   
        up_direction_ = XMVector3Rotate(up_direction_, right_rotation_quaternion);  
    }*/
}


PerspectiveCamera* PerspectiveCamera::GetNewCameraRepresentingSubfrustrum(float near_plane_percent, float far_plane_percent)
{
    PerspectiveCamera camera(*this);
    camera.setNear(camera.Near()*near_plane_percent);
    camera.setFar(camera.Far()*far_plane_percent);
    camera.SetUpMatricies();
    return &camera;
}

DirectX::SimpleMath::Matrix PerspectiveCamera::GetSubfrustrumProjection(float near_plane_percent, float far_plane_percent)
{
    if (!near_plane_percent)
        return DirectX::XMMatrixPerspectiveFovLH(fov_, aspect_ratio_, 1, far_*far_plane_percent);
    return DirectX::XMMatrixPerspectiveFovLH(fov_, aspect_ratio_, far_*near_plane_percent, far_*far_plane_percent);
}

void PerspectiveCamera::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookAtLH(position_, focus_position_, up_direction_);
    perspective_matrix_ = DirectX::XMMatrixPerspectiveFovLH(fov_, aspect_ratio_, near_, far_);
}

