#include "Frustrum.h"

#include "DirectionalLightComponent.h"
#include "PerspectiveCamera.h"
#include "Structs.h"

Frustrum::Frustrum(PerspectiveCamera* camera) : camera_(camera)
{
    //const auto view = camera->view_matrix().Transpose();
    //const auto proj = camera->projection_matrix().Transpose();
    const auto view = camera->view_matrix();
    const auto proj = camera->projection_matrix();
    
    const auto viewProj = view * proj;
    far_plane = camera->Far();
    const auto inv = viewProj.Invert();
   
    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int z = 0; z < 2; ++z)
            {
                auto cur_vec = DirectX::SimpleMath::Vector4((x * 2) - 1, (y * 2) - 1, z, 1);
                DirectX::SimpleMath::Vector4 corner = DirectX::XMVector4Transform(cur_vec, inv);
                //DirectX::SimpleMath::Vector4 corner = DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4(2*x-1, 2*y-1, z, 1), inv);
                frustrumCorners[4*x+2*y+z] = corner/corner.w;
            }
        }
    }
    
    DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3::Zero;
    for (const auto& v : frustrumCorners)
    {
        center += DirectX::SimpleMath::Vector3(v.x, v.y, v.z);
    }
    center /= 8;
    //DirectX::SimpleMath::Vector3 centerWS = DirectX::XMVector4Transform(center, view.Invert());
    lightView = XMMatrixLookAtLH(center,
        center + DirectionalLightComponent::DirectionalLightData()->DirectionWS,
        DirectionalLightComponent::GetDirectionalLight()->up_direction());
    
}

Frustrum::Frustrum(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    const auto viewProj = view * proj;
    const auto inv = viewProj.Invert();
   
    for (int x = 0; x < 2; ++x)
    {
        for (int y = 0; y < 2; ++y)
        {
            for (int z = 0; z < 2; ++z)
            {
                auto cur_vec = DirectX::SimpleMath::Vector4((x * 2) - 1, (y * 2) - 1, z, 1);
                DirectX::SimpleMath::Vector4 corner = DirectX::XMVector4Transform(cur_vec, inv);
                //DirectX::SimpleMath::Vector4 corner = DirectX::SimpleMath::Vector4::Transform(DirectX::SimpleMath::Vector4(2*x-1, 2*y-1, z, 1), inv);
                frustrumCorners[4*x+2*y+z] = corner/corner.w;
            }
        }
    }

    DirectX::SimpleMath::Vector3 center = DirectX::SimpleMath::Vector3::Zero;
    for (const auto& v : frustrumCorners)
    {
        center += DirectX::SimpleMath::Vector3(v.x, v.y, v.z);
    }
    center /= 8;
    //center += camera_->position();
    //center = XMVector4Transform(center, view.Invert());
    lightView = XMMatrixLookAtLH(center,
        center + DirectionalLightComponent::DirectionalLightData()->DirectionWS,
        DirectionalLightComponent::GetDirectionalLight()->up_direction());
        //DirectX::SimpleMath::Vector3::Forward);
}

void Frustrum::GenerateCascadeData()
{
    for (int i = 0; i < CASCADE_COUNT; i++)
    {
        /*const auto subfrustrum_proj = camera_->GetSubfrustrumProjection(0,
            static_cast<float>((i+1))/CASCADE_COUNT);*/
        const auto subfrustrum_proj = camera_->GetSubfrustrumProjection(static_cast<float>(i) / CASCADE_COUNT,
            static_cast<float>((i+1))/CASCADE_COUNT);
        Frustrum subfrustrum(camera_->view_matrix(), subfrustrum_proj);
        cascade_data_->viewProjMats[i] = (subfrustrum.lightView * subfrustrum.CalculateLightProj(subfrustrum.lightView)).Transpose();
        cascade_data_->distances[i] = camera_->Far()*static_cast<float>((i+1))/CASCADE_COUNT;
    }
}

DirectX::SimpleMath::Matrix Frustrum::CalculateLightProj(DirectX::SimpleMath::Matrix ParentsLightView)
{
    float minX = FLT_MAX;
    float maxX = FLT_MIN;
    float minY = FLT_MAX;
    float maxY = FLT_MIN;
    float minZ = FLT_MAX;
    float maxZ = FLT_MIN;
    std::vector<DirectX::SimpleMath::Vector4> frustrumCornersLS;
    for (const auto& v : frustrumCorners)
    {
        const auto trf = DirectX::SimpleMath::Vector4::Transform(v, ParentsLightView);
        frustrumCornersLS.push_back(trf);
        //const auto trf = v;
        minX = min(minX, trf.x);
        maxX = max(maxX, trf.x);
        minY = min(minY, trf.y);
        maxY = max(maxY, trf.y);
        minZ = min(minZ, trf.z);
        maxZ = max(maxZ, trf.z);
    }

    constexpr float zMult = 10.0f;
    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

    return DirectX::XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);
}
