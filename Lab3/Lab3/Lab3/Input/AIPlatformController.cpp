#include "AIPlatformController.h"
#include "Ball.h"
#include "Platform.h"

AIPlatformController::AIPlatformController(InputDevice* input_device, Platform* platform, Ball* ball)
    : Controllable(input_device), platform_(platform), ball_(ball) {}

void AIPlatformController::Control()
{
    if (ball_->rect().pos.y > platform_->rect().pos.y) platform_->MoveUp();
    else if (ball_->rect().pos.y < platform_->rect().pos.y) platform_->MoveDown();
}

