#include "RoX/AssetIO.h"

#include <fstream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

std::string GetNameFromFilePath(std::string filePath) {
    std::string baseName = filePath.substr(filePath.find_last_of("/") + 1);
    size_t extension(baseName.find_last_of("."));
    return baseName.substr(0, extension);
}

void ParseBones(std::vector<Bone>& bones, std::unordered_map<std::string, std::uint8_t>& ids, const aiNode* pNode) {
    for (int i = 0; i < pNode->mNumChildren; ++i) {
        std::uint8_t& id = ids[pNode->mName.C_Str()];
        if (!id)
            id = ids.size() - 1;
        Bone bone(pNode->mName.C_Str());
        bones.push_back(bone);
        ParseBones(bones, ids, pNode->mChildren[i]);
    } 
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

void ParseSubmesh(Submesh& submesh, const aiMesh* pMesh) {
    ParseVertices(*submesh.GetVertices(), pMesh);
    ParseIndices(*submesh.GetIndices(), pMesh);
    submesh.SetIndexCount(submesh.GetIndices()->size());
}

std::unique_ptr<Submesh> AssetIO::ImportSubmeshFromVBO(std::string filePath) {
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

    auto pSubmesh = std::make_unique<Submesh>(GetNameFromFilePath(filePath), 0);

    pSubmesh->GetVertices().get()->resize(numVertices);
    file.read(reinterpret_cast<char*>(pSubmesh->GetVertices()->data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices);

    pSubmesh->GetIndices()->resize(numIndices);
    file.read(reinterpret_cast<char*>(pSubmesh->GetIndices()->data()), sizeof(std::uint16_t) * numIndices);
    pSubmesh->SetIndexCount(pSubmesh->GetIndices()->size());

    file.close();

    return pSubmesh;
}

void AssetIO::ExportSubmeshToVBO(Submesh* pSubmesh, std::string filePath) {
    std::string extension = filePath.substr(filePath.size() - 4);
    if (extension != ".vbo")
        filePath += ".vbo";

    std::uint32_t numVertices = pSubmesh->GetVertices()->size();
    std::uint32_t numIndices = pSubmesh->GetIndices()->size();
   
    std::ofstream file(filePath, std::ofstream::binary);
    file.write(reinterpret_cast<char*>(&numVertices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(&numIndices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(pSubmesh->GetVertices()->data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices)
        .write(reinterpret_cast<char*>(pSubmesh->GetIndices()->data()), sizeof(std::uint16_t) * numIndices);

    file.close();
}

std::unique_ptr<Submesh> AssetIO::ImportSubmesh(std::string filePath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());

    auto pSubmesh = std::make_unique<Submesh>(GetNameFromFilePath(filePath), 0);
    ParseSubmesh(*pSubmesh, pScene->mMeshes[0]);
    return pSubmesh;
}

void AssetIO::ExportSubmesh(Submesh* pSubmesh, std::string filePath) {
    // TODO
    throw std::runtime_error("ExportSubmesh() not yet implemented");
}

std::shared_ptr<Mesh> AssetIO::ImportMesh(std::string filePath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());
    
    auto pMesh = std::make_shared<Mesh>(GetNameFromFilePath(filePath));
    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        auto submesh = std::make_unique<Submesh>(pScene->mMeshes[i]->mName.C_Str(), 0);
        ParseSubmesh(*submesh, pScene->mMeshes[i]);
        pMesh->Add(std::move(submesh));
    }
    return pMesh;
}

void AssetIO::ExportMesh(Mesh* pMeshPart, std::string filePath) {
    // TODO
    throw std::runtime_error("ExportMesh() not yet implemented");
}

std::shared_ptr<Model> AssetIO::ImportMesh(std::string filePath, std::shared_ptr<Material> pMaterial) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());

    auto pModel = std::make_shared<Model>(GetNameFromFilePath(filePath), pMaterial);
    auto pMesh = std::make_shared<Mesh>(GetNameFromFilePath(filePath));
    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        auto submesh = std::make_unique<Submesh>(pScene->mMeshes[i]->mName.C_Str(), 0);
        ParseSubmesh(*submesh, pScene->mMeshes[i]);
        pMesh->Add(std::move(submesh));
    }
    pModel->Add(pMesh);

    return pModel;
}

void AssetIO::ExportMesh(Model* pMesh, std::string filePath) {
    // TODO
    throw std::runtime_error("ExportMesh() not yet implemented");
}
