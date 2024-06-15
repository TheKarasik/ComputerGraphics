#pragma once
#include <SimpleMath.h>

#include "Controllable.h"
#include "Structs.h"
#include "Updatable.h"

struct MouseMoveEventArgs;

class CameraObject : public Updatable
{
public:
  CameraObject() = default;
  CameraObject(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3);
  virtual DirectX::SimpleMath::Matrix view_matrix() {return view_matrix_;}
  virtual DirectX::SimpleMath::Matrix perspective_matrix() {return perspective_matrix_;}
  DirectX::SimpleMath::Matrix ViewPerspectiveMatrix() const;
  void set_position(DirectX::SimpleMath::Vector3 position) {position_ = position;}
  void set_up_direction(DirectX::SimpleMath::Vector3 up_direction) {up_direction_ = up_direction;}
  void set_focus_position(DirectX::SimpleMath::Vector3 focus_position) {focus_position_ = focus_position;}
  DirectX::SimpleMath::Vector3 position() {return  position_;}
  DirectX::SimpleMath::Vector3 focus_position() {return focus_position_;}
  DirectX::SimpleMath::Vector3 up_direction() {return up_direction_;}
  void SetUpMatricies();
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
  DirectX::SimpleMath::Vector3 focus_position_;
  DirectX::SimpleMath::Vector3 up_direction_;



public:
  //void Control() override;
  DirectX::SimpleMath::Vector3 right_direction() {return  XMVector3Cross(focus_position_-position_, up_direction_);}
};
