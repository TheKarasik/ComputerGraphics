#include "../public/TriangleComponent.h"

TriangleComponent::TriangleComponent(Game* game, TriangleGeometry Locations, TriangleColor Colors) :
    GameComponent(game),
    Locations(Locations),
    Colors(Colors){ }