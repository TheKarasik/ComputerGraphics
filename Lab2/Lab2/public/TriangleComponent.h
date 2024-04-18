#pragma once
#include "Game.h"
#include "GameComponent.h"
#include "TriangleColor.h"

class TriangleComponent : public GameComponent
{
public:
    TriangleComponent(Game* game, TriangleGeometry Locations, TriangleColor Colors);
private:
    TriangleGeometry Locations;
    TriangleColor Colors;
};
