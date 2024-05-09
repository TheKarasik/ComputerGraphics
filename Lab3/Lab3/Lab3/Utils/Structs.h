#pragma once
#include <DirectXMath.h>

struct TriangleVertex
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
};

struct TriangleGeometry
{
    TriangleVertex vertecies[3];
    TriangleGeometry() = default;
};

struct ConstantDataVertexShader
{
    DirectX::XMFLOAT4 offset;
    DirectX::XMFLOAT4 size;
};