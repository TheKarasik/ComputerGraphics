#include "LightComponent.h"

LightComponent::LightComponent(Renderer* renderer)
{
    renderer_ = renderer;
    light_data.directionWS = DirectX::SimpleMath::Vector4(direction_.x, direction_.y, direction_.z, 0);
    constant_buffer_light = new ConstantBuffer<LightDataStruct>(renderer);
}

void LightComponent::update()
{
    light_data.mViewProj = ortographic_matrix_.Transpose()*view_matrix_.Transpose();
    constant_buffer_light->UpdateBuffer(&light_data);
}

void LightComponent::SetUpMatricies()
{
    view_matrix_ = XMMatrixLookToLH(position_, direction_, up_direction_);
    ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}

void LightComponent::Activate()
{
    renderer_->Context()->PSSetConstantBuffers(2,1,constant_buffer_light->buffer());
}
