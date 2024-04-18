#pragma once
#include <directxmath.h>
class TriangleGeometry
{
public:
    TriangleGeometry(DirectX::XMFLOAT4 GeometryData*);
private:
    DirectX::XMFLOAT4* GeometryData;
};
