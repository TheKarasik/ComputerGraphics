﻿#pragma once
//#include "Game.h"

class Game;

class GameComponent
{
public:
    //virtual ~GameComponent() = default;
    GameComponent(Game* game) : game_(game) {}
    //virtual void Draw();
protected:
    Game* game_;
};
