#pragma once
#include "Object3D.h"
#include <WICTextureLoader.h>

class FileTexture;
class Triangle;
class MeshGeometry;

class Mesh : public Object3D
{
public:
    Mesh(Renderer* renderer, const DirectX::SimpleMath::Matrix& transformation = DirectX::SimpleMath::Matrix::Identity);
    void set_meshes(MeshGeometry* mesh_geometries);
    void set_texture(FileTexture* texture);
    void set_drawability(bool is_drawable) {IsDrawable = is_drawable;}
    void flip_normals();
    FileTexture* texture() {return texture_;}
    //void set_parent(Object3D* parent) {Object3D::parent_ = parent;}
    bool PlayerControlled = false;
    DirectX::BoundingSphere* sphere;
    std::vector<Mesh*> pick_ups;
    void draw() override;
    void update() override;
    void set_bounding_sphere();
private:
    std::vector<Triangle*> triangles;
    FileTexture* texture_;
    AbstractBuffer* vertex_buffer_;
    AbstractBuffer* index_buffer_;
    unsigned int strides = 0;
    unsigned int offset = 0;
    TriangleVertex* vertecies;
    const int* indecies;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertexBuffer;
    unsigned indiciesNum = 0;
    bool IsDrawable = true;
};
