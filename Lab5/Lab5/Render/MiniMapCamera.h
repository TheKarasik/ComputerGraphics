#pragma once
#include "OrthographicCamera.h"

class MiniMapCamera : public OrthographicCamera
{
public:
    MiniMapCamera(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction,
        const DirectX::SimpleMath::Vector3& up_direction);

    void update() override;
};
