#include "SpotLightComponent.h"

#include <chrono>
#include <random>

#include "Mesh.h"

SpotLightComponent::SpotLightComponent(Renderer* renderer) : LightComponent(renderer)
{
    light_data.Type = SpotLight;
    SetupMesh();
    Lights.push_back(this);
}

void SpotLightComponent::SetDirection(DirectX::SimpleMath::Vector3 DirectionVector)
{
    //light_data.DirectionWS = DirectX::SimpleMath::Vector4(DirectionVector.x, DirectionVector.y, DirectionVector.z, 0);
    LightComponent::SetDirection(DirectionVector);
    volume_ = nullptr;
    SetupMesh();
}

void SpotLightComponent::SetRange(float Range)
{
    volume_ = nullptr;
    light_data.Range = Range;
    SetupMesh();
}

void SpotLightComponent::SetupMesh()
{
    MeshGeometry* cone_geom = new MeshGeometry();
    cone_geom->vertecies.push_back(light_data.PositionWS);

    DirectX::SimpleMath::Vector4 a = GetRandomPerpendicular(light_data.DirectionWS);
    DirectX::SimpleMath::Vector4 b;
    {
        DirectX::SimpleMath::Vector3 v1 = DirectX::SimpleMath::Vector3(light_data.DirectionWS.x,
            light_data.DirectionWS.y, light_data.DirectionWS.z);
        DirectX::SimpleMath::Vector3 v2 = DirectX::SimpleMath::Vector3(a.x, a.y, a.z);
        DirectX::SimpleMath::Vector3 res = v1.Cross(v2);
        b = DirectX::SimpleMath::Vector4(res.x, res.y, res.z, 0);
    }
    
    //root->vertecies[i].position = light_data.PositionWS;
    //DirectX::SimpleMath::Vector4 root_proj = DirectX::SimpleMath::Vector4(1,0,0,1);
    DirectX::SimpleMath::Vector4 root_proj = light_data.PositionWS + light_data.DirectionWS * light_data.Range;
    float radius = light_data.Range * tan(Pi/180.0f*light_data.SpotlightAngle);
    float phi = 0;
    float delta_phi = 2*Pi/n;
    //Заполняем точки на окружности
    for (int i = 1; i <= n; i++)
    {
        DirectX::SimpleMath::Vector4 point_on_base = root_proj + radius*cos(phi)*a + radius*sin(phi)*b;
        cone_geom->vertecies.push_back(point_on_base);
        phi += delta_phi;
    }
    //Собираем треугольники на боковой поверхности
    float index_on_base = 1;
    for (int i = 0; i < n; i++)
    {
        cone_geom->indicies.push_back(0);
        cone_geom->indicies.push_back(index_on_base);
        if (i == n - 1) cone_geom->indicies.push_back(1);
        else cone_geom->indicies.push_back(++index_on_base);
    }
    //Собираем треугольники на основании
    index_on_base = 2;
    for (int i = 0; i < n-2; i++)
    {
        cone_geom->indicies.push_back(1);
        cone_geom->indicies.push_back(++index_on_base);
        cone_geom->indicies.push_back(index_on_base-1);
    }

    DirectX::SimpleMath::Quaternion q;
    {
        DirectX::SimpleMath::Vector3 v1 = DirectX::SimpleMath::Vector3(light_data.DirectionWS.x,
            light_data.DirectionWS.y, light_data.DirectionWS.z);
        DirectX::SimpleMath::Vector3 v2 = DirectX::SimpleMath::Vector3(1,0,0);
        DirectX::SimpleMath::Vector3 cp = v1.Cross(v2);
        q = DirectX::SimpleMath::Quaternion(cp.x, cp.y, cp.z, 0);
        q.w = sqrtf(powf(v1.Length(), 2) + (powf(v2.Length(), 2))) + v1.Dot(v2);
        q.Normalize();
    }
    volume_ = new Mesh(renderer_);
    //Cone->set_scale(DirectX::SimpleMath::Vector3(light_data.Range, 1, 1));
    //volume_->set_rotation(q);
    //Cone->set_location(DirectX::SimpleMath::Vector3(light_data.PositionWS.x, light_data.PositionWS.y, light_data.PositionWS.z));
    volume_->set_meshes(cone_geom);
    volume_->set_drawability(false);
}

DirectX::SimpleMath::Vector4 SpotLightComponent::GetRandomPerpendicular(DirectX::SimpleMath::Vector4 v4)
{
    bool bx0 = false, by0 = false, bz0 = false;
    float x, y, z = 0;
    if (v4.x == 0) bx0 = true;
    if (v4.y == 0) by0 = true;
    if (v4.z == 0) bz0 = true;
    DirectX::SimpleMath::Vector4 ret = DirectX::SimpleMath::Vector4(0,0,0,0);
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::default_random_engine eng(seed);
    std::uniform_real_distribution<> distr(-1, 1);
    
    if (bx0 && by0 && bz0) return ret;
    if (bx0 && by0)
    {
        ret.x = static_cast<float>(distr(eng));
        ret.y = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    if (bx0 && bz0)
    {
        ret.x = static_cast<float>(distr(eng));
        ret.z = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    if (by0 && bz0)
    {
        ret.y = static_cast<float>(distr(eng));
        ret.z = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    if (bx0)
    {
        ret.x = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    if (by0)
    {
        ret.y = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    if (bz0)
    {
        ret.z = static_cast<float>(distr(eng));
        ret.Normalize();
        return ret;
    }
    
    ret.x = static_cast<float>(distr(eng));
    ret.y = static_cast<float>(distr(eng));
    ret.z = -(ret.x*v4.x + ret.y*v4.y)/v4.z;
    return ret;
}
