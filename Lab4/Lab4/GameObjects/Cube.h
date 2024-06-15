#pragma once
#include "Object3D.h"
#include "Triangle.h"

class Cube : public Object3D
{
public:
    Cube(Renderer* renderer, Object3D* parent);
    //Cube(Renderer* renderer, DirectX::XMFLOAT4 color);
private:
    Triangle* triangles_[12];
};
