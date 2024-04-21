#include "RoX/AssetIO.h"

#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

void ParseVertices(std::vector<DirectX::VertexPositionNormalTexture>& vertices, const aiMesh* pMesh);
void ParseIndices(std::vector<std::uint16_t>& indices, const aiMesh* pMesh);

void AssetIO::ImportMeshFromVBO(Submesh& mesh, std::string filePath) {
    std::string extension = filePath.substr(filePath.size() - 4);
    if (extension != ".vbo")
        throw std::invalid_argument("Must be a .vbo file");

    std::ifstream file(filePath, std::ifstream::binary); 
    if (!file.is_open())
        throw std::invalid_argument("Failed to open .vbo file");

    std::uint32_t numVertices = 0;
    std::uint32_t numIndices = 0;
    
    file.read(reinterpret_cast<char*>(&numVertices), sizeof(std::uint32_t));
    if (!numVertices)
        throw std::runtime_error("Failed reading numVertices from .vbo file");

    file.read(reinterpret_cast<char*>(&numIndices), sizeof(std::uint32_t));
    if (!numIndices)
        throw std::runtime_error("Failed reading numIndices from .vbo file");

    mesh.GetVertices().get()->resize(numVertices);
    file.read(reinterpret_cast<char*>(mesh.GetVertices().get()->data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices);

    mesh.GetIndices().get()->resize(numIndices);
    file.read(reinterpret_cast<char*>(mesh.GetIndices().get()->data()), sizeof(std::uint16_t) * numIndices);
    mesh.SetIndexCount(mesh.GetIndices()->size());

    file.close();
}

void AssetIO::ExportMeshToVBO(Submesh& mesh, std::string filePath) {
    std::string extension = filePath.substr(filePath.size() - 4);
    if (extension != ".vbo")
        filePath += ".vbo";

    std::uint32_t numVertices = mesh.GetVertices().get()->size();
    std::uint32_t numIndices = mesh.GetIndices().get()->size();
   
    std::ofstream file(filePath, std::ofstream::binary);
    file.write(reinterpret_cast<char*>(&numVertices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(&numIndices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(mesh.GetVertices().get()->data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices)
        .write(reinterpret_cast<char*>(mesh.GetIndices().get()->data()), sizeof(std::uint16_t) * numIndices);

    file.close();
}

void AssetIO::ImportMesh(Submesh& mesh, std::string filePath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());

    ParseVertices(*mesh.GetVertices().get(), pScene->mMeshes[0]);
    ParseIndices(*mesh.GetIndices().get(), pScene->mMeshes[0]);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void AssetIO::ExportMesh(Submesh& mesh, std::string filePath) {
    // TODO
    throw std::runtime_error("ExportMesh() is unsupported");
}

void ParseVertices(std::vector<DirectX::VertexPositionNormalTexture>& vertices, const aiMesh* pMesh) {
    printf("Parsing vertices...\n");

    vertices.resize(pMesh->mNumVertices);
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices[i].position.x = pMesh->mVertices[i].x;
        vertices[i].position.y = pMesh->mVertices[i].y;
        vertices[i].position.z = pMesh->mVertices[i].z;

        vertices[i].normal.x = pMesh->mNormals[i].x;
        vertices[i].normal.y = pMesh->mNormals[i].y;
        vertices[i].normal.z = pMesh->mNormals[i].z;

        vertices[i].textureCoordinate.x = pMesh->mTextureCoords[0][i].x;
        vertices[i].textureCoordinate.y = pMesh->mTextureCoords[0][i].y;
    }
}

void ParseIndices(std::vector<std::uint16_t>& indices, const aiMesh* pMesh) {
    printf("Parsing indices...\n");

    indices.resize(pMesh->mNumFaces * 3);

    int indicesIndex = 0;
    for (int faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex) {
        for (int faceIndicesIndex = 0; faceIndicesIndex < pMesh->mFaces[faceIndex].mNumIndices; ++faceIndicesIndex) {
            indices[indicesIndex++] = pMesh->mFaces[faceIndex].mIndices[faceIndicesIndex];
        }
    }
}
