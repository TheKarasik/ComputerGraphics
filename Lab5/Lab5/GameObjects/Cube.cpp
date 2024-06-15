#include "Cube.h"

//DirectX::XMFLOAT4(1, -1, -1, 1) - A
//DirectX::XMFLOAT4(-1, -1, -1, 1) - B
//DirectX::XMFLOAT4(-1, 1, -1, 1) - C
//DirectX::XMFLOAT4(1, 1, -1, 1) - D
//DirectX::XMFLOAT4(1, -1, 1, 1) - E
//DirectX::XMFLOAT4(-1, -1, 1, 1) - F
//DirectX::XMFLOAT4(-1, 1, 1, 1) - G
//DirectX::XMFLOAT4(1, 1, 1, 1) - H


Cube::Cube(Renderer* renderer, Object3D* parent): Object3D(renderer)
{
    set_parent(parent);
    TriangleVertex A = DirectX::XMFLOAT4(1, -1, -1, 1);
    TriangleVertex B = DirectX::XMFLOAT4(-1, -1, -1, 1);
    TriangleVertex C = DirectX::XMFLOAT4(-1, 1, -1, 1);
    TriangleVertex D = DirectX::XMFLOAT4(1, 1, -1, 1);

    TriangleVertex E = DirectX::XMFLOAT4(1, -1, 1, 1);
    TriangleVertex F = DirectX::XMFLOAT4(-1, -1, 1, 1);
    TriangleVertex G = DirectX::XMFLOAT4(-1, 1, 1, 1);
    TriangleVertex H = DirectX::XMFLOAT4(1, 1, 1, 1);

    std::vector<TriangleGeometry> triangle_geometries;

    triangle_geometries.push_back({A, B, C});
    triangle_geometries.push_back({A, D, C});
    triangle_geometries.push_back({A, B, F});
    triangle_geometries.push_back({A, D, H});
    
    triangle_geometries.push_back({F, B, C});
    triangle_geometries.push_back({H, D, C});
    triangle_geometries.push_back({A, E, F});
    triangle_geometries.push_back({A, E, H});
    
    triangle_geometries.push_back({F, G, C});
    triangle_geometries.push_back({H, G, C});
    triangle_geometries.push_back({H, E, F});
    triangle_geometries.push_back({F, G, H});

    for (int i = 0; i < 12; i++)
    {
        triangles_[i] = new Triangle(renderer_, triangle_geometries[i]);
        //triangles_[i]->change_color(color_);
        triangles_[i]->Initialize();
    }
    /*for(const auto triangle : triangles_)
    {
        triangle->change_color(color_);
    }*/
}

