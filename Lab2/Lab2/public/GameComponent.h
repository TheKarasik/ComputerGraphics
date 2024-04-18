#pragma once
#include "Game.h"

class GameComponent
{
public:
    GameComponent(Game* game) : game_(game) {}
protected:
    Game* game_;
};
