#pragma once
#include <d3d11.h>

class PixelShader;
class VertexShader;
class Renderer;

class ShadowmapShader
{
public:
    ShadowmapShader(Renderer* renderer);
    void Activate();
private:
    Renderer* renderer_;
    PixelShader* pixel_shader_;
    VertexShader* vertex_shader_;
    ID3D11InputLayout* layout;
    ID3D11RasterizerState* rastState;
};
