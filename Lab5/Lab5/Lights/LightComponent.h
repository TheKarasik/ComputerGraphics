#pragma once
#include <SimpleMath.h>

//#include "ConstantBuffer.h"
#include "Structs.h"
#include "d3d11.h"
#include "Updatable.h"


class Mesh;
class LightVolume;
class OrthographicCamera;
class Renderer;
class ShadowmapShader;

class LightComponent : public Updatable
{
public:
    LightComponent(Renderer* renderer);
    virtual void draw_volume();
    void update() override;
    virtual void SetPosition(DirectX::SimpleMath::Vector3 new_position);
    virtual void SetDirection(DirectX::SimpleMath::Vector3 DirectionVector);
    virtual void SetColor(DirectX::SimpleMath::Vector4 color) {light_data.Color = color;}
    virtual void SetSpotlightAngle(float SpotlightAngle); 
    virtual void SetRange(float Range); 
    virtual void SetIntensity(float Intensity) {light_data.Intensity = Intensity;}
    //void SetType(LightType Type);
    void SetUpVSVectors();
    DirectX::SimpleMath::Vector4 up_direction() {return up_direction_;}
    LightDataStruct* LightData() {return &light_data;}
    Mesh* Volume() {return volume_;}
    //void Activate();
    
    static std::vector<LightComponent*> Lights;
    
    /*DirectX::SimpleMath::Vector3 position_ = DirectX::SimpleMath::Vector3(0,30,0);
    DirectX::SimpleMath::Vector3 direction_ = DirectX::SimpleMath::Vector3(0,-1,0);
    DirectX::SimpleMath::Vector3 up_direction_ = DirectX::SimpleMath::Vector3(1,0,0);*/
    
    /*float width_ = 800;
    float height_ = 800;
    float near_ = -100;
    float far_ = 100;*/
    
    //ConstantBuffer<LightDataStruct>* constant_buffer_light;
protected:
    Renderer* renderer_;
    Mesh* volume_ = nullptr;
    LightDataStruct light_data;
    DirectX::SimpleMath::Vector4 up_direction_;
private:
    
    DirectX::SimpleMath::Quaternion rotation;
};
