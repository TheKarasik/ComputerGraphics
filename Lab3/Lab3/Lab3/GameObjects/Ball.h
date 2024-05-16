#pragma once
#include "RectangleObject.h"

class Ball : public RectangleObject
{
public:
    Ball(Renderer* renderer, RectangleGeometry rect_ =
    RectangleGeometry{DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(0.05, 0.05)});
    void update(float elapsed) override;
    void XReverseVelocityVector() {velocity_vector.x *= -1;}
    void IncreaseVelocityAmplitude() {velocity_amplitude += 0.5;}
    void Spawn();
private:
    DirectX::XMFLOAT2 velocity_vector = DirectX::XMFLOAT2(0, 0);
    float velocity_amplitude;
    unsigned int left_score = 0;
    unsigned int right_score = 0;
};
