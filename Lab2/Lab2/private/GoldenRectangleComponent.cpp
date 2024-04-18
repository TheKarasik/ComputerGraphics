#include "../public/GoldenRectangleComponent.h"



GoldenRectangleComponent::GoldenRectangleComponent(Game* game, LineSegment XLineSegment, LineSegment YLineSegment,
    bool AreOutTrianglesReturned, uint8_t depth) :
    GameComponent(game), XLineSegment(XLineSegment), YLineSegment(YLineSegment), AreOutTrianglesProcessed(AreOutTrianglesReturned),
    IsXAxisBeingCut(true), BeginHue(0.0f), EndHue(360.0f), depth(depth), CurrentHue(BeginHue),
    HueStep((EndHue-CurrentHue)/depth)
{
    for (int i = 0; i < depth; ++i) ProcessGoldenRatio();
}

void GoldenRectangleComponent::ProcessGoldenRatio()
{
    DirectX::XMFLOAT4 Points[3];
    float Hues[3];
    
    Points[0] = DirectX::XMFLOAT4(XLineSegment.BeginPoint, YLineSegment.BeginPoint, 0.5, 1.0);
    Hues[0] = CurrentHue;
    CurrentHue += HueStep;
    if (IsXAxisBeingCut)
    {
        Points[1] = DirectX::XMFLOAT4(XLineSegment.GoldenRatioPoint(), YLineSegment.EndPoint, 0.5, 1.0);
        if (AreOutTrianglesProcessed) Points[2] = DirectX::XMFLOAT4(XLineSegment.BeginPoint, YLineSegment.EndPoint, 0.5, 1.0);
        else Points[2] = DirectX::XMFLOAT4(XLineSegment.GoldenRatioPoint(), YLineSegment.BeginPoint, 0.5, 1.0);
    }
    else
    {
        Points[1] = DirectX::XMFLOAT4(XLineSegment.EndPoint, YLineSegment.GoldenRatioPoint(), 0.5, 1.0);
        if (AreOutTrianglesProcessed) Points[2] = DirectX::XMFLOAT4(XLineSegment.EndPoint, YLineSegment.BeginPoint, 0.5, 1.0);
        else Points[2] = DirectX::XMFLOAT4(XLineSegment.EndPoint, YLineSegment.GoldenRatioPoint(), 0.5, 1.0);
    }
    Hues[1] = CurrentHue;
    Hues[2] = CurrentHue;
    
    TriangleGeometry TGeometry = TriangleGeometry(Points);
    TriangleColor TColor = TriangleColor(Hues);
    
    Triangles.push_back(TriangleComponent(game_, TGeometry, TColor, HueStep));
    
    if (IsXAxisBeingCut) ShiftLineSegment(&XLineSegment);
    else ShiftLineSegment(&YLineSegment);
    
    IsXAxisBeingCut = !IsXAxisBeingCut;
}

void GoldenRectangleComponent::Update(float t)
{
    for (int i = 0; i < Triangles.size(); ++i)
    {
        Triangles[i].Update(t + i*HueStep);
    }
}

void GoldenRectangleComponent::Draw()
{
    for (int i = 0; i < Triangles.size(); ++i)
    {
        Triangles[i].Draw();
    }
}

void GoldenRectangleComponent::ShiftLineSegment(LineSegment* LSegment)
{
    LSegment->BeginPoint = LSegment->EndPoint;
    LSegment->EndPoint = LSegment->GoldenRatioPoint();
    LSegment->CalculateGoldenRatioPoint();
}
