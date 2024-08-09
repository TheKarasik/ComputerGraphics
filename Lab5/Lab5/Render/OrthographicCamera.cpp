#include "OrthographicCamera.h"

#include "InputDevice.h"
#include "Mesh.h"
#include "Structs.h"

OrthographicCamera::OrthographicCamera(DirectX::SimpleMath::Vector3 position,
                           DirectX::SimpleMath::Vector3 direction,
                           DirectX::SimpleMath::Vector3 up_direction) :
    position_(position), direction_(direction), up_direction_(up_direction)
{
    SetUpMatricies();
    //InputDevice::singleton->MouseMove.AddRaw(this, &CameraObject::RotateCamera);
}
DirectX::SimpleMath::Matrix OrthographicCamera::ViewPerspectiveMatrix() const
{
    return (view_matrix_*ortographic_matrix_);
    //return (view_matrix_*perspective_matrix_).Transpose();
    //return (perspective_matrix_*view_matrix_);
    //return (perspective_matrix_*view_matrix_).Transpose();
}

void OrthographicCamera::update()
{
    SetUpMatricies();
}



void OrthographicCamera::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, direction_, up_direction_);
    ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}

