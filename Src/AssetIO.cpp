#include "RoX/AssetIO.h"

#include <fstream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

using BoneNameToAiBone = std::unordered_map<std::string, const aiBone*>;

std::string GetNameFromFilePath(std::string filePath) {
    std::string baseName = filePath.substr(filePath.find_last_of("/") + 1);
    size_t extension(baseName.find_last_of("."));
    return baseName.substr(0, extension);
}

DirectX::XMMATRIX GetTransformFromAiBone(const aiBone* pBone) {
    return {
        pBone->mOffsetMatrix.a1, pBone->mOffsetMatrix.a2, pBone->mOffsetMatrix.a3, pBone->mOffsetMatrix.a4,
        pBone->mOffsetMatrix.b1, pBone->mOffsetMatrix.b2, pBone->mOffsetMatrix.b3, pBone->mOffsetMatrix.b4,
        pBone->mOffsetMatrix.c1, pBone->mOffsetMatrix.c2, pBone->mOffsetMatrix.c3, pBone->mOffsetMatrix.c4,
        pBone->mOffsetMatrix.d1, pBone->mOffsetMatrix.d2, pBone->mOffsetMatrix.d3, pBone->mOffsetMatrix.d4
    };
}

void ParseBoneNameToAiBone(const aiScene* pScene, BoneNameToAiBone& boneNameToAiBone) {
    for (int meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
        const aiMesh* pMesh = pScene->mMeshes[meshIndex];
        for (int boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex) {
            const aiBone* pBone = pMesh->mBones[boneIndex];
            boneNameToAiBone[pBone->mName.C_Str()] = pBone;
        }
    }
}

void ParseBonesRecursive(const aiNode* pNode, BoneNameToAiBone& boneNameToAiBone, Model& model, std::uint32_t parentIndex) {
    std::uint32_t index = parentIndex;

    if (boneNameToAiBone.find(pNode->mName.C_Str()) != boneNameToAiBone.end()) {
        Bone bone(pNode->mName.C_Str(), parentIndex);
        model.GetBones().push_back(bone);

        index = model.GetNumBones() - 1;
        model.GetBoneMatrices()[index] = GetTransformFromAiBone(boneNameToAiBone.at(pNode->mName.C_Str()));
        DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant(model.GetBoneMatrices()[index]);
        model.GetInverseBindPoseMatrices()[index] = DirectX::XMMatrixInverse(&determinant, model.GetBoneMatrices()[index]);

        if (parentIndex >= 0 && parentIndex < model.GetNumBones()) 
            model.GetBones()[parentIndex].GetChildIndices().push_back(index);
    }

    for (int i = 0; i < pNode->mNumChildren; ++i) {
        ParseBonesRecursive(pNode->mChildren[i], boneNameToAiBone, model, index);
    }
}

void ParseBones(const aiScene* pScene, Model& model) {
    BoneNameToAiBone boneNameToAiBone;
    ParseBoneNameToAiBone(pScene, boneNameToAiBone);
    model.MakeBoneMatricesArray(boneNameToAiBone.size());
    model.MakeInverseBoneMatricesArray(boneNameToAiBone.size());
    ParseBonesRecursive(pScene->mRootNode, boneNameToAiBone, model, -1); 
}


void ParseVertices(std::vector<VertexPositionNormalTexture>& vertices, const aiMesh* pMesh) {
    printf("Parsing vertices...\n");

    vertices.reserve(pMesh->mNumVertices);
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                {{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }}, 
                {{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }}, 
                {{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }}
                });
    }
}

void ParseSkinnedVertices(std::vector<VertexPositionNormalTextureSkinning>& vertices, const aiMesh* pMesh) {
    printf("Parsing vertices...\n");

    vertices.reserve(pMesh->mNumVertices);
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                {{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }}, 
                {{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }}, 
                {{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }},
                { 0, 0, 0, 0},
                {{ 1.f, 1.f, 1.f, 1.f }}
                });
    }
}

void ParseIndices(std::vector<std::uint16_t>& indices, const aiMesh* pMesh) {
    printf("Parsing indices...\n");

    indices.reserve(pMesh->mNumFaces * 3);
    for (int faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex) {
        for (int faceIndicesIndex = 0; faceIndicesIndex < pMesh->mFaces[faceIndex].mNumIndices; ++faceIndicesIndex) {
            indices.push_back(pMesh->mFaces[faceIndex].mIndices[faceIndicesIndex]);
        }
    }
}

std::shared_ptr<Model> AssetIO::ImportModel(std::string filepath, std::shared_ptr<Material> material, bool skinned) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filepath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filepath + "': " + importer.GetErrorString());

    Model model(GetNameFromFilePath(filepath), material);
    ParseBones(pScene, model);

    std::shared_ptr<IMesh>pIMesh;
    if (skinned)
        pIMesh = std::make_shared<SkinnedMesh>(GetNameFromFilePath(filepath) + "_mesh");
    else
        pIMesh = std::make_shared<Mesh>(GetNameFromFilePath(filepath) + "_mesh");

    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* pAIMesh = pScene->mMeshes[i];

        auto pSubmesh = std::make_unique<Submesh>(pAIMesh->mName.C_Str(), 0);
        pSubmesh->SetStartIndex(pIMesh->GetNumIndices());
        pSubmesh->SetVertexOffset(pIMesh->GetNumVertices());
        
        if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(pIMesh.get()))
            ParseSkinnedVertices(pSkinnedMesh->GetVertices(), pAIMesh);
        if (auto pMesh = dynamic_cast<Mesh*>(pIMesh.get()))
            ParseVertices(pMesh->GetVertices(), pAIMesh);

        ParseIndices(pIMesh->GetIndices(), pAIMesh);
        pSubmesh->SetIndexCount(pIMesh->GetNumIndices() - pSubmesh->GetStartIndex());

        pIMesh->Add(std::move(pSubmesh));
    }

    model.Add(std::move(pIMesh));

    return std::make_shared<Model>(model);
}


/*

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

std::shared_ptr<Model> AssetIO::ImportModel(std::string filePath, std::shared_ptr<Material> pMaterial) {
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
    ParseBones(pScene, *pModel.get());

    return pModel;
}

void AssetIO::ExportModel(Model* pMesh, std::string filePath) {
    // TODO
    throw std::runtime_error("ExportMesh() not yet implemented");
}
*/
