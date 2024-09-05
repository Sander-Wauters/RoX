#include "RoX/AssetIO.h"

#include "Util/pch.h"

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
        if (CurrentIndex > 3)
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
        pMatrix->a1, pMatrix->b1, pMatrix->c1, pMatrix->d1,
        pMatrix->a2, pMatrix->b2, pMatrix->c2, pMatrix->d2,
        pMatrix->a3, pMatrix->b3, pMatrix->c3, pMatrix->d3,
        pMatrix->a4, pMatrix->b4, pMatrix->c4, pMatrix->d4
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

bool IsBone(const aiNode* pNode, BoneNameToAiBone& boneNameToAiBone) {
    return boneNameToAiBone.find(pNode->mName.C_Str()) != boneNameToAiBone.end();
}

void ParseBonesRecursive(
        const aiNode* pNode, 
        BoneNameToAiBone& boneNameToAiBone, 
        Model& model, 
        std::uint32_t parentIndex, 
        const DirectX::XMMATRIX& parentTransform)
{
    std::uint32_t index = parentIndex;

    DirectX::XMMATRIX nodeTransform = ParseMatrix(&pNode->mTransformation);
    DirectX::XMMATRIX globalTransform = parentTransform * nodeTransform;

    if (IsBone(pNode, boneNameToAiBone)) {
        Bone bone(pNode->mName.C_Str(), parentIndex);
        model.GetBones().push_back(bone);

        index = model.GetNumBones() - 1;

        DirectX::XMMATRIX offset = ParseMatrix(&boneNameToAiBone.at(pNode->mName.C_Str())->mOffsetMatrix);
        model.GetInverseBindPoseMatrices()[index] = offset;
        //model.GetBoneMatrices()[index] = globalTransform * offset;
        
        // Bone matrices are FUNCKED no mather what file format or export settings, so just put the identity.
        model.GetBoneMatrices()[index] = DirectX::XMMatrixIdentity();
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
    ParseBonesRecursive(
            pScene->mRootNode, 
            boneNameToAiBone, 
            model, 
            -1, 
            DirectX::XMMatrixIdentity());
}

void ParseVertexBoneData(const aiScene* pScene, std::vector<VertexBoneData>& vertexBoneData, std::vector<std::uint32_t>& startVertices, std::vector<Bone>& bones) {
    std::uint32_t totalVertices = 0;

    startVertices.resize(pScene->mNumMeshes);
    for (std::uint32_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
        const aiMesh* pMesh = pScene->mMeshes[meshIndex];

        startVertices[meshIndex] = totalVertices;
        totalVertices += pMesh->mNumVertices;
        vertexBoneData.resize(totalVertices);

        for (std::uint32_t boneIndex = 0; boneIndex < pMesh->mNumBones; ++boneIndex) {
            const aiBone* pBone = pMesh->mBones[boneIndex];

            std::uint32_t boneId = 0;
            for (std::uint32_t i = 0; i < bones.size(); ++i) {
                if (bones[i].GetName() == pBone->mName.C_Str()) {
                    boneId = i;
                    break;
                }
            }

            for (std::uint32_t weightIndex = 0; weightIndex < pBone->mNumWeights; ++weightIndex) {
                const aiVertexWeight weight = pBone->mWeights[weightIndex];
                vertexBoneData[startVertices[meshIndex] + weight.mVertexId].AddBone(boneId, weight.mWeight);
            }
        }
    }
}

void ParseVertices(const aiMesh* pMesh, std::vector<VertexPositionNormalTexture>& vertices) {
    vertices.reserve(pMesh->mNumVertices + vertices.capacity());
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                { pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z }, 
                { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z }, 
                { pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y }});
    }
}

void ParseSkinnedVertices(const aiMesh* pMesh, std::vector<VertexPositionNormalTextureSkinning>& vertices) {
    vertices.reserve(pMesh->mNumVertices + vertices.capacity());
    for (int i = 0; i < pMesh->mNumVertices; ++i) {
        vertices.push_back({
                { pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z },
                { pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z },
                { pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y },
                { 0, 0, 0, 0 },
                { 0.f, 0.f, 0.f, 0.f }});
    }
}

void ParseIndices(const aiMesh* pMesh, std::vector<std::uint16_t>& indices) {
    indices.reserve((pMesh->mNumFaces * 3) + indices.capacity());
    for (int faceIndex = 0; faceIndex < pMesh->mNumFaces; ++faceIndex) {
        for (int faceIndicesIndex = 0; faceIndicesIndex < pMesh->mFaces[faceIndex].mNumIndices; ++faceIndicesIndex) {
            indices.push_back(pMesh->mFaces[faceIndex].mIndices[faceIndicesIndex]);
        }
    }
}

void ParseModel(const aiScene* pScene, Model& model, bool skinned) {
    std::vector<VertexBoneData> vertexBoneData;
    std::vector<std::uint32_t> startVertices;
    std::unordered_map<std::string, std::uint32_t> boneNameToIndex;
    ParseVertexBoneData(pScene, vertexBoneData, startVertices, model.GetBones()); 

    for (int meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
        const aiMesh* pAIMesh = pScene->mMeshes[meshIndex];

        auto pMesh = std::make_shared<Mesh>(pAIMesh->mName.C_Str());
        auto pSkinnedMesh = std::make_shared<SkinnedMesh>(pAIMesh->mName.C_Str());

        auto pSubmesh = std::make_unique<Submesh>(std::string(pAIMesh->mName.C_Str()) + "_submesh", 0);
        if (skinned) {
            ParseSkinnedVertices(pAIMesh, pSkinnedMesh->GetVertices());
            ParseIndices(pAIMesh, pSkinnedMesh->GetIndices());

            for (int vertexIndex = 0; vertexIndex < pSkinnedMesh->GetNumVertices(); ++vertexIndex) {
                VertexBoneData& bd = vertexBoneData[startVertices[meshIndex] + vertexIndex];
                bd.Calibrate();
                pSkinnedMesh->GetVertices()[vertexIndex].SetBlendIndices({ bd.Indices[0], bd.Indices[1], bd.Indices[2], bd.Indices[3] });
                pSkinnedMesh->GetVertices()[vertexIndex].SetBlendWeights({ bd.Weights[0], bd.Weights[1], bd.Weights[2], bd.Weights[3] });
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

        ParseVertexBoneData(pScene, vertexBoneData, startVertices, model.GetBones()); 
        for (int i = 0; i < skinnedVertices.size(); ++i) {
            VertexBoneData& bd = vertexBoneData[i];
            bd.Calibrate();
            skinnedVertices[i].SetBlendIndices({ bd.Indices[0], bd.Indices[1], bd.Indices[2], bd.Indices[3] });
            skinnedVertices[i].SetBlendWeights({ bd.Weights[0], bd.Weights[1], bd.Weights[2], bd.Weights[3] });
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

void ParseKeyframes(const aiNodeAnim* pNodeAnim, std::vector<Keyframe>& keyframes) {
    for (std::uint32_t i = 0; i < pNodeAnim->mNumScalingKeys; ++i) {
        const aiVectorKey key = pNodeAnim->mScalingKeys[i];

        std::uint32_t keyframeIndex = -1;
        for (std::uint32_t j = 0; j < keyframes.size(); ++j) {
            if (keyframes[j].TimePosition == key.mTime) {
                keyframeIndex = j;
                break;
            }
        }

        if (keyframeIndex != -1)
            keyframes[keyframeIndex].Scale = { key.mValue.x, key.mValue.y, key.mValue.z };
        else {
            Keyframe keyframe = { (float)key.mTime };
            keyframe.Scale = { key.mValue.x, key.mValue.y, key.mValue.z };
            keyframes.push_back(keyframe);
        }
    }
    for (std::uint32_t i = 0; i < pNodeAnim->mNumPositionKeys; ++i) {
        const aiVectorKey key = pNodeAnim->mPositionKeys[i];

        std::uint32_t keyframeIndex = -1;
        for (std::uint32_t j = 0; j < keyframes.size(); ++j) {
            if (keyframes[j].TimePosition == key.mTime) {
                keyframeIndex = j;
                break;
            }
        }

        if (keyframeIndex != -1)
            keyframes[keyframeIndex].Translation = { key.mValue.x, key.mValue.y, key.mValue.z };
        else {
            Keyframe keyframe = { (float)key.mTime };
            keyframe.Translation = { key.mValue.x, key.mValue.y, key.mValue.z };
            keyframes.push_back(keyframe);
        }
    }
    for (std::uint32_t i = 0; i < pNodeAnim->mNumRotationKeys; ++i) {
        const aiQuatKey key = pNodeAnim->mRotationKeys[i];

        std::uint32_t keyframeIndex = -1;
        for (std::uint32_t j = 0; j < keyframes.size(); ++j) {
            if (keyframes[j].TimePosition == key.mTime) {
                keyframeIndex = j;
                break;
            }
        }

        if (keyframeIndex != -1)
            keyframes[keyframeIndex].RotationQuaternion = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };
        else {
            Keyframe keyframe = { (float)key.mTime };
            keyframe.RotationQuaternion = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w };
            keyframes.push_back(keyframe);
        }
    }

    std::sort(keyframes.begin(), keyframes.end());
}

void FillMissingBoneAnimations(
        const aiNode* pNode, 
        BoneNameToAiBone& boneNameToAiBone, 
        std::vector<BoneAnimation>& boneAnimations, 
        std::vector<std::string>& nodeAnimNames,
        std::uint32_t& boneAnimationIndex) 
{
    if (IsBone(pNode, boneNameToAiBone)) {
        bool hasAnimation = false;
        std::uint32_t animationIndex = -1;
        for (std::uint32_t i = 0; i < nodeAnimNames.size(); ++i) {
            if (nodeAnimNames[i] == pNode->mName.C_Str()) {
                hasAnimation = true;
                animationIndex = i;
                break;
            }
        }

        if (hasAnimation && (animationIndex != boneAnimationIndex)) {
            std::swap(boneAnimations[animationIndex], boneAnimations[boneAnimationIndex]);
            std::swap(nodeAnimNames[animationIndex], nodeAnimNames[boneAnimationIndex]);
        }

        if (!hasAnimation) {
            boneAnimations.insert(boneAnimations.begin() + boneAnimationIndex, BoneAnimation());    
            nodeAnimNames.insert(nodeAnimNames.begin() + boneAnimationIndex, pNode->mName.C_Str());
        }
        ++boneAnimationIndex;
    }

    for (std::uint32_t i = 0; i < pNode->mNumChildren; ++i) {
        FillMissingBoneAnimations(pNode->mChildren[i], boneNameToAiBone, boneAnimations, nodeAnimNames, boneAnimationIndex);
    } 
}

std::unordered_map<std::string, std::shared_ptr<Animation>> AssetIO::ImportAnimations(std::string filepath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filepath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filepath + "': " + importer.GetErrorString());

    std::unordered_map<std::string, std::shared_ptr<Animation>> animations;

    BoneNameToAiBone boneNameToAiBone;
    ParseBoneNameToAiBone(pScene, boneNameToAiBone);

    for (std::uint32_t animationIndex = 0; animationIndex < pScene->mNumAnimations; ++animationIndex) {
        const aiAnimation* pAiAnimation = pScene->mAnimations[animationIndex];

        std::shared_ptr<Animation>& pAnimation = animations[pAiAnimation->mName.C_Str()];
        if (!pAnimation)
            pAnimation = std::make_shared<Animation>();

        std::vector<std::string> nodeAnimNames;

        for (std::uint32_t nodeAnimIndex = 0; nodeAnimIndex < pAiAnimation->mNumChannels; ++nodeAnimIndex) {
            const aiNodeAnim* pNodeAnim = pAiAnimation->mChannels[nodeAnimIndex];

            if (boneNameToAiBone.find(pNodeAnim->mNodeName.C_Str()) == boneNameToAiBone.end())
                continue;

            nodeAnimNames.push_back(pNodeAnim->mNodeName.C_Str());

            BoneAnimation boneAnimation;
            ParseKeyframes(pNodeAnim, boneAnimation.Keyframes);
            if (boneAnimation.Keyframes.size() == 1) // A BoneAnimation need at least 2 keyframes.
                boneAnimation.Keyframes.push_back({});

            for (Keyframe& key : boneAnimation.Keyframes) {
                key.TimePosition /= pAiAnimation->mTicksPerSecond == 0 ? 30 : pAiAnimation->mTicksPerSecond;
            }

            pAnimation->BoneAnimations.push_back(std::move(boneAnimation));
        }

        std::uint32_t startIndex = 0;
        FillMissingBoneAnimations(pScene->mRootNode, boneNameToAiBone, pAnimation->BoneAnimations, nodeAnimNames, startIndex);
    }

    return animations;
}
