#include "DirectionalLightComponent.h"

#include "CascadeShadowmapShader.h"
#include "GBufferShader.h"
#include "OrthographicCamera.h"
#include "Renderer.h"
#include "ShadowmapShader.h"
#include "Structs.h"

DirectionalLightComponent* DirectionalLightComponent::DirectionalLight_;

DirectionalLightComponent::DirectionalLightComponent(Renderer* renderer) : LightComponent(renderer)
{
    directional_light_data_ = new DirectionalLightDataStruct();
    directional_light_data_->DirectionWS = light_data->DirectionWS;
    directional_light_data_->PositionWS = -light_data->DirectionWS*00;
    directional_light_data_->DirectionVS = light_data->DirectionVS;
    directional_light_data_->Color = light_data->Color;
    directional_light_data_->Intensity = light_data->Intensity;
    light_data->Type = DirectionalLight;
    
    DirectionalLight_ = this;

    cascade_shadowmap_shader_ = new CascadeShadowmapShader(renderer_);
    shadowmap_shader_ = new ShadowmapShader(renderer_);
    Camera_ = new OrthographicCamera(v3(directional_light_data_->PositionWS), v3(light_data->DirectionWS), v3(up_direction_));
    //directional_light_data_.mView = Camera_->view_matrix().Transpose();
    directional_light_data_->mView = Camera_->view_matrix().Transpose();
    directional_light_data_->mProj = Camera_->projection_matrix().Transpose();
    
    renderer_->GBuffer().UpdateDirectionalLights();
}
void DirectionalLightComponent::SetDirection(DirectX::SimpleMath::Vector3 DirectionVector)
{
    LightComponent::SetDirection(DirectionVector);
    directional_light_data_->DirectionWS = light_data->DirectionWS;
    directional_light_data_->DirectionVS = light_data->DirectionVS;
    directional_light_data_->PositionWS = -light_data->DirectionWS*00;
    Camera_->set_position(v3(directional_light_data_->PositionWS));
    Camera_->set_direction(v3(directional_light_data_->DirectionWS));
    Camera_->set_up_direction(v3(up_direction_));
    //directional_light_data_.mView = Camera_->view_matrix().Transpose();
    directional_light_data_->mView = Camera_->view_matrix().Transpose();
    directional_light_data_->mProj = Camera_->projection_matrix().Transpose();
}

void DirectionalLightComponent::SetColor(DirectX::SimpleMath::Vector4 color)
{
    LightComponent::SetColor(color);
    directional_light_data_->Color = light_data->Color;
}

void DirectionalLightComponent::SetIntensity(float Intensity)
{
    LightComponent::SetIntensity(Intensity);
    directional_light_data_->Intensity = light_data->Intensity;
}

DirectionalLightDataStruct* DirectionalLightComponent::DirectionalLightData()
{
    return DirectionalLight_->directional_light_data_;
}

/*DirectX::SimpleMath::Matrix DirectionalLightComponent::ViewProjection()
{
    return Camera_->perspective_matrix().Transpose() * Camera_->view_matrix().Transpose();
    directional_light_data_.mProj = Camera_->perspective_matrix().Transpose();
}*/
