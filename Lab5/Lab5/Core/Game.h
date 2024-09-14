#pragma once
#include "Display32.h"
#include "InputDevice.h"
#include <chrono>
#include "Importer3D.h"
#include "PointLightComponent.h"

//#include "Cube.h"

#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "assimp-vc143-mtd.lib")

#define S_PER_UPDATE 0.016

class DirectionalLightComponent;
class SpotLightComponent;
class MiniMapCamera;
class OrthographicCamera;
class LightComponent;
class ThirdPersonPlayer;
class Object3D;
class FileTexture;
class Renderer;
class PerspectiveCamera;

class Game
{
public:
    Game();
    InputDevice* Input() {return input_;}
    Display32* Display() {return display_;}
    void GameLoop();
private:
    bool single_player = false;
    void Render();
    void Update();
    void ProcessInput();
    void CleanUp();
    void ImportMaxwell();
    PointLightComponent* point_light;
    SpotLightComponent* spot_light;
    DirectionalLightComponent* directional_light;
    DirectionalLightComponent* directional_light2;
    ThirdPersonPlayer* tpp;
    std::vector<Mesh*> meshes_;
    std::vector<Mesh*> pick_ups;
    Mesh* sphere;
    Mesh* plane;
    FileTexture* body_texture;
    FileTexture* whiskers_texture;
    PerspectiveCamera* camera_;
    //MiniMapCamera* mini_map_camera_;
    Renderer* renderer_;
    InputDevice* input_;
    Display32* display_;
    std::chrono::time_point<std::chrono::steady_clock> start;
    bool IsEscapePressed = false;
    //GoldenRectangleComponent* GRectangle;
    float lag;
    float elapsed;
    MSG msg = {};
};
