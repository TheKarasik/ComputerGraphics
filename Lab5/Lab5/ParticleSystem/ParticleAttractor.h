#pragma once
#include <vector>

#include "Structs.h"

class ParticleAttractor
{
public:
    ParticleAttractor(AttractorStruct attractor);
    static std::vector<AttractorStruct> Attractors;
    //static std::vector<AttractorStruct> Attractors() {return attractors_;}
private:
    const AttractorStruct attractor_;
};
