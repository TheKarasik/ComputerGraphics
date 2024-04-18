#include "../public/LineSegment.h"

LineSegment::LineSegment(float BeginPoint, float EndPoint) :
    BeginPoint(BeginPoint),
    EndPoint(EndPoint) { CalculateGoldenRatioPoint(); }

void LineSegment::CalculateGoldenRatioPoint()
{
    if (BeginPoint < EndPoint) GoldenRatioPoint_ = BeginPoint + (EndPoint - BeginPoint)/GOLDEN_RATIO;
    else GoldenRatioPoint_ = EndPoint - (EndPoint - BeginPoint)/GOLDEN_RATIO;
}
