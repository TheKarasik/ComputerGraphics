#include "Ball.h"

#include <random>

#include "Platform.h"

Ball::Ball(Renderer* renderer, RectangleGeometry rect) :
RectangleObject(renderer, rect)
{
   Spawn();
}

void Ball::Spawn()
{
    rect_.pos = DirectX::XMFLOAT2(0,0);
    velocity_vector = DirectX::XMFLOAT2(0, 0);
    
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::default_random_engine eng(seed);
    std::uniform_real_distribution<> distr(-1, 1);
    
    while (velocity_vector.x == 0) velocity_vector.x = static_cast<float>(distr(eng));
    velocity_vector.y = static_cast<float>(distr(eng));
    float len = sqrtf(pow(velocity_vector.x, 2) + pow(velocity_vector.y, 2));
    velocity_vector.x /= len;
    velocity_vector.y /= len;

    velocity_amplitude = 1.5;
}

void Ball::update(float elapsed)
{
    if(rect_.DetectVerticalBorderHit()) velocity_vector.y *= -1;
    if(rect_.DetectHorizontalBorderHit())
    {
        if (rect_.pos.x < 0) right_score++;
        else if (rect_.pos.x > 0) left_score++;
        Spawn();
        printf("left score: %d, right score %d \n", left_score, right_score);
    }
    rect_.pos.x += elapsed * velocity_vector.x * velocity_amplitude;
    rect_.pos.y += elapsed * velocity_vector.y * velocity_amplitude;
    RectangleObject::update(elapsed);
}
