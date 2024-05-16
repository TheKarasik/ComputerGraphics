#include "Platform.h"

#include "AIPlatformController.h"
#include "Ball.h"
#include "PlayerPlatformController.h"

Platform::Platform(Renderer* renderer, bool player_controlled, Ball* ball, RectangleGeometry rect) :
    RectangleObject(renderer, rect), ball_(ball)
    /*controller(player_controlled ?
    new PlayerPlatformController(InputDevice::singleton, this) :
    new AIPlatformController(InputDevice::singleton, this, ball))*/
{
    if (player_controlled) controller = new PlayerPlatformController(InputDevice::singleton, this);
    else controller = new AIPlatformController(InputDevice::singleton, this, ball);
}

void Platform::MoveUp() { rect_.pos.y += step; }

void Platform::MoveDown() { rect_.pos.y -= step; }

void Platform::update(float elapsed)
{
    if (rect_.DetectCollision(ball_->rect()))
    {
        ball_->XReverseVelocityVector();
        ball_->IncreaseVelocityAmplitude();
    }
    if (rect_.DetectVerticalBorderHit())
    {
        if (rect_.pos.y > 0) rect_.pos.y = 1 - rect_.size.y;
        else if  (rect_.pos.y < 0) rect_.pos.y = -1 + rect_.size.y;
    }
    RectangleObject::update(elapsed);
}

