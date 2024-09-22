#pragma once
#include "LightComponent.h"

struct MeshGeometry;

class SpotLightComponent : public LightComponent
{
public:
    SpotLightComponent(Renderer* renderer);
    void SetDirection(DirectX::SimpleMath::Vector3 DirectionVector) override;
    void SetRange(float Range) override;
    void SetupMesh();
private:
    DirectX::SimpleMath::Vector4 GetRandomPerpendicular(DirectX::SimpleMath::Vector4 v4);
    MeshGeometry* geometry;
    unsigned int n = 16;
};
