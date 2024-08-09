#pragma once
#include <d3d11.h>

#include "ConstantBuffer.h"
#include "Structs.h"


class LightComponent;
class VertexShader;
class PixelShader;
class Renderer;

class MainShader
{
public:
    MainShader(Renderer* renderer);
    void Activate();
    void SetLight(LightComponent* light_component) {light_ = light_component;}
    void UpdateTransformCB(ConstantBufferTransformMatricies CBSubresource);
    void UpdateTransformData(ConstantBufferTransformMatricies TransformData);
    void UpdateMaterialData(MaterialProperties Material);
    void UpdateLightData(LightDataStruct Light);
    VertexShader* vertex_shader() {return vertex_shader_;}
    PixelShader* pixel_shader() {return pixel_shader_;}

private:
    ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
    ConstantBuffer<MaterialProperties>* texture_constant_buffer_;
    ConstantBuffer<LightDataStruct>* constant_buffer_light;
    Renderer* renderer_;
    PixelShader* pixel_shader_;
    VertexShader* vertex_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
    LightComponent* light_;
};
