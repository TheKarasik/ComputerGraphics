#pragma once
#include <DirectXMath.h>
#include "vector"
#define Pi 3.1415926535f
#define CASCADE_COUNT 4


struct CascadeData
{
    DirectX::SimpleMath::Matrix viewProjMats[CASCADE_COUNT];
    /*DirectX::SimpleMath::Matrix viewMats[CASCADE_COUNT];
    DirectX::SimpleMath::Matrix projMats[CASCADE_COUNT];*/
    float distances[CASCADE_COUNT];
};

struct TriangleVertex     //New version
{
    TriangleVertex() = default;
    TriangleVertex(DirectX::XMFLOAT4 pos) : position(pos) {}
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT2 tex;
    DirectX::XMFLOAT3 normal;
};

struct StructuredBufferSize
{
    alignas(16) UINT size;
};

struct ConstantBufferTransformMatricies
{
    DirectX::SimpleMath::Matrix world;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
    DirectX::SimpleMath::Vector4 camera_position;
    //alignas (16) bool RenderState;
};

struct MaterialStructCB
{
    //DirectX::SimpleMath::Vector4 ambient_color = DirectX::SimpleMath::Vector4(0.5f);
    DirectX::SimpleMath::Vector4 global_ambient = DirectX::SimpleMath::Vector4(0.2f);
    DirectX::SimpleMath::Vector4 specular_color = DirectX::SimpleMath::Vector4::One;
    float specular_power = 50.0f;
    float specular_scale = 0.25f;
    DirectX::SimpleMath::Vector2 padding;
};

struct ScrenToViewStructCB
{
    DirectX::SimpleMath::Matrix inverse_projection;
    alignas(16) DirectX::SimpleMath::Vector2 screen_dimensions;
};

enum LightType : unsigned int
{
    SpotLight = 0,
    PointLight = 1,
    DirectionalLight = 2
};

struct DirectionalLightDataStruct
{
    DirectX::SimpleMath::Matrix mView;
    DirectX::SimpleMath::Matrix mProj;
    DirectX::SimpleMath::Vector4 PositionWS;
    DirectX::SimpleMath::Vector4 DirectionWS;
    DirectX::SimpleMath::Vector4 DirectionVS;
    DirectX::SimpleMath::Vector4 Color;
    alignas(16) float Intensity;
};

struct LightDataStruct 
{
    DirectX::SimpleMath::Vector4 PositionWS;
    DirectX::SimpleMath::Vector4 DirectionWS;
    DirectX::SimpleMath::Vector4 PositionVS;
    DirectX::SimpleMath::Vector4 DirectionVS;
    DirectX::SimpleMath::Vector4 Color;
    float SpotlightAngle;
    float Range;
    float Intensity;
    LightType Type;
};

/*struct TriangleVertex       //Old version
{
    TriangleVertex(DirectX::XMFLOAT4 pos) : position(pos), color(DirectX::XMFLOAT4(1,1,1,1)) {}
    TriangleVertex(DirectX::XMFLOAT4 pos, DirectX::XMFLOAT4 col) : position(pos), color(col) {}
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT4 color;
};*/

struct TriangleGeometry
{
    TriangleVertex vertecies[3];
    //TriangleGeometry() = default;
};

struct MeshGeometry
{
    std::vector<TriangleVertex> vertecies;
    std::vector<int> indicies;
    //TriangleGeometry() = default;
};

struct CoordinateTransformationCBStruct
{
    DirectX::SimpleMath::Matrix world_matrix;
    DirectX::SimpleMath::Matrix view_matrix;
    DirectX::SimpleMath::Matrix projection_matrix;
};

struct MouseMoveEventArgs
{
    DirectX::SimpleMath::Vector2 Position;
    DirectX::SimpleMath::Vector2 Offset;
    int WheelDelta;
};

enum RenderStateEnum
{
    NoRenderState = 0,
    MainRenderState = 1,
    ShadowmapRenderState = 2,
    CascadeShadowmapRenderState = 3,
    GBufferRenderState = 4,
    LightingStageState = 5,
    //MinimapRenderState = 2
};

/*struct RectangleGeometry
{
    DirectX::XMFLOAT2 pos;
    DirectX::XMFLOAT2 size;
    bool DetectCollision(RectangleGeometry rect)
    {
        if (fabsf(pos.x - rect.pos.x) <= size.x + rect.size.y &&
            fabsf(pos.y - rect.pos.y) <= size.y + rect.size.y) return true;
        return false;
    }
    bool DetectVerticalBorderHit()
    {
        if (fabsf(pos.y) + size.y >= 1) return true;
        return false;
    }
    bool DetectHorizontalBorderHit()
    {
        if (fabsf(pos.x) + size.x >= 1) return true;
        return false;
    }
};*/