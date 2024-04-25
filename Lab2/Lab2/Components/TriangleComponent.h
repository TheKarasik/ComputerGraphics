#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "TriangleColor.h"
#include "TriangleGeometry.h"

class TriangleColor;
class TriangleGeometry;

class TriangleComponent : public GameComponent
{
public:
    TriangleComponent(Game* game, TriangleGeometry Locations, TriangleColor Colors, float HueStep);
    void Update(float h);
    virtual void Draw() /*override*/;
private:
    void GenerateVB();
    void GenerateIB();
    float HueStep;
    TriangleGeometry Locations;
    TriangleColor Colors;
    ID3D11Buffer* vb_;
    ID3D11Buffer* ib_;
    UINT strides;
    UINT offsets;
    float RoundHue(float hue) {return fmod(hue, 360.0f); }
};
