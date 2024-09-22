#include "Game.h"

#include <fstream>
#include <math.h>

//#include "Ball.h"
//#include "Platform.h"
#include <random>
//#include "Structs.h"
#include "DirectionalLightComponent.h"
#include "PerspectiveCamera.h"
#include "LightComponent.h"
#include "Mesh.h"
#include "MiniMapCamera.h"
#include "OrthographicCamera.h"
#include "FileTexture.h"
//#include "ParticleAttractor.h"
//#include "ParticleSystem.h"
#include "ParticleAttractor.h"
#include "ParticleSystem.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ThirdPersonPlayer.h"
#include "Renderer.h"
//#include "DirectionalLightComponent.h"

std::vector<Controllable*> Controllable::changing_objects;
std::vector<Updatable*> Updatable::changing_objects;

float Updatable::elapsed;
//std::vector<DirectionalLightComponent*> DirectionalLights_; 

Game::Game()
{
    
    display_ = new Display32(L"Lab 5", 800, 800, input_);
    input_ = new InputDevice(this);
    
    float deg = 3.14/180*0;
    camera_ = new PerspectiveCamera(DirectX::SimpleMath::Vector3(0, 1, 0),
        DirectX::SimpleMath::Vector3(0, sin(deg), cos(deg)), DirectX::SimpleMath::Vector3(0, cos(deg), -sin(deg)));

    //mini_map_camera_ = new MiniMapCamera(DirectX::SimpleMath::Vector3(0,50,0),
    //    DirectX::SimpleMath::Vector3(0,-1,0), DirectX::SimpleMath::Vector3(0,0,1));
    
    renderer_ = new Renderer(*display_, camera_);
    
    point_light = new PointLightComponent(renderer_);
    point_light->SetPosition(DirectX::SimpleMath::Vector3( 0, 2, 10));
    point_light->SetColor(DirectX::SimpleMath::Vector4(0,0,1,1));
    point_light->SetRange(7);

    spot_light = new SpotLightComponent(renderer_);
    spot_light->SetPosition(DirectX::SimpleMath::Vector3( -10, 10, 0));
    spot_light->SetDirection(DirectX::SimpleMath::Vector3(1,-1,0));
    spot_light->SetColor(DirectX::SimpleMath::Vector4(1,0,0,1));
    spot_light->SetRange(50);

    directional_light = new DirectionalLightComponent(renderer_);
    directional_light->SetDirection(DirectX::SimpleMath::Vector3(0,-1,0));
    directional_light->SetIntensity(0.3f);
    directional_light->SetColor(DirectX::SimpleMath::Vector4(1,1,1,1));

    emmiter_sphere = new EmitterSphere();
    emmiter_sphere->max_spawn = 1000;
    emmiter_sphere->position = DirectX::SimpleMath::Vector4(0, 2, -5, 1);
    emmiter_sphere->scale = DirectX::SimpleMath::Vector4(5, 5, 5, 1);
    emmiter_sphere->partitioning = DirectX::SimpleMath::Vector4(1, 1, 1, 1);
    emmiter_sphere->rotation = DirectX::SimpleMath::Matrix::CreateRotationX(0) *
            DirectX::SimpleMath::Matrix::CreateRotationY(0) *
            DirectX::SimpleMath::Matrix::CreateRotationZ(0);
    emmiter_sphere->particles_base_speed = 1;
    emmiter_sphere->particles_life_span = 3;
    emmiter_sphere->particles_weight_start = 1;
    emmiter_sphere->particles_weight_finish = 1;
    emmiter_sphere->color = DirectX::SimpleMath::Color(0.5f, 0.2f, 0.2f, 1.0f);
    emmiter_sphere->particles_size_start = 0.1f;
    emmiter_sphere->particles_size_finish = 0.1f;
    emmiter_sphere->rng_seed = 1;

    /*AttractorStruct attractor_struct;
    attractor_struct.position = DirectX::SimpleMath::Vector4(3,2,0,1);
    attractor_struct.gravity = 1;
    attractor_struct.mass = 1;
    attractor_struct.killZoneRadius = 1;
    attractor_ = new ParticleAttractor(attractor_struct);*/
    
    particle_system_ = new ParticleSystem(*emmiter_sphere,  renderer_); 
    particle_system_->initialize_system();
    renderer_->ProvideParticleSystem(particle_system_);
    
    //particle_system_->set_emitter_data(emmiter_sphere);
    
    //Import Maxwell
    for (int i = 0; i<5; i++) ImportMaxwell();
    
    //Import Goffman
    Importer3D importer_3d2 = Importer3D();
    if (importer_3d2.DoTheImportThing("Models/source/GofmanSphere.fbx"))
    {
        std::vector<Mesh*> imported_meshes = importer_3d2.GetMeshes(renderer_);
        body_texture = new FileTexture(renderer_, L"Models/textures/Gofman.jpg");
        imported_meshes[0]->set_texture(body_texture);
        imported_meshes[0]->set_location(DirectX::XMFLOAT3(0,2,-0));
        imported_meshes[0]->set_scale(DirectX::SimpleMath::Vector3(1,1,1));
        meshes_.insert(meshes_.end(), imported_meshes.begin(), imported_meshes.end());
        sphere = imported_meshes[0];
        //sphere->set_scale(DirectX::XMFLOAT3(1000,1000,1000));
        sphere->set_scale(DirectX::XMFLOAT3(2,2,2));
        sphere->set_bounding_sphere();
        sphere->pick_ups = pick_ups;
        tpp = new ThirdPersonPlayer(sphere, camera_);
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
        body_texture = new FileTexture(renderer_, L"Models/textures/error.jpg");
        imported_meshes[0]->set_texture(body_texture);
        imported_meshes[0]->set_scale(DirectX::XMFLOAT3(50,50,50));
        meshes_.insert(meshes_.end(), imported_meshes.begin(), imported_meshes.end());
        plane = imported_meshes[0];
        //plane->flip_normals();
        //plane->add_rotation(DirectX::SimpleMath::Vector3(1,0,0), Pi);
    }
    
    /*for (const auto m : meshes_)
    {
        m->set_scale(DirectX::SimpleMath::Vector3(10,10,10));
    }*/
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
        Updatable::elapsed = elapsed;
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
        body_texture = new FileTexture(renderer_, L"Models/textures/dingus_nowhiskers.jpg");
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
