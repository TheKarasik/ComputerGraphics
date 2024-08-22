#include "LightComponent.h"

#include "MainShader.h"
#include "OrthographicCamera.h"
#include "Renderer.h"
#include "ShadowmapShader.h"

LightComponent::LightComponent(Renderer* renderer)
{
    renderer_ = renderer;
    light_data.directionWS = DirectX::SimpleMath::Vector4(direction_.x, direction_.y, direction_.z, 0);
    ShadowmapCamera = new OrthographicCamera(position_, direction_, up_direction_,width_,height_, near_, far_);
    //ShadowmapCamera->set_camera_props(width_,height_, near_, far_);
    shadowmap_shader = new ShadowmapShader(renderer_);
    //SetUpMatricies();
    //light_data.mViewProj = ortographic_matrix_.Transpose()*view_matrix_.Transpose();
    light_data.mViewProj = ShadowmapCamera->ViewPerspectiveMatrix().Transpose();
    light_data.position = DirectX::SimpleMath::Vector4(position_.x, position_.y, position_.z, 1);
}

/*void LightComponent::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, direction_, up_direction_);
    ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}*/

/*void LightComponent::Activate()
{
    renderer_->Context()->PSSetConstantBuffers(2,1,constant_buffer_light->buffer());
}*/
