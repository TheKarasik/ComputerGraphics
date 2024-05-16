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

struct RectangleGeometry
{
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;
    bool DetectCollision(RectangleGeometry rect)
    {
        if (fabsf(pos.x - rect.pos.x) <= size.x + rect.size.y &&
            fabsf(pos.y - rect.pos.y) <= size.y + rect.size.y) return true;
        return false;
    }
    bool DetectVerticalBorderHit()
    {
        if (fabsf(pos.y) + size.y >= 1) return true;
        return false;
    }
    bool DetectHorizontalBorderHit()
    {
        if (fabsf(pos.x) + size.x >= 1) return true;
        return false;
    }
};