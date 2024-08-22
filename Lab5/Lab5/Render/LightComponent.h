#pragma once
#include <SimpleMath.h>

//#include "ConstantBuffer.h"
#include "Structs.h"
#include "d3d11.h"
#include "Updatable.h"


class OrthographicCamera;
class Renderer;
class ShadowmapShader;

class LightComponent
{
public:
    LightComponent(Renderer* renderer);
    //void SetUpMatricies();
    //void Activate();
    DirectX::SimpleMath::Matrix view_matrix_;
    DirectX::SimpleMath::Matrix ortographic_matrix_;
    DirectX::SimpleMath::Vector3 position_ = DirectX::SimpleMath::Vector3(0,30,0);
    DirectX::SimpleMath::Vector3 direction_ = DirectX::SimpleMath::Vector3(0,-1,0);
    DirectX::SimpleMath::Vector3 up_direction_ = DirectX::SimpleMath::Vector3(1,0,0);
    /*DirectX::SimpleMath::Vector3 direction_ = DirectX::SimpleMath::Vector3(0.788,-0.788,0);
    DirectX::SimpleMath::Vector3 up_direction_ = DirectX::SimpleMath::Vector3(0.788,0.788,0);*/
    float width_ = 800;
    float height_ = 800;
    float near_ = -100;
    float far_ = 100;
    LightDataStruct light_data;
    Renderer* renderer_;
    //ConstantBuffer<LightDataStruct>* constant_buffer_light;
    OrthographicCamera* ShadowmapCamera;
    ShadowmapShader* shadowmap_shader;
};
