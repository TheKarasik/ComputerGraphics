#include "Icosahedron.h"

#include <complex>

#include "Triangle.h"

Icosahedron::Icosahedron(Renderer* renderer, Object3D* parent, unsigned int recursion) : Object3D(renderer, parent)
{
    std::vector<TriangleVertex> vertices;
    std::vector<TriangleGeometry> faces;
    
    float t = (1 + std::sqrt(5))/2;

    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(-1,t,0), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(1,t,0), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(-1,-t,0), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(1,-t,0), &vertices);
    
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(0,-1,t), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(0,1,t), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(0,-1,-t), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(0,1,-t), &vertices);
    
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(t,0,-1), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(t,0,1), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(-t,0,-1), &vertices);
    normalize_and_store_vertex(DirectX::SimpleMath::Vector3(-t,0,1), &vertices);

    faces.push_back(TriangleGeometry({vertices[0], vertices[11], vertices[5]}));
    faces.push_back(TriangleGeometry({vertices[0], vertices[5], vertices[1]}));
    faces.push_back(TriangleGeometry({vertices[0], vertices[1], vertices[7]}));
    faces.push_back(TriangleGeometry({vertices[0], vertices[7], vertices[10]}));
    faces.push_back(TriangleGeometry({vertices[0], vertices[10], vertices[11]}));
    
    faces.push_back(TriangleGeometry({vertices[1], vertices[5], vertices[9]}));
    faces.push_back(TriangleGeometry({vertices[5], vertices[11], vertices[4]}));
    faces.push_back(TriangleGeometry({vertices[11], vertices[10], vertices[2]}));
    faces.push_back(TriangleGeometry({vertices[10], vertices[7], vertices[6]}));
    faces.push_back(TriangleGeometry({vertices[7], vertices[1], vertices[8]}));
    
    faces.push_back(TriangleGeometry({vertices[3], vertices[9], vertices[4]}));
    faces.push_back(TriangleGeometry({vertices[3], vertices[4], vertices[2]}));
    faces.push_back(TriangleGeometry({vertices[3], vertices[2], vertices[6]}));
    faces.push_back(TriangleGeometry({vertices[3], vertices[6], vertices[8]}));
    faces.push_back(TriangleGeometry({vertices[3], vertices[8], vertices[9]}));
    
    faces.push_back(TriangleGeometry({vertices[4], vertices[9], vertices[5]}));
    faces.push_back(TriangleGeometry({vertices[2], vertices[4], vertices[11]}));
    faces.push_back(TriangleGeometry({vertices[6], vertices[2], vertices[10]}));
    faces.push_back(TriangleGeometry({vertices[8], vertices[6], vertices[7]}));
    faces.push_back(TriangleGeometry({vertices[9], vertices[8], vertices[1]}));

    for (int i = 0; i < recursion; i++)
    {
        std::vector<TriangleGeometry> new_faces;
        for(auto f : faces)
        {
            TriangleVertex mv1 = generate_middle_vertex(f.vertecies[0], f.vertecies[1]);
            TriangleVertex mv2 = generate_middle_vertex(f.vertecies[1], f.vertecies[2]);
            TriangleVertex mv3 = generate_middle_vertex(f.vertecies[2], f.vertecies[0]);

            new_faces.push_back({f.vertecies[0], mv1, mv3});
            new_faces.push_back({mv1, f.vertecies[1], mv2});
            new_faces.push_back({mv2, mv3, f.vertecies[2]});
            new_faces.push_back({mv3, mv2, mv1});
        }
        faces = std::move(new_faces);
    }

    for (const auto f : faces)
    {
        Triangle* t = new Triangle(renderer_, f);
        triangles_.push_back(t);
        t->change_color(color_);
        t->Initialize();
    }
}

void Icosahedron::normalize_and_store_vertex(DirectX::SimpleMath::Vector3 vertex, std::vector<TriangleVertex>* vector)
{
    vertex.Normalize();
    vector->push_back(TriangleVertex(DirectX::XMFLOAT4(vertex.x, vertex.y, vertex.z, 1)));
    //return {{DirectX::XMFLOAT4(vertex.x, vertex.y, vertex.z, 1)}};
}

TriangleVertex Icosahedron::generate_middle_vertex(TriangleVertex tv1, TriangleVertex tv2)
{
    DirectX::SimpleMath::Vector3 v1(tv1.position.x, tv1.position.y, tv1.position.z);
    DirectX::SimpleMath::Vector3 v2(tv2.position.x, tv2.position.y, tv2.position.z);
    DirectX::SimpleMath::Vector3 v3 = (v1+v2)/2;
    v3.Normalize();
    return {DirectX::XMFLOAT4(v3.x, v3.y, v3.z, 1)};
}
