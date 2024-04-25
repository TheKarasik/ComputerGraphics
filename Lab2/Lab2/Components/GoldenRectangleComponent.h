#pragma once
#include <directxmath.h>
#include <vector>

#include "LineSegment.h"

//#include "GoldenRectangleComponent.h"
#include "TriangleComponent.h"
#include "GameComponent.h"

#define GOLDEN_RATIO 1.618f

class TriangleColor;
class TriangleGeometry;
class TriangleComponent;

class GoldenRectangleComponent : public GameComponent
{
public:
    GoldenRectangleComponent(Game* game, LineSegment XLineSegment, LineSegment YLineSegment,
        bool AreOutTrianglesReturned, uint8_t depth);
    void ProcessGoldenRatio();
    virtual void Update(float t);
    virtual void Draw() /*override*/;
private:
    void ShiftLineSegment(LineSegment* LSegment);
    bool IsXAxisBeingCut;
    bool AreOutTrianglesProcessed;
    LineSegment XLineSegment;
    LineSegment YLineSegment;
    const float BeginHue;
    const float EndHue;
    float CurrentHue;
    float HueStep;
    float freq = 0.001;
    uint8_t depth;
    std::vector<TriangleComponent> Triangles{};
};
