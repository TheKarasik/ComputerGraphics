#include "PlayerPlatformController.h"

PlayerPlatformController::PlayerPlatformController(InputDevice* input_device, Platform* platform)
    : PlayerController(input_device, new std::map<Keys, Delegate<void>>), platform_ (platform)
{
    move_up_delegate.BindRaw(platform, &Platform::MoveUp);
    move_down_delegate.BindRaw(platform, &Platform::MoveDown);
    
    layout_->emplace(UpButton_, move_up_delegate);
    layout_->emplace(DownButton_, move_down_delegate);
}

void PlayerPlatformController::Control()
{
    for (auto it = layout_->begin(); it != layout_->end(); ++it)
        if (input_device_->IsKeyDown(it->first))
            it->second.Execute();
}

void PlayerPlatformController::SetUpButton(Keys UpButton)
{
    auto f = layout_->at(UpButton_);
    layout_->erase(UpButton_);
    UpButton_ = UpButton;
    layout_->emplace(UpButton_, f);
}

void PlayerPlatformController::SetDownButton(Keys DownButton)
{
    auto f = layout_->at(DownButton_);
    layout_->erase(DownButton_);
    DownButton_ = DownButton;
    layout_->emplace(DownButton_, f);
}
