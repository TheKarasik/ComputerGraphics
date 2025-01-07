#pragma once
#include <SimpleMath.h>


#include "Updatable.h"
#define Pi 3.1415926535f

struct CascadeData;
class Frustrum;
struct MouseMoveEventArgs;

class PerspectiveCamera : public Updatable
{
public:
  PerspectiveCamera() = default;
  PerspectiveCamera(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3);
  PerspectiveCamera (PerspectiveCamera& camera);
  //PerspectiveCamera(DirectX::SimpleMath::Vector3, DirectX::SimpleMath::Vector3,DirectX::SimpleMath::Vector3, float, float, float, float);
  virtual DirectX::SimpleMath::Matrix view_matrix() {return view_matrix_;}
  virtual DirectX::SimpleMath::Matrix projection_matrix() {return perspective_matrix_;}
  DirectX::SimpleMath::Matrix ViewPerspectiveMatrix() const;
  void set_position(DirectX::SimpleMath::Vector3 position) {position_ = position;}
  void set_up_direction(DirectX::SimpleMath::Vector3 up_direction) {up_direction_ = up_direction;}
  void set_focus_position(DirectX::SimpleMath::Vector3 focus_position) {focus_position_ = focus_position;}
  void invert_pitch(bool pitch_inversion) {pitch_inversion_ = pitch_inversion;}
  void provide_reference_camaera(PerspectiveCamera* reference_camera) {reference_camera_ = reference_camera;}
  DirectX::SimpleMath::Vector3 position() {return  position_;}
  DirectX::SimpleMath::Vector3 focus_position() {return focus_position_;}
  DirectX::SimpleMath::Vector3 up_direction() {return up_direction_;}
  PerspectiveCamera* GetNewCameraRepresentingSubfrustrum(float near_plane_percent, float far_plane_percent);
  DirectX::SimpleMath::Matrix GetSubfrustrumProjection(float near_plane_percent, float far_plane_percent);
  CascadeData* cascade_data() {return cascade_data_;}
  //void DivideInSubfrustrums(DirectX::SimpleMath::Vector4 corners[8]);
  //DirectX::SimpleMath::Vector4* GetFrustrumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
  float Far() {return far_;}
  void setFar(float Far) {far_ = Far;}
  float Near() {return near_;}
  void setNear(float Near) {Near == 0 ? near_ = 1 : near_ = Near;}
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
  //DirectX::SimpleMath::Vector3 reflection_position_;
  DirectX::SimpleMath::Vector3 focus_position_;
  DirectX::SimpleMath::Vector3 up_direction_;


  //Frustrum* frustrum_;

protected:
  CascadeData* cascade_data_;
  bool pitch_inversion_ = false;
  PerspectiveCamera* reference_camera_;

public:
  //void Control() override;
  DirectX::SimpleMath::Vector3 right_direction() {return  XMVector3Cross(focus_position_-position_, up_direction_);}
};
