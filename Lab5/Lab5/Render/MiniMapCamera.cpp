#include "MiniMapCamera.h"

#include "Mesh.h"


MiniMapCamera::MiniMapCamera(const DirectX::SimpleMath::Vector3& position,
                             const DirectX::SimpleMath::Vector3& direction, const DirectX::SimpleMath::Vector3& up_direction)
: OrthographicCamera(position, direction, up_direction) {}

void MiniMapCamera::update()
{
    position_.x = Player_->location().x;
    position_.z = Player_->location().z;
    OrthographicCamera::update();
}
