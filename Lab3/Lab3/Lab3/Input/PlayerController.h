#pragma once
#include <map>

#include "Controllable.h"

class PlayerController : public Controllable
{
public:
    virtual ~PlayerController() {delete layout_;}

    PlayerController(InputDevice* input_device, std::map<Keys, Delegate<void>>* layout)
    : Controllable(input_device), layout_(layout) {}

protected:
    std::map<Keys, Delegate<void>>* layout_;
};
