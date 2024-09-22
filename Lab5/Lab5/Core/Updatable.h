#pragma once
#include <vector>

class Updatable
{
public:
    Updatable() {changing_objects.push_back(this);}
    virtual void update() = 0;
    static float elapsed;
    static std::vector<Updatable*> changing_objects;
protected:
    virtual ~Updatable() = default;
};
