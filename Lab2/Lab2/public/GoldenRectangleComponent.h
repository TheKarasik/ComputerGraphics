#pragma once
#include <directxmath.h>
#include <vector>
#include "TriangleGeometry.h"

#include "LineSegment.h"

#include "GoldenRectangleComponent.h"
#include "LineSegment.h"
#include "TriangleComponent.h"

#define GOLDEN_RATIO 1.618f

class GoldenRectangleComponent : public GameComponent
{
public:
    GoldenRectangleComponent(Game* game, LineSegment XLineSegment, LineSegment YLineSegment,
        bool AreOutTrianglesReturned, uint8_t depth);
    void ProcessGoldenRatio();
    void ShiftLineSegment(LineSegment* LSegment);
private:
    bool IsXAxisBeingCut;
    bool AreOutTrianglesProcessed;
    LineSegment XLineSegment;
    LineSegment YLineSegment;
    const float BeginHue;
    const float EndHue;
    float CurrentHue;
    float HueStep;
    uint8_t depth;
    std::vector<TriangleComponent> Triangles{};
};
