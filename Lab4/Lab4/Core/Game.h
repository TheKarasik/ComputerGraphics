#pragma once
#include "Display32.h"
#include "InputDevice.h"
#include <chrono>


//#include "Cube.h"

#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "dxguid.lib")

#define S_PER_UPDATE 0.016

class Object3D;
class Icosahedron;
class OrbitalMovement;
//class TriangleComponent;
//class GoldenRectangleComponent;
class Cube;
class Renderer;
class CameraObject;

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
    Cube* ParentCube;
    Cube* ChildrenCube;
    Object3D* JustCenter;
    std::vector<Cube*> Cubes;
    std::vector<Icosahedron*> Icosahedrons;
    std::vector<OrbitalMovement*> orbital_movement_;
    CameraObject* camera_;
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
