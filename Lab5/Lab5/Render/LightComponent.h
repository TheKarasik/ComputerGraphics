#pragma once
#include <SimpleMath.h>

#include "ConstantBuffer.h"
#include "Updatable.h"


class LightComponent : public Updatable
{
public:
    LightComponent(Renderer* renderer);
    void update() override;
    void SetUpMatricies();
    void Activate();
    Renderer* renderer_;
    DirectX::SimpleMath::Matrix view_matrix_;
    DirectX::SimpleMath::Matrix ortographic_matrix_;
    DirectX::SimpleMath::Vector3 position_ = DirectX::SimpleMath::Vector3(0,30,0);
    DirectX::SimpleMath::Vector3 direction_ = DirectX::SimpleMath::Vector3(0,-1,0);
    DirectX::SimpleMath::Vector3 up_direction_ = DirectX::SimpleMath::Vector3(1,0,0);
    float width_ = 800;
    float height_ = 800;
    float near_ = -100;
    float far_ = 100;
    struct LightDataStruct
    {
        DirectX::SimpleMath::Matrix mViewProj;
        DirectX::SimpleMath::Vector4 directionWS;
        DirectX::SimpleMath::Vector4 color = DirectX::SimpleMath::Vector4(1,1,1,1);
    } light_data;
    ConstantBuffer<LightDataStruct>* constant_buffer_light;
};
