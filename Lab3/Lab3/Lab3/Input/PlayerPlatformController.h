#pragma once
#include "InputDevice.h"
#include "Platform.h"
#include "PlayerController.h"

class PlayerPlatformController : public PlayerController
{
public:
    PlayerPlatformController(InputDevice* input_device, Platform* platform); 
    void Control() override;
    void SetUpButton(Keys UpButton);
    void SetDownButton(Keys DownButton);
    Delegate<void> move_up_delegate;
    Delegate<void> move_down_delegate;
private:
    Platform* platform_;
    Keys UpButton_ = Keys::W;
    Keys DownButton_ = Keys::S;
};
