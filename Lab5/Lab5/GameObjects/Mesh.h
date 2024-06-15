﻿#pragma once
#include "Object3D.h"
#include <WICTextureLoader.h>

class Texture;
class Triangle;
class MeshGeometry;

class Mesh : public Object3D
{
public:
    Mesh(Renderer* renderer, const DirectX::SimpleMath::Matrix& transformation = DirectX::SimpleMath::Matrix::Identity);
    void set_meshes(const MeshGeometry* mesh_geometries);
    void set_texture(Texture* texture);
    //void set_parent(Object3D* parent) {Object3D::parent_ = parent;}
    bool PlayerControlled = false;
    DirectX::BoundingSphere* sphere;
    std::vector<Mesh*> pick_ups;
    void draw() override;
    void update() override;
    void set_bounding_sphere();
private:
    std::vector<Triangle*> triangles;
    Texture* texture_;
    AbstractBuffer* vertex_buffer_;
    AbstractBuffer* index_buffer_;
    unsigned int strides = 0;
    unsigned int offset = 0;
    const TriangleVertex* vertecies;
    const int* indecies;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertexBuffer;
    unsigned indiciesNum = 0;
};
