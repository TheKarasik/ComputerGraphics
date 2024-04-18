#pragma once
#include <directxmath.h>
#include <cmath>

class TriangleColor
{
public:
    TriangleColor(float* Hues);
    DirectX::XMFLOAT4* ColorData() {return ColorData_;}
private:
    DirectX::XMFLOAT4* ColorData_;
    DirectX::XMFLOAT4 HSV2RGB(float Hue);
};
