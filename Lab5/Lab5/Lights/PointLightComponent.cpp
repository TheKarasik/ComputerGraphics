#include "PointLightComponent.h"

#include "Importer3D.h"
#include "Mesh.h"

PointLightComponent::PointLightComponent(Renderer* renderer) : LightComponent(renderer)
{
    Importer3D importer = Importer3D();
    if (importer.DoTheImportThing("Models/source/GofmanSphere.fbx"))
    {
        std::vector<Mesh*> imported_meshes = importer.GetMeshes(renderer_);
        volume_ = imported_meshes[0];
        //volume_->set_location(DirectX::SimpleMath::Vector3(0,0,0));
        volume_->set_drawability(false);
    }
    light_data->PositionWS = light_data->PositionWS + DirectX::SimpleMath::Vector3(light_data->Range, 0, 0);
    light_data->Type = PointLight;

    Lights.push_back(this);
}

void PointLightComponent::SetRange(float Range)
{
    DirectX::SimpleMath::Vector3 v3 = DirectX::SimpleMath::Vector3(light_data->PositionWS.x, light_data->PositionWS.y, light_data->PositionWS.z);
    float RangeMultiplier = Range/light_data->Range;
    volume_->set_scale(DirectX::SimpleMath::Vector3(RangeMultiplier, RangeMultiplier, RangeMultiplier));
    //volume_->set_location(v3 - DirectX::SimpleMath::Vector3(0, RangeMultiplier, 0));
    volume_->set_location(v3);
    
    LightComponent::SetRange(Range);
}

void PointLightComponent::SetPosition(DirectX::SimpleMath::Vector3 new_position)
{
    LightComponent::SetPosition(new_position);
    volume_->set_location(new_position);
    //volume_->set_location(new_position - DirectX::SimpleMath::Vector3(0, light_data.Range, 0));
}
