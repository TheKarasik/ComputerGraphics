#pragma once
#include <unordered_map>

#include "Object3D.h"
class Triangle;
class TriangleGeometry;

class Icosahedron : public Object3D
{
public:
    Icosahedron(Renderer* renderer, Object3D* parent, unsigned int recursion = 2);
private:
    void normalize_and_store_vertex(DirectX::SimpleMath::Vector3 vertex, std::vector<TriangleVertex>* vector);
    TriangleVertex generate_middle_vertex(TriangleVertex tv1, TriangleVertex tv2);
    std::vector<Triangle*> triangles_;
};
