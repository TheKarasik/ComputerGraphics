#pragma once
#include "Renderer.h"

class Drawable
{
public:
    virtual ~Drawable() = default;
    Drawable(Renderer* renderer) : renderer_(renderer)  {visual_objects.push_back(this);}
    virtual void draw() = 0;
    static std::vector<Drawable*> visual_objects;
protected:
    Renderer* renderer_;
    //virtual ~Drawable() = default;
};