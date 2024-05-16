#pragma once
#include "RectangleObject.h"
#include "Controllable.h"
#include "InputDevice.h"

class Ball;

class Platform : public RectangleObject
{
public:
    Platform(Renderer* renderer, bool player_controlled, Ball* ball, RectangleGeometry rect =
        RectangleGeometry{DirectX::XMFLOAT2(-0.8, 0), DirectX::XMFLOAT2(0.05, 0.1)});
    void MoveUp();
    void MoveDown();
    void update(float elapsed) override;
    Controllable* GetController() {return controller;}
private:
    float step = 0.005;
    Controllable* controller;
    Ball* ball_;
};
