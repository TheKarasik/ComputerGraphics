#pragma once
#define BUFFER_NUM 4 // Исключая основной RenderTargetView (LightAccumulation)
#include "ConstantBuffer.h"
#include "StructuredBuffer.h"

class DirectionalLightComponent;
class Display32;
class Renderer;

class GBufferShader
{
public:
  GBufferShader(Renderer* renderer, Display32* display);
  void Activate();
  void ProvideMeshData(Mesh* mesh);
  void UpdateDirectionalLights();
  ID3D11ShaderResourceView** GetDiffuseSRV() {return &SRV[0];}
  ID3D11ShaderResourceView** GetNormalSRV() {return &SRV[1];}
  ID3D11ShaderResourceView** GetSpecularSRV() {return &SRV[2];}
  ID3D11ShaderResourceView** GetDepthSRV() {return &SRVDepth;}
private:
  DirectX::SimpleMath::Vector2 ScreenDimensions;
  ConstantBuffer<ConstantBufferTransformMatricies>* constant_buffer_transform;
  ConstantBuffer<MaterialStructCB>* constant_buffer_texture;
  //ConstantBuffer<StructuredBufferSize>* constant_buffer_dir_lights_num;
  ConstantBuffer<ScrenToViewStructCB>* constant_buffer_screen_to_view;
  ConstantBuffer<CascadeData>* constant_buffer_cascade;
  //StructuredBuffer<DirectionalLightDataStruct>* directional_lights;
  ConstantBuffer<DirectionalLightDataStruct>* constant_buffer_directional_light;
  ID3D11Texture2D* LightTextures[BUFFER_NUM];
  ID3D11RenderTargetView* RTV[BUFFER_NUM];
  ID3D11ShaderResourceView* SRV[BUFFER_NUM];
  ID3D11ShaderResourceView* SRVDepth;
  ID3D11Texture2D* RTDepthStencilTexture;
  ID3D11DepthStencilView* DSV;
  PixelShader* pixel_shader_;
  VertexShader* vertex_shader_;
  ID3D11InputLayout* layout;
  ID3D11RasterizerState* rastState;
  D3D11_VIEWPORT viewport;
  Renderer* renderer_;
  Display32* display_;
};
