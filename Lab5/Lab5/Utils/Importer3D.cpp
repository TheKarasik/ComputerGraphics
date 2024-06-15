#include "Importer3D.h"

#include <iostream>

#include "Mesh.h"
#include "Triangle.h"

Importer3D::MeshesWithTransformation::MeshAndTriangles::MeshAndTriangles(aiMesh* mesh)
{
    MeshGeometry* geom = new MeshGeometry();
        //TriangleGeometry* geom = new TriangleGeometry();
        for (int j = 0; j < mesh->mNumVertices; j++)
        {
            geom->vertecies.push_back(TriangleVertex());
            //TriangleVertex
            const DirectX::XMFLOAT3 position3 = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mVertices[j]);
            geom->vertecies[j].position = DirectX::XMFLOAT4(position3.x, position3.y, position3.z, 1);
            geom->vertecies[j].tex = *reinterpret_cast<DirectX::XMFLOAT2*>(&mesh->mTextureCoords[0][j]);
            geom->vertecies[j].normal = *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh->mNormals[j]);
        }
    for (uint32_t j = 0; j < mesh->mNumFaces; j++)
    {
        aiFace face = mesh->mFaces[j];
        for (uint32_t k = 0; k < face.mNumIndices; k++)
        {
            geom->indicies.push_back(face.mIndices[k]);
        }
    }
    
    mesh_geom_.push_back(geom);
        //triangles_.push_back(geom);
    
}

bool Importer3D::DoTheImportThing( const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    scene_ = importer.ReadFile( pFile,
        aiProcess_MakeLeftHanded            |
        aiProcess_FlipUVs                   |
        aiProcess_CalcTangentSpace          |
        aiProcess_Triangulate               |
        aiProcess_JoinIdenticalVertices     |
        aiProcess_SortByPType);

    // If the import failed, report it
    if (nullptr == scene_) {
        //DoTheErrorLogging( importer.GetErrorString());
        return false;
    }
    aiMatrix4x4 transformation = aiMatrix4x4();
    
    /*std::cout << "Root node children nodes: " <<std::endl;
    std::cout << scene_->mRootNode->mNumChildren  << std::endl;
    std::cout << "Root node transformation: " <<std::endl;
    print_matrix(scene_->mRootNode->mTransformation);
    std::cout << "First children node meshes: " <<std::endl;
    std::cout << scene_->mRootNode->mChildren[0]->mNumMeshes  << std::endl;
    std::cout << "First children node transformation: " <<std::endl;
    print_matrix(scene_->mRootNode->mChildren[0]->mTransformation);
    std::cout << "First children node children nodes: " <<std::endl;
    std::cout << scene_->mRootNode->mChildren[0]->mNumChildren  << std::endl;*/
    // Now we can access the file's contents.
    
    ProceedStoringNodes(scene_->mRootNode, transformation);
    //PrepareEverything();

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

void Importer3D::ProceedStoringNodes(aiNode* current_node, aiMatrix4x4 transformation)
{
    MeshesWithTransformation mwt;
    aiMatrix4x4 aiTransformation = current_node->mTransformation * transformation;
    aiTransformation = aiTransformation.Transpose();
    mwt.transformation = aiTransformation;
    for (unsigned i = 0; i < current_node->mNumMeshes; i++)
        mwt.meshes_and_triangles.push_back(new MeshesWithTransformation::MeshAndTriangles(scene_->mMeshes[current_node->mMeshes[i]]));
    mwt_vector_.push_back(mwt);
    
    for (unsigned i = 0; i < current_node->mNumChildren; i++)
        ProceedStoringNodes(current_node->mChildren[i], aiTransformation);
}

DirectX::XMFLOAT3 Importer3D::convertV3(aiVector3t<float> v)
{
    return DirectX::XMFLOAT3(v.x, v.y, v.z);
}

std::vector<Mesh*> Importer3D::GetMeshes(Renderer* renderer)
{
    unsigned i = 0;
    std::vector<Mesh*> meshes;
    for (const auto mwt : mwt_vector_)
    {
        for (const auto mat : mwt.meshes_and_triangles)
        {
            if (i == 1) continue;
            aiVector3t<float> scale;
            aiVector3t<float> axis;
            float angle;
            aiVector3t<float> position;
            mwt.transformation.Decompose(scale, axis, angle, position);
            
            Mesh* mesh = new Mesh(renderer);
            //mesh->set_location(convertV3(position));
            mesh->set_rotation(convertV3(-axis), angle);
            //mesh->set_scale(convertV3(scale));
            for (int i = 0; i < mat->mesh_geom().size(); i++)
            {
                mesh->set_meshes(mat->mesh_geom()[i]);
            }
            meshes.push_back(mesh);
            i++;
        }
    }
    return meshes;
    
}

void Importer3D::print_matrix(aiMatrix4x4 m)
{
    aiVector3t<float> scale;
    aiVector3t<float> axis;
    float angle;
    aiVector3t<float> position;
    m.Decompose(scale, axis, angle, position);
    std::cout << "Scale vector: " << scale.x << " " << scale.y << " " << scale.z <<std::endl;
    std::cout << "Axis vector: " << axis.x << " " << axis.y << " " << axis.z <<std::endl;
    std::cout << "Angle: " << angle <<std::endl;
    std::cout << "Position vector: " << position.x << " " << position.y << " " << position.z <<std::endl;
    
}

