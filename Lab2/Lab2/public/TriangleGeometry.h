#pragma once
#include <directxmath.h>
//#include <intsafe.h>

class TriangleGeometry
{
public:
    TriangleGeometry(DirectX::XMFLOAT4* GeometryData);
    DirectX::XMFLOAT4* GeometryData() {return GeometryData_;}
private:
    DirectX::XMFLOAT4* GeometryData_;
};
