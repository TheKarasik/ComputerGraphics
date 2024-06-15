#include "Game.h"

#include <fstream>
#include <math.h>

//#include "Ball.h"
//#include "Platform.h"
#include "Cube.h"
#include "Renderer.h"
#include "CameraObject.h"
#include "Icosahedron.h"
#include "OrbitalMovement.h"

std::vector<Controllable*> Controllable::changing_objects;
std::vector<Updatable*> Updatable::changing_objects;

Game::Game()
{
    display_ = new Display32(L"Lab 4", 800, 800, input_);
    input_ = new InputDevice(this);
    
    float deg = 3.14/180*0;
    /*camera_ = new CameraObject(DirectX::SimpleMath::Vector3(0, 0, -20),
        DirectX::SimpleMath::Vector3(0, sin(deg), cos(deg)), DirectX::SimpleMath::Vector3(0, cos(deg), -sin(deg)));*/
    camera_ = new CameraObject(DirectX::SimpleMath::Vector3(0,40,0),
        DirectX::SimpleMath::Vector3(0,-1, 0), DirectX::SimpleMath::Vector3(1, 0, 0));
    
    renderer_ = new Renderer(*display_, camera_);
    
    //JustCenter = new Object3D(renderer_, nullptr);
    JustCenter = new Cube(renderer_, nullptr);
    
    for (int i = 0; i < 2; i++)
    {
        Cubes.push_back(new Cube(renderer_, JustCenter));
        orbital_movement_.push_back(new OrbitalMovement(Cubes[i]));
    }
    for (int i = 0; i < 2; i++)
    {
        Icosahedrons.push_back(new Icosahedron(renderer_, Cubes[i]));
        orbital_movement_.push_back(new OrbitalMovement(Icosahedrons[i]));
    }
    //orbital_movement_[0]->multiply_angular_speed(20);

    
    
    /*JustCenter = new Cube(renderer_, nullptr);
    ParentCube = new Cube(renderer_, JustCenter);
    ChildrenCube = new Cube(renderer_, ParentCube);*/

    //orbital_movement_.push_back(new OrbitalMovement(ParentCube));
    //ParentCube->set_location(DirectX::XMFLOAT3(-5, 0, 0));
    //orbital_movement_.push_back(new OrbitalMovement(ChildrenCube));

    /*JustCenter->set_location(DirectX::XMFLOAT3(-2, 0, 0));
    ParentCube->set_location(DirectX::XMFLOAT3(-5, 0, 0));
    ChildrenCube->set_location(DirectX::XMFLOAT3(10, 0, 0));*/
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

void Game::Update()
{
    /*
    std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
    float t = std::chrono::duration_cast<std::chrono::microseconds>(current - start).count() / 1000000.0f ;
    GRectangle->Update(t*0.1f);
    */
    for (auto co : Updatable::changing_objects) co->update();
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
