#pragma once
#include "LightComponent.h"

class PointLightComponent : public LightComponent
{
public:
    PointLightComponent(Renderer* renderer);
    void SetRange(float Range) override;
    void SetPosition(DirectX::SimpleMath::Vector3 new_position) override;
    void SetDirection(DirectX::SimpleMath::Vector3 EulerAngles) override {}
};
