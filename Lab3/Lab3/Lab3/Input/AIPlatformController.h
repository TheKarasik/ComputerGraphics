#pragma once

#include "Controllable.h"

class Platform;
class Ball;

class AIPlatformController : public Controllable
{
public:
    AIPlatformController(InputDevice* input_device, Platform* platform, Ball* ball);
    void Control() override;
private:
    Ball* ball_;
    Platform* platform_;
};
