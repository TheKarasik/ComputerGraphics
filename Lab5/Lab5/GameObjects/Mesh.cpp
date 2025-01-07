#include "Mesh.h"

#include <random>
#include <set>

#include "..\Render\FileTexture.h"
#include "Triangle.h"

Mesh::Mesh(Renderer* renderer, const DirectX::SimpleMath::Matrix& transformation)
    : Object3D(renderer, transformation)
{
    //update();
}

void Mesh::set_meshes(MeshGeometry* mesh_geometries)
{
    vertecies = mesh_geometries->vertecies.data();
    indecies = mesh_geometries->indicies.data();
    indiciesNum = mesh_geometries->indicies.size();
    /*vertex_buffer_ = new VertexBuffer(renderer_, &vertecies, sizeof TriangleVertex * mesh_geometries->vertecies.size());
    index_buffer_ = new IndexBuffer(renderer_, &indecies, mesh_geometries->indicies.size() * sizeof(int));*/

    D3D11_BUFFER_DESC indexBufDesc;
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * mesh_geometries->indicies.size();

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &mesh_geometries->indicies[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    renderer_->device()->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

    D3D11_BUFFER_DESC vertexBufDesc;
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(TriangleVertex) * mesh_geometries->vertecies.size();

    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &mesh_geometries->vertecies[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    renderer_->device()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
    
    strides = sizeof TriangleVertex;

}

void Mesh::set_texture(FileTexture* texture)
{
    texture_ = texture;
}

void Mesh::set_normal_texture(FileTexture* texture)
{
    normal_texture_ = texture;
}

void Mesh::flip_normals()
{
    for (int i = 0; i < indiciesNum; i++)
    {
        vertecies[i].normal = DirectX::XMFLOAT3(-vertecies[i].normal.x, -vertecies[i].normal.y, -vertecies[i].normal.z);
    }
}

void Mesh::draw()
{
    Object3D::draw();
    
    if (!IsDrawable) return;

    renderer_->ProvideMeshData(this);

    renderer_->Context()->IASetIndexBuffer(indexBuffer,  DXGI_FORMAT_R32_UINT, 0);
    renderer_->Context()->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offset);
    renderer_->Context()->DrawIndexed(indiciesNum, 0, 0);
}

void Mesh::update()
{
    Object3D::update();
    if (!PlayerControlled) return;
    //std::cout << "location (player): " << sphere->Center.x << " " << sphere->Center.y << " " << sphere->Center.z << std::endl;
    for (const auto p : pick_ups)
    {
        if (!p->sphere) continue;
        //std::cout << "location (pickups): " << p->sphere->Center.x << " " << p->sphere->Center.y << " " << p->sphere->Center.z << std::endl;
        if (this->sphere->Intersects(*p->sphere))
        {
            auto prevWorld = p->global_world_matrix();
            p->set_parent(this);
            auto newWorld = p->compostion_transformation_matrix();
            p->set_local_transformation(prevWorld*newWorld.Invert());
            p->sphere = nullptr;
        }
    }
    sphere->Center = this->location();
    
    //texture_->texture_constant_buffer_->UpdateBuffer(&texture_->MatProp);
}

void Mesh::set_bounding_sphere()
{
    unsigned radius = 2;
    /*for (const auto t : triangles)
    {
        for (int i = 0; i < 3; i++)
        {
            if (t->triangle().vertecies[i].position.x*this->scale().x > radius) radius = t->triangle().vertecies[i].position.x*this->scale().x;
            if (t->triangle().vertecies[i].position.y*this->scale().y > radius) radius = t->triangle().vertecies[i].position.y*this->scale().y;
            if (t->triangle().vertecies[i].position.z*this->scale().z > radius) radius = t->triangle().vertecies[i].position.z*this->scale().z;
            /*if (t->triangle().vertecies[i].position.x > radius) radius = t->triangle().vertecies[i].position.x;
            if (t->triangle().vertecies[i].position.y > radius) radius = t->triangle().vertecies[i].position.y;
            if (t->triangle().vertecies[i].position.z > radius) radius = t->triangle().vertecies[i].position.z;#1#
        }
        //triangles.back()->Initialize();
    }*/
    sphere = new DirectX::BoundingSphere(location(), radius);
}

