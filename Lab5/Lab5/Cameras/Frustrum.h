#pragma once
#include <SimpleMath.h>

#include "Structs.h"

class PerspectiveCamera;

class Frustrum
{
public:
    Frustrum(PerspectiveCamera* camera);
    Frustrum(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
    void ProvideCascadeData(CascadeData* cascade_data) {cascade_data_ = cascade_data;}
    void GenerateCascadeData();
private:
    DirectX::SimpleMath::Matrix CalculateLightProj(DirectX::SimpleMath::Matrix ParentsLightView);
    //Frustrum* Subfrustrums[CASCADE_COUNT] = {};
    PerspectiveCamera* camera_;
    DirectX::SimpleMath::Vector4 frustrumCorners[8];
    DirectX::SimpleMath::Matrix lightView;
    DirectX::SimpleMath::Matrix lightProj;
    CascadeData* cascade_data_;
    float far_plane;
};
