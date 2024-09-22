#pragma once
#include "LightComponent.h"


struct DirectionalLightDataStruct;
class CascadeShadowmapShader;

class DirectionalLightComponent : public LightComponent
{
public:
    DirectionalLightComponent(Renderer* renderer);
    OrthographicCamera* Camera() {return Camera_;}
    void SetDirection(DirectX::SimpleMath::Vector3 DirectionVector) override;
    void SetColor(DirectX::SimpleMath::Vector4 color) override;
    void SetIntensity(float Intensity) override;
    ShadowmapShader* shadowmap_shader() const {return shadowmap_shader_;}
    CascadeShadowmapShader* cascade_shadowmap_shader() const {return cascade_shadowmap_shader_;}
    static DirectionalLightDataStruct* DirectionalLightData();
    static DirectionalLightComponent* GetDirectionalLight() {return DirectionalLight_;}
    //DirectX::SimpleMath::Matrix ViewProjection();
    DirectionalLightDataStruct* directional_light_data_;
private:
    DirectX::SimpleMath::Vector3 v3(DirectX::SimpleMath::Vector4 v4) {return DirectX::SimpleMath::Vector3(v4.x, v4.y, v4.z);}
    OrthographicCamera* Camera_;
    CascadeShadowmapShader* cascade_shadowmap_shader_;
    ShadowmapShader* shadowmap_shader_;
    static DirectionalLightComponent* DirectionalLight_; 
};
