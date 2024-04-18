#pragma once
#include <directxmath.h>
class TriangleColor
{
public:
    TriangleColor(float* Hues);
    TriangleColor();
private:
    DirectX::XMFLOAT4 ColorData[3];
    DirectX::XMFLOAT4 HSV2RGB(float Hue);
};
