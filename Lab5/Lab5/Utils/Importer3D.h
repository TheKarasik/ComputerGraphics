#pragma once

#include <SimpleMath.h>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

struct MeshGeometry;
struct TriangleGeometry;
class Renderer;
class Mesh;


class Importer3D
{
public:
    struct MeshesWithTransformation
    {
        struct MeshAndTriangles
        {
            MeshAndTriangles(aiMesh* mesh);
            std::vector<MeshGeometry*> mesh_geom() {return mesh_geom_;}
        private:
            std::vector<MeshGeometry*> mesh_geom_ = std::vector<MeshGeometry*>();
        };
        std::vector<MeshAndTriangles*> meshes_and_triangles;
        aiMatrix4x4 transformation;
    };
    bool DoTheImportThing( const std::string& pFile);
    std::vector<Mesh*> GetMeshes(Renderer* renderer);
    void print_matrix(aiMatrix4x4 m);
private:
    std::vector<MeshesWithTransformation> mwt_vector_;
    const aiScene* scene_;
    void ProceedStoringNodes(aiNode* current_node, aiMatrix4x4 transformation);
    DirectX::XMFLOAT3 convertV3(aiVector3t<float> v);
};
