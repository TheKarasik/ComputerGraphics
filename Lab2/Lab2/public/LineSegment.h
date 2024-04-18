#pragma once

#define GOLDEN_RATIO 1.618f

class LineSegment
{
public:
    LineSegment(float BeginPoint, float EndPoint);
    float BeginPoint;
    float EndPoint;
    float GoldenRatioPoint() {return GoldenRatioPoint_;}
    void CalculateGoldenRatioPoint();
private:
    float GoldenRatioPoint_;
};