#include "Game.h"

#include <fstream>


Game::Game()
{
    display_ = new Display32(L"Lab 2", 800, 800);
    input_ = new InputDevice(this);

    renderer_ = new Renderer(*display_);

    platform = new Platform(*renderer_);

    game_objects.push_back(platform);
}
void Game::GameLoop()
{
    start = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> previous = start;
    lag = 0.0f;
    while (true)
    {
        std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - previous).count() / 1000000.0f ;
        //printf("elapsed: %f\n", elapsed);
        previous = current;
        lag += elapsed;
        
        ProcessInput();
        if (IsEscapePressed) return;

        while (lag >= S_PER_UPDATE)
        {
            //printf("lag: %f\n", lag);
            Update();
            lag -= S_PER_UPDATE;
        }
        renderer_->Render(game_objects);
    }
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
    
}

void Game::ProcessInput()
{
    
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    if (input_->IsKeyDown(Keys::Escape) || msg.message == WM_QUIT) IsEscapePressed = true;
}
