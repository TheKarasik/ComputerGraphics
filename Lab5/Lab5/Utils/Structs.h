#pragma once
#include <DirectXMath.h>
#include "vector"
#define Pi 3.1415926535f


struct TriangleVertex     //New version
{
    TriangleVertex() = default;
    TriangleVertex(DirectX::XMFLOAT4 pos) : position(pos) {}
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT2 tex;
    DirectX::XMFLOAT3 normal;
};

/*struct TriangleVertex       //Old version
{
    TriangleVertex(DirectX::XMFLOAT4 pos) : position(pos), color(DirectX::XMFLOAT4(1,1,1,1)) {}
    TriangleVertex(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 col) : position(pos), color(col) {}
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
};*/

struct TriangleGeometry
{
    TriangleVertex vertecies[3];
    //TriangleGeometry() = default;
};

struct MeshGeometry
{
    std::vector<TriangleVertex> vertecies;
    std::vector<int> indicies;
    //TriangleGeometry() = default;
};

struct CoordinateTransformationCBStruct
{
    DirectX::SimpleMath::Matrix world_matrix;
    DirectX::SimpleMath::Matrix view_matrix;
    DirectX::SimpleMath::Matrix projection_matrix;
};

struct MouseMoveEventArgs
{
    DirectX::SimpleMath::Vector2 Position;
    DirectX::SimpleMath::Vector2 Offset;
    int WheelDelta;
};

/*struct RectangleGeometry
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
};*/