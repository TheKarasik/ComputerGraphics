#include "OrthographicCamera.h"

#include "Frustrum.h"
#include "InputDevice.h"
#include "Mesh.h"
#include "Structs.h"

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position,
                           DirectX::SimpleMath::Vector3 direction,
                           DirectX::SimpleMath::Vector3 up_direction) :
    position_(position), direction_(direction), up_direction_(up_direction)
{
    //cascade_data_ = new CascadeData();
    SetUpMatricies();
    //InputDevice::singleton->MouseMove.AddRaw(this, &CameraObject::RotateCamera);
}

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 direction,
    DirectX::SimpleMath::Vector3 up_direction, float width, float height, float near_plane, float far_plane) : 
    OrthographicCamera(position, direction, up_direction) 
{
    set_camera_props(width,height, near_plane, far_plane);
}

DirectX::SimpleMath::Matrix OrthographicCamera::ViewPerspectiveMatrix() const
{
    return (view_matrix_*ortographic_matrix_);
    //return (view_matrix_*perspective_matrix_).Transpose();
    //return (perspective_matrix_*view_matrix_);
    //return (perspective_matrix_*view_matrix_).Transpose();
}

void OrthographicCamera::set_camera_props(float width, float height, float near_plane, float far_plane)
{
    width_ = width;
    height_ = height;
    near_ = near_plane;
    far_ = far_plane;
}

void OrthographicCamera::update()
{
    SetUpMatricies();
    /*Frustrum frustrum(this);
    frustrum.ProvideCascadeData(cascade_data_);
    frustrum.GenerateCascadeData();*/
}



void OrthographicCamera::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, direction_, up_direction_);
    ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}

