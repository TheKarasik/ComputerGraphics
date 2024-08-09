#pragma once
#include <SimpleMath.h>

//#include "ConstantBuffer.h"
#include "Structs.h"
#include "d3d11.h"
#include "Updatable.h"


class OrthographicCamera;
class Renderer;

class LightComponent : public Updatable
{
public:
    LightComponent(Renderer* renderer);
    void update() override;
    void SetUpMatricies();
    void Activate();
    void InitializeShadows();
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
    LightDataStruct light_data;
    //ConstantBuffer<LightDataStruct>* constant_buffer_light;
    OrthographicCamera* ShadowmapCamera;
    ID3D11Texture2D* RTTextureShadow;
    ID3D11Texture2D* DSTextureShadow;
    ID3D11DepthStencilView* DSVShadow;
    ID3D11RenderTargetView* RTVShadow;
    ID3D11ShaderResourceView* SRVShadow;
    ID3D11SamplerState* SamplerState;
};
