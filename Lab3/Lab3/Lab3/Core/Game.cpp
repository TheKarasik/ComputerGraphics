#include "Game.h"

#include <fstream>

#include "AIPlatformController.h"
#include "Ball.h"
#include "PixelShader.h"
#include "Platform.h"
#include "PlayerPlatformController.h"
#include "VertexShader.h"

std::vector<Controllable*> Controllable::changing_objects;
std::vector<Updatable*> Updatable::changing_objects;

Game::Game()
{
    bool player_controlled1 = false;
    bool player_controlled2 = false;
    display_ = new Display32(L"Lab 3", 800, 800, input_);
    input_ = new InputDevice(this);

    renderer_ = new Renderer(*display_);
    ball = new Ball(renderer_);
    left_platform = new Platform(renderer_, player_controlled1, dynamic_cast<Ball*>(ball));
    right_platform = new Platform(renderer_, player_controlled2, dynamic_cast<Ball*>(ball),
        RectangleGeometry{DirectX::XMFLOAT2(0.8, 0), DirectX::XMFLOAT2(0.05, 0.1)});
    
    if (player_controlled1 && player_controlled2)
    {
        auto rp = dynamic_cast<Platform*> (right_platform);
        if (!rp) return;
        auto c = rp->GetController();
        if (!c) return;
        auto ppc = dynamic_cast<PlayerPlatformController*>(c);
        if (!ppc) return;
        ppc->SetUpButton(Keys::Up);
        ppc->SetDownButton(Keys::Down);
    }
}
void Game::GameLoop()
{
    start = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> previous = start;
    lag = 0.0f;
    while (!IsEscapePressed)
    {
        std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - previous).count() / 1000000.0f ;
        //printf("elapsed: %f\n", elapsed);
        previous = current;
        lag += elapsed;
        
        ProcessInput();

        while (lag >= S_PER_UPDATE)
        {
            //printf("lag: %f\n", lag);
            Update();
            lag -= S_PER_UPDATE;
        }
        renderer_->Render();
    }
    CleanUp();
}

void Game::Render()
{
    
}

void Game::Update()
{
    /*
    std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
    float t = std::chrono::duration_cast<std::chrono::microseconds>(current - start).count() / 1000000.0f ;
    GRectangle->Update(t*0.1f);
    */
    for (auto co : Updatable::changing_objects) co->update(elapsed);
}

void Game::ProcessInput()
{
    
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if(input_->IsKeyDown(Keys::Escape)) IsEscapePressed = true;
    for (auto co : Controllable::changing_objects)
        co->Control();
}

void Game::CleanUp()
{
    delete display_;
    delete input_;
    delete renderer_;
    
}
