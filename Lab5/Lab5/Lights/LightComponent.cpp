#include "LightComponent.h"

#include "MainShader.h"
#include "Mesh.h"
#include "OrthographicCamera.h"
#include "Renderer.h"
#include "ShadowmapShader.h"

LightComponent::LightComponent(Renderer* renderer)
{
    renderer_ = renderer;
    up_direction_ = DirectX::SimpleMath::Vector4(0, 1, 0, 0);
    
    //SetUpMatricies();
    
    light_data.PositionWS = DirectX::SimpleMath::Vector4(0, 0, 0, 1);
    light_data.DirectionWS = DirectX::SimpleMath::Vector4(1, 0, 0, 0);
    SetUpVSVectors();
    light_data.Color = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
    light_data.SpotlightAngle = 45;
    light_data.Range = 1;
    light_data.Intensity = 1;
    light_data.Type = DirectionalLight;
    
    //ShadowmapCamera->set_camera_props(width_,height_, near_, far_);
    //SetUpMatricies();
    //light_data.mViewProj = ortographic_matrix_.Transpose()*view_matrix_.Transpose();
    //light_data.mViewProj = ShadowmapCamera->ViewPerspectiveMatrix().Transpose();
}

void LightComponent::draw_volume()
{
    if(!volume_) return;
    
    volume_->set_drawability(true);
    volume_->draw();
    volume_->set_drawability(false);
}

void LightComponent::update()
{
    SetUpVSVectors();
}

void LightComponent::SetPosition(DirectX::SimpleMath::Vector3 new_position)
{
    light_data.PositionWS = DirectX::SimpleMath::Vector4(new_position.x, new_position.y, new_position.z, 1);
    if (LightData()->Type != DirectionalLight)
        volume_->set_location(new_position);
    SetUpVSVectors(); 
}

void LightComponent::SetDirection(DirectX::SimpleMath::Vector3 DirectionVector)
{
    DirectionVector.Normalize();
    DirectX::SimpleMath::Quaternion q;
    {
        DirectX::SimpleMath::Vector3 v1 = DirectX::SimpleMath::Vector3(light_data.DirectionWS.x,
            light_data.DirectionWS.y, light_data.DirectionWS.z);
        DirectX::SimpleMath::Vector3 v2 = DirectionVector;
        DirectX::SimpleMath::Vector3 cp = v1.Cross(v2);
        q = DirectX::SimpleMath::Quaternion(cp.x, cp.y, cp.z, 0);
        q.w = sqrtf(powf(v1.Length(), 2) + powf(v2.Length(), 2)) + v1.Dot(v2);
        q.Normalize();
    }
    light_data.DirectionWS = DirectX::SimpleMath::Vector4(DirectionVector.x, DirectionVector.y, DirectionVector.z, 0);
    
    DirectX::SimpleMath::Vector4 inverse = XMQuaternionInverse(q);
    up_direction_ = XMQuaternionMultiply(q, up_direction_);
    up_direction_ = XMQuaternionMultiply(up_direction_, inverse);
    up_direction_.Normalize();

    if (LightData()->Type != PointLight)
        volume_->add_rotation(q);
    SetUpVSVectors();
    
    /*DirectX::SimpleMath::Vector4 rotation_quat = XMQuaternionRotationRollPitchYawFromVector(DirectionVector);
    DirectX::SimpleMath::Vector4 inverse = XMQuaternionInverse(rotation_quat);
    
    light_data.DirectionWS = XMQuaternionMultiply(rotation_quat, light_data.DirectionWS);
    light_data.DirectionWS = XMQuaternionMultiply(light_data.DirectionWS, inverse);
    light_data.DirectionWS.Normalize();
    
    up_direction_ = XMQuaternionMultiply(rotation_quat, light_data.DirectionWS);
    up_direction_ = XMQuaternionMultiply(light_data.DirectionWS, inverse);
    up_direction_.Normalize();

    if (LightData()->Type != PointLight)
        volume_->set_rotation(rotation_quat);
    SetUpVSVectors();*/
}

void LightComponent::SetSpotlightAngle(float SpotlightAngle)
{
    light_data.SpotlightAngle = SpotlightAngle;

    
}

void LightComponent::SetRange(float Range)
{
    light_data.Range = Range;
}

/*void LightComponent::SetType(LightType Type)
{
    switch (Type)
    {
    case PointLight:
        volume_ = new PointLightVolume(renderer_);
        volume_->Volume()->set_scale(DirectX::SimpleMath::Vector3(light_data.Range, light_data.Range, light_data.Range));
        break;
    case SpotLight:

        volume_->Volume()->set_rotation(XMQuaternionRotationRollPitchYawFromVector(light_data.DirectionWS));
        break;
    case DirectionalLight:

        volume_->Volume()->set_rotation(XMQuaternionRotationRollPitchYawFromVector(light_data.DirectionWS));
        break;
    }
    volume_->Volume()->set_location(DirectX::SimpleMath::Vector3(light_data.PositionWS.x, light_data.PositionWS.y, light_data.PositionWS.z));
    light_data.Type = Type;
}*/

void LightComponent::SetUpVSVectors()
{
    //DirectX::SimpleMath::Matrix mView = XMMatrixLookToLH(light_data.PositionWS, light_data.DirectionWS, up_direction_);
    light_data.PositionVS = XMVector4Transform(light_data.PositionWS, renderer_->camera()->view_matrix());
    light_data.DirectionVS = XMVector4Transform(light_data.DirectionWS, renderer_->camera()->view_matrix());
    //light_data.mView = mView.Transpose();
    
    //ortographic_matrix_ = DirectX::XMMatrixOrthographicLH(width_, height_, near_, far_);
}

/*void LightComponent::Activate()
{
    renderer_->Context()->PSSetConstantBuffers(2,1,constant_buffer_light->buffer());
}*/
