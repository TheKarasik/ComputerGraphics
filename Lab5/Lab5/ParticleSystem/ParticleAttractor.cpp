#include "ParticleAttractor.h"


ParticleAttractor::ParticleAttractor(AttractorStruct attractor) : attractor_(attractor)
{
    Attractors.push_back(attractor_);
}
