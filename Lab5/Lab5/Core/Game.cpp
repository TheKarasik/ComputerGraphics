#include "Game.h"

#include <fstream>
#include <math.h>

//#include "Ball.h"
//#include "Platform.h"
#include <random>

#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "MiniMapCamera.h"
#include "OrthographicCamera.h"
#include "Texture.h"
#include "ThirdPersonPlayer.h"

std::vector<Controllable*> Controllable::changing_objects;
std::vector<Updatable*> Updatable::changing_objects;

Game::Game()
{
    display_ = new Display32(L"Lab 5", 800, 800, input_);
    input_ = new InputDevice(this);
    
    float deg = 3.14/180*0;
    camera_ = new PerspectiveCamera(DirectX::SimpleMath::Vector3(0, 1, -10),
        DirectX::SimpleMath::Vector3(0, sin(deg), cos(deg)), DirectX::SimpleMath::Vector3(0, cos(deg), -sin(deg)));

    //mini_map_camera_ = new MiniMapCamera(DirectX::SimpleMath::Vector3(0,50,0),
    //    DirectX::SimpleMath::Vector3(0,-1,0), DirectX::SimpleMath::Vector3(0,0,1));
    
    renderer_ = new Renderer(*display_, camera_);
    light = new LightComponent(renderer_);
    renderer_->SetUpLightAndShadows(light);
    //renderer_->SetMiniMapCamera(mini_map_camera_);
    
    //Import Maxwell
    for (int i = 0; i<5; i++) ImportMaxwell();
    
    //Import Goffman
    Importer3D importer_3d2 = Importer3D();
    if (importer_3d2.DoTheImportThing("Models/source/GofmanSphere.fbx"))
    {
        std::vector<Mesh*> imported_meshes = importer_3d2.GetMeshes(renderer_);
        body_texture = new Texture(renderer_, L"Models/textures/Gofman.jpg");
        imported_meshes[0]->set_texture(body_texture);
        imported_meshes[0]->set_location(DirectX::XMFLOAT3(0,2,-12));
        meshes_.insert(meshes_.end(), imported_meshes.begin(), imported_meshes.end());
        sphere = imported_meshes[0];
        sphere->set_scale(DirectX::XMFLOAT3(2,2,2));
        sphere->set_bounding_sphere();
        sphere->pick_ups = pick_ups;
        //tpp = new ThirdPersonPlayer(sphere, camera_);
    }
    for (const auto m : meshes_)
    {
        m->add_rotation(DirectX::XMFLOAT3(0,1,0), Pi/2);
    }
    tpp = new ThirdPersonPlayer(sphere, camera_);
    //mini_map_camera_->SetPlayer(sphere);

    //Import Plane
    Importer3D importer_3d3 = Importer3D();
    if (importer_3d3.DoTheImportThing("Models/source/ErrorPlane.fbx"))
    {
        std::vector<Mesh*> imported_meshes = importer_3d3.GetMeshes(renderer_);
        body_texture = new Texture(renderer_, L"Models/textures/error.jpg");
        imported_meshes[0]->set_texture(body_texture);
        imported_meshes[0]->set_scale(DirectX::XMFLOAT3(50,50,50));
        meshes_.insert(meshes_.end(), imported_meshes.begin(), imported_meshes.end());
        plane = imported_meshes[0];
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

void Game::Update()
{
    /*std::chrono::time_point<std::chrono::steady_clock> current = std::chrono::steady_clock::now();
    float t = std::chrono::duration_cast<std::chrono::microseconds>(current - start).count() / 1000000.0f ;
    GRectangle->Update(t*0.1f);*/
    
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

void Game::ImportMaxwell()
{
    Importer3D importer_3d1 = Importer3D();
    if (importer_3d1.DoTheImportThing("Models/source/Maxwell.fbx"))
    {
        std::vector<Mesh*> imported_meshes = importer_3d1.GetMeshes(renderer_);
        //meshes_[0]->set_parent(meshes_[1]);
        body_texture = new Texture(renderer_, L"Models/textures/dingus_nowhiskers.jpg");
        imported_meshes[0]->set_texture(body_texture);
        imported_meshes[0]->set_scale(DirectX::XMFLOAT3(0.1, 0.1, 0.1));
        
        auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::default_random_engine eng(seed);
        std::uniform_real_distribution<> pos_distr(-50, 50);
        imported_meshes[0]->set_location(DirectX::XMFLOAT3(pos_distr(eng), 0, pos_distr(eng)));
        
        imported_meshes[0]->set_bounding_sphere();
        meshes_.insert(meshes_.end(), imported_meshes.begin(), imported_meshes.end());
        pick_ups.push_back(imported_meshes[0]);


    }
}
