#pragma once
#include <SimpleMath.h>

#include "Controllable.h"
#include "Structs.h"
#include "Updatable.h"

struct MouseMoveEventArgs;

class CameraObject : public Updatable, public Controllable
{
public:
  CameraObject(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3);
  DirectX::SimpleMath::Matrix view_matrix() {return view_matrix_;}
  DirectX::SimpleMath::Matrix perspective_matrix() {return perspective_matrix_;}
  DirectX::SimpleMath::Matrix ViewPerspectiveMatrix() const;
  void update() override;
  void RotateCamera(const MouseMoveEventArgs& event);
private:
  float fov_ = Pi/4;
  float aspect_ratio_ = 1;
  float near_ = 1;
  float far_ = 100;

  float step_linear = 0.1;
  float step_angular = Pi/180/10;
  

  DirectX::SimpleMath::Matrix view_matrix_;
  DirectX::SimpleMath::Matrix perspective_matrix_;
  
  DirectX::SimpleMath::Vector3 position_;
  DirectX::SimpleMath::Vector3 look_direction_;
  DirectX::SimpleMath::Vector3 up_direction_;

  void SetUpMatricies();

public:
  void Control() override;
  DirectX::SimpleMath::Vector3 right_direction() {return  XMVector3Cross(up_direction_, look_direction_);}
};
