#include "RoX/AssetIO.h"

#include <fstream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

using BoneNameToAiBone = std::unordered_map<std::string, const aiBone*>;

struct VertexBoneData {
    std::uint8_t CurrentIndex = 0;
    std::vector<std::uint32_t> Indices = { 0, 0, 0, 0 };
    std::vector<float> Weights = { 0.f, 0.f, 0.f, 0.f }; 

    void AddBone(std::uint32_t index, float weight) {
        if (CurrentIndex > 4)
            return;

        Indices[CurrentIndex] = index;
        Weights[CurrentIndex++] = weight;
    }

    void Calibrate() {
        float total = Weights[0] + Weights[1] + Weights[2] + Weights[3];

        int i = 0;
        while (total < 1.f) {
            Weights[i % CurrentIndex] += 0.00001;
            total = Weights[0] + Weights[1] + Weights[2] + Weights[3];
        }
    }
};

std::string GetNameFromFilePath(std::string filePath) {
    std::string baseName = filePath.substr(filePath.find_last_of("/") + 1);
    size_t extension(baseName.find_last_of("."));
    return baseName.substr(0, extension);
}

DirectX::XMMATRIX ParseMatrix(const aiMatrix4x4* pMatrix) {
    return {
        pMatrix->a1, pMatrix->a2, pMatrix->a3, pMatrix->a4,
        pMatrix->b1, pMatrix->b2, pMatrix->b3, pMatrix->b4,
        pMatrix->c1, pMatrix->c2, pMatrix->c3, pMatrix->c4,
        pMatrix->d1, pMatrix->d2, pMatrix->d3, pMatrix->d4
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

void ParseBonesRecursive(const aiNode* pNode, BoneNameToAiBone& boneNameToAiBone, Model& model, std::uint32_t parentIndex, const DirectX::XMMATRIX& parentTransform) {
    std::uint32_t index = parentIndex;

    DirectX::XMMATRIX globalTransform = parentTransform * ParseMatrix(&pNode->mTransformation);

    if (boneNameToAiBone.find(pNode->mName.C_Str()) != boneNameToAiBone.end()) {
        Bone bone(pNode->mName.C_Str(), parentIndex);
        model.GetBones().push_back(bone);

        index = model.GetNumBones() - 1;

        DirectX::XMMATRIX inverseBindPose = ParseMatrix(&boneNameToAiBone.at(pNode->mName.C_Str())->mOffsetMatrix);
        model.GetInverseBindPoseMatrices()[index] = inverseBindPose;
        model.GetBoneMatrices()[index] = globalTransform * inverseBindPose;

        if (parentIndex >= 0 && parentIndex < model.GetNumBones()) 
            model.GetBones()[parentIndex].GetChildIndices().push_back(index);
    }

    for (int i = 0; i < pNode->mNumChildren; ++i) {
        ParseBonesRecursive(pNode->mChildren[i], boneNameToAiBone, model, index, globalTransform);
    }
}

void ParseBones(const aiScene* pScene, Model& model) {
    BoneNameToAiBone boneNameToAiBone;
    ParseBoneNameToAiBone(pScene, boneNameToAiBone);
    model.MakeBoneMatricesArray(boneNameToAiBone.size());
    model.MakeInverseBoneMatricesArray(boneNameToAiBone.size());
    ParseBonesRecursive(pScene->mRootNode, boneNameToAiBone, model, -1, DirectX::XMMatrixIdentity()); 
}

void ParseVertexBoneData(const aiScene* pScene, std::vector<VertexBoneData>& vertexBoneData, std::vector<std::uint32_t> startVertices, std::unordered_map<std::string, std::uint32_t> boneNameToIndex) {
    std::uint32_t totalVertices = 0;

    startVertices.resize(pScene->mNumMeshes);
    for (std::uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
        const aiMesh* pMesh = pScene->mMeshes[meshIndex];

        startVertices[meshIndex] = totalVertices;
        totalVertices += pMesh->mNumVertices;
        vertexBoneData.resize(totalVertices);

        for (std::uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex) {
            const aiBone* pBone = pMesh->mBones[boneIndex];
            std::uint32_t& boneId = boneNameToIndex[pBone->mName.C_Str()];
            if (!boneId)
                boneId = boneNameToIndex.size() - 1;

            for (std::uint32_t weightIndex = 0; weightIndex < pBone->mNumWeights; ++weightIndex) {
                const aiVertexWeight weight = pBone->mWeights[weightIndex];
                vertexBoneData[startVertices[meshIndex] + weight.mVertexId].AddBone(boneId, weight.mWeight);
            }
        }
    }
}

void ParseVertices(const aiMesh* pMesh, std::vector<VertexPositionNormalTexture>& vertices) {
    printf("Parsing vertices...\n");

    vertices.reserve(pMesh->mNumVertices + vertices.capacity());
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                {{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }}, 
                {{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }}, 
                {{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }}
                });
    }
}

void ParseSkinnedVertices(const aiMesh* pMesh, std::vector<VertexPositionNormalTextureSkinning>& vertices) {
    printf("Parsing vertices...\n");

    vertices.reserve(pMesh->mNumVertices + vertices.capacity());
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                {{ pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }}, 
                {{ pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }}, 
                {{ pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }},
                { 0, 0, 0, 0 },
                {{ 0.f, 0.f, 0.f, 0.f }}
                });
    }
}

void ParseIndices(const aiMesh* pMesh, std::vector<std::uint16_t>& indices) {
    printf("Parsing indices...\n");

    indices.reserve((pMesh->mNumFaces * 3) + indices.capacity());
    for (int faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex) {
        for (int faceIndicesIndex = 0; faceIndicesIndex < pMesh->mFaces[faceIndex].mNumIndices; ++faceIndicesIndex) {
            indices.push_back(pMesh->mFaces[faceIndex].mIndices[faceIndicesIndex]);
        }
    }
}

void ParseModel(const aiScene* pScene, Model& model, bool skinned) {
    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* pAIMesh = pScene->mMeshes[i];

        auto pMesh = std::make_shared<Mesh>(pAIMesh->mName.C_Str());
        auto pSkinnedMesh = std::make_shared<SkinnedMesh>(pAIMesh->mName.C_Str());

        auto pSubmesh = std::make_unique<Submesh>(std::string(pAIMesh->mName.C_Str()) + "_submesh", 0);
        if (skinned) {
            ParseSkinnedVertices(pAIMesh, pSkinnedMesh->GetVertices());
            ParseIndices(pAIMesh, pSkinnedMesh->GetIndices());

            std::vector<VertexBoneData> vertexBoneData;
            std::vector<std::uint32_t> startVertices;
            std::unordered_map<std::string, std::uint32_t> boneNameToIndex;

            ParseVertexBoneData(pScene, vertexBoneData, startVertices, boneNameToIndex); 
            for (int i = 0; i < pSkinnedMesh->GetNumVertices(); ++i) {
                VertexBoneData& bd = vertexBoneData[i];
                bd.Calibrate();
                pSkinnedMesh->GetVertices()[i].SetBlendIndices({ bd.Indices[0], bd.Indices[1], bd.Indices[2], bd.Indices[3] });
                pSkinnedMesh->GetVertices()[i].SetBlendWeights({{ bd.Weights[0], bd.Weights[1], bd.Weights[2], bd.Weights[3] }});
            }

            pSubmesh->SetIndexCount(pSkinnedMesh->GetNumIndices());

            pSkinnedMesh->Add(std::move(pSubmesh));
            model.Add(std::move(pSkinnedMesh));
        } else {
            ParseVertices(pAIMesh, pMesh->GetVertices());
            ParseIndices(pAIMesh, pMesh->GetIndices());

            pSubmesh->SetIndexCount(pMesh->GetNumIndices());

            pMesh->Add(std::move(pSubmesh));
            model.Add(std::move(pMesh));
        }
    }
}

void ParsePackedModel(const aiScene* pScene, Model& model, bool skinned) {
    std::vector<VertexPositionNormalTexture> vertices;
    std::vector<VertexPositionNormalTextureSkinning> skinnedVertices;
    std::vector<std::uint16_t> indices;

    auto pMesh = std::make_shared<Mesh>(model.GetName() + "_mesh");
    auto pSkinnedMesh = std::make_shared<SkinnedMesh>(model.GetName() + "_mesh");

    for (int i = 0; i < pScene->mNumMeshes; ++i) {
        const aiMesh* pAIMesh = pScene->mMeshes[i];
        auto pSubmesh = std::make_unique<Submesh>(pAIMesh->mName.C_Str(), 0);
        pSubmesh->SetStartIndex(indices.size());
        pSubmesh->SetVertexOffset(skinned ? skinnedVertices.size() : vertices.size());

        if (skinned)
            ParseSkinnedVertices(pAIMesh, skinnedVertices);
        else
            ParseVertices(pAIMesh, vertices);

        ParseIndices(pAIMesh, indices);
        pSubmesh->SetIndexCount(indices.size() - pSubmesh->GetStartIndex());

        if (skinned)
            pSkinnedMesh->Add(std::move(pSubmesh));
        else
            pMesh->Add(std::move(pSubmesh));
    }
    
    if (skinned) {
        std::vector<VertexBoneData> vertexBoneData;
        std::vector<std::uint32_t> startVertices;
        std::unordered_map<std::string, std::uint32_t> boneNameToIndex;

        ParseVertexBoneData(pScene, vertexBoneData, startVertices, boneNameToIndex); 
        for (int i = 0; i < skinnedVertices.size(); ++i) {
            VertexBoneData& bd = vertexBoneData[i];
            bd.Calibrate();
            skinnedVertices[i].SetBlendIndices({ bd.Indices[0], bd.Indices[1], bd.Indices[2], bd.Indices[3] });
            skinnedVertices[i].SetBlendWeights({{ bd.Weights[0], bd.Weights[1], bd.Weights[2], bd.Weights[3] }});
        }

        pSkinnedMesh->GetVertices() = skinnedVertices;
        pSkinnedMesh->GetIndices() = indices;
        model.Add(std::move(pSkinnedMesh));
    } else {
        pMesh->GetVertices() = vertices;
        pMesh->GetIndices() = indices;
        model.Add(std::move(pMesh));
    }
}

std::shared_ptr<Model> AssetIO::ImportModel(std::string filepath, std::shared_ptr<Material> material, bool skinned, bool packMeshes) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filepath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filepath + "': " + importer.GetErrorString());

    Model model(GetNameFromFilePath(filepath), material);
    ParseBones(pScene, model);
    if (packMeshes)
        ParsePackedModel(pScene, model, skinned);
    else 
        ParseModel(pScene, model, skinned);

    return std::make_shared<Model>(model);
}

