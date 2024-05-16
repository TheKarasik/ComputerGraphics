#pragma once
#include "Game.h"
#include "Updatable.h"

class Controllable
{
public: 
    Controllable(InputDevice* controller) : input_device_(controller) {changing_objects.push_back(this);}
    virtual void Control() = 0;
    static std::vector<Controllable*> changing_objects;
protected:
    ~Controllable() = default;
    InputDevice* input_device_;
};