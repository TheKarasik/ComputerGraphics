#pragma once
class Renderer;
class Game;

class GameComponent
{
public:
    virtual ~GameComponent() = default;
    GameComponent(Renderer& renderer);// : game_(game) {}
    virtual void draw() = 0;
protected:
    Renderer& renderer_;
};