#include "RoX/AssetIO.h"

#include "../Util/pch.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded)

#include "../FileFormats/RoXModl.h"
#include "../FileFormats/RoXAnim.h"

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

std::shared_ptr<Model> AssetIO::ImportModel(std::string filePath, std::shared_ptr<Material> material, bool skinned, bool packMeshes) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());

    Model model(material, GetNameFromFilePath(filePath));
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

std::unordered_map<std::string, std::shared_ptr<Animation>> AssetIO::ImportAnimations(std::string filePath) {
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);

    if (!pScene)
        throw std::runtime_error("Error parsing '" + filePath + "': " + importer.GetErrorString());

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

std::shared_ptr<Model> AssetIO::ImportRoXModl(std::string filePath, std::shared_ptr<Material> pMaterial) {
    std::ifstream fin(filePath, std::ios::binary);
    if (!fin.is_open())
        throw std::runtime_error("Failed to open file: '" + filePath + "'");

    fin.seekg(0);

    ROXMODL::HEADER modelHeader = {};
    fin.read(reinterpret_cast<char*>(&modelHeader), sizeof(ROXMODL::HEADER));

    char* modelName = new char[modelHeader.NameSizeInBytes + 1];
    fin.read(modelName, modelHeader.NameSizeInBytes);
    modelName[modelHeader.NameSizeInBytes] = '\0';

    std::vector<Bone> bones;
    bones.reserve(modelHeader.NumBones);
    for (std::uint8_t i = 0; i < modelHeader.NumBones; ++i) {
        ROXMODL::BONE_HEADER boneHeader = {};
        fin.read(reinterpret_cast<char*>(&boneHeader), sizeof(ROXMODL::BONE_HEADER));

        char* boneName = new char[boneHeader.NameSizeInBytes + 1];
        fin.read(boneName, boneHeader.NameSizeInBytes);
        boneName[boneHeader.NameSizeInBytes] = '\0';

        bones.push_back({ boneName, boneHeader.ParentIndex });
        delete [] boneName;
    }

    std::vector<DirectX::XMFLOAT4X4> boneMatrices;
    boneMatrices.resize(modelHeader.NumBones);
    fin.read(reinterpret_cast<char*>(boneMatrices.data()), sizeof(DirectX::XMFLOAT4X4) * modelHeader.NumBones);

    std::vector<DirectX::XMFLOAT4X4> inverseBoneMatrices;
    inverseBoneMatrices.resize(modelHeader.NumBones);
    fin.read(reinterpret_cast<char*>(inverseBoneMatrices.data()), sizeof(DirectX::XMFLOAT4X4) * modelHeader.NumBones);

    std::vector<std::shared_ptr<IMesh>> meshes;
    meshes.reserve(modelHeader.NumMeshes);
    for (std::uint32_t i = 0; i < modelHeader.NumMeshes; ++i) {
        ROXMODL::MESH_HEADER meshHeader = {};
        fin.read(reinterpret_cast<char*>(&meshHeader), sizeof(ROXMODL::MESH_HEADER));

        char* meshName = new char[meshHeader.NameSizeInBytes + 1];
        fin.read(meshName, meshHeader.NameSizeInBytes);
        meshName[meshHeader.NameSizeInBytes] = '\0';

        std::shared_ptr<IMesh> pMesh;
        if (!meshHeader.IsSkinned)
            pMesh = std::make_shared<Mesh>(meshName);
        else
            pMesh = std::make_shared<SkinnedMesh>(meshName);
        delete [] meshName;

        pMesh->GetBoneInfluences().resize(sizeof(std::uint32_t) * meshHeader.NumBoneInfluences);
        fin.read(reinterpret_cast<char*>(pMesh->GetBoneInfluences().data()), sizeof(std::uint32_t) * meshHeader.NumBoneInfluences);

        for (std::uint32_t j = 0; j < meshHeader.NumSubmeshes; ++j) {
            ROXMODL::SUBMESH_HEADER submeshHeader = {};
            fin.read(reinterpret_cast<char*>(&submeshHeader), sizeof(ROXMODL::SUBMESH_HEADER));

            char* submeshName = new char[submeshHeader.NameSizeInBytes + 1];
            fin.read(submeshName, submeshHeader.NameSizeInBytes);
            submeshName[submeshHeader.NameSizeInBytes] = '\0';
            
            auto pSubmesh = std::make_unique<Submesh>(submeshName, submeshHeader.MaterialIndex);
            delete [] submeshName;
            pSubmesh->SetIndexCount(submeshHeader.IndexCount);
            pSubmesh->SetStartIndex(submeshHeader.StartIndex);
            pSubmesh->SetVertexOffset(submeshHeader.VertexOffset);

            pMesh->Add(std::move(pSubmesh));
        }

        ROXMODL::INDEX_BUFFER_HEADER ibHeader = {};
        fin.read(reinterpret_cast<char*>(&ibHeader), sizeof(ROXMODL::INDEX_BUFFER_HEADER));

        pMesh->GetIndices().resize(ibHeader.NumIndices);
        fin.read(reinterpret_cast<char*>(pMesh->GetIndices().data()), ibHeader.IndexSizeInBytes * ibHeader.NumIndices);

        ROXMODL::VERTEX_BUFFER_HEADER vbHeader = {};
        fin.read(reinterpret_cast<char*>(&vbHeader), sizeof(ROXMODL::VERTEX_BUFFER_HEADER));

        if (auto p = dynamic_cast<Mesh*>(pMesh.get())) {
            p->GetVertices().resize(vbHeader.NumVertices);
            fin.read(reinterpret_cast<char*>(p->GetVertices().data()), vbHeader.VertexSizeInBytes * vbHeader.NumVertices);
        } else if (auto p = dynamic_cast<SkinnedMesh*>(pMesh.get())) {
            p->GetVertices().resize(vbHeader.NumVertices);
            fin.read(reinterpret_cast<char*>(p->GetVertices().data()), vbHeader.VertexSizeInBytes * vbHeader.NumVertices);
        } else
            throw std::runtime_error("Failed to downcast IMesh.");
 
        meshes.push_back(std::move(pMesh));
    }

    fin.close();

    auto pModel = std::make_shared<Model>(pMaterial, modelName);
    delete [] modelName;
    pModel->GetMeshes() = std::move(meshes);
    pModel->GetBones() = std::move(bones);
    pModel->MakeBoneMatricesArray(pModel->GetNumBones());;
    for (std::uint8_t i = 0; i < pModel->GetNumBones(); ++i) {
        pModel->GetBoneMatrices()[i] = DirectX::XMLoadFloat4x4(&boneMatrices[i]);
    }
    pModel->MakeInverseBoneMatricesArray(pModel->GetNumBones());;
    for (std::uint8_t i = 0; i < pModel->GetNumBones(); ++i) {
        pModel->GetInverseBindPoseMatrices()[i] = DirectX::XMLoadFloat4x4(&inverseBoneMatrices[i]);
    }

    return pModel;
}

void AssetIO::ExportRoXModl(std::shared_ptr<Model>& pModel, std::string filePath) {
    std::ofstream fout(filePath, std::ios::binary);
    if (!fout.is_open())
        throw std::runtime_error("Failed to open file: '" + filePath + ".roxmodl'");

    fout.seekp(0);

    ROXMODL::HEADER modelHeader;
    modelHeader.Version = 1;
    modelHeader.NameSizeInBytes = pModel->GetName().length();
    modelHeader.NumBones = pModel->GetNumBones();
    modelHeader.NumMeshes = pModel->GetNumMeshes();
    modelHeader.NumMaterials = pModel->GetNumMaterials();
    fout.write(reinterpret_cast<char*>(&modelHeader), sizeof(ROXMODL::HEADER));
    fout.write(pModel->GetName().c_str(), modelHeader.NameSizeInBytes);

    for (Bone& bone : pModel->GetBones()) {
        ROXMODL::BONE_HEADER boneHeader;
        boneHeader.NameSizeInBytes = bone.GetName().length();
        boneHeader.ParentIndex = bone.GetParentIndex();

        fout.write(reinterpret_cast<char*>(&boneHeader), sizeof(ROXMODL::BONE_HEADER));
        fout.write(bone.GetName().c_str(), boneHeader.NameSizeInBytes);
    }

    std::vector<DirectX::XMFLOAT4X4> boneMatrices;
    boneMatrices.resize(pModel->GetNumBones());
    for (std::uint8_t i = 0; i < pModel->GetNumBones(); ++i) {
        DirectX::XMStoreFloat4x4(&boneMatrices[i], pModel->GetBoneMatrices()[i]);
    }
    fout.write(reinterpret_cast<char*>(boneMatrices.data()), sizeof(DirectX::XMFLOAT4X4) * pModel->GetNumBones());

    std::vector<DirectX::XMFLOAT4X4> inverseBoneMatrices;
    inverseBoneMatrices.resize(pModel->GetNumBones());
    for (std::uint8_t i = 0; i < pModel->GetNumBones(); ++i) {
        DirectX::XMStoreFloat4x4(&inverseBoneMatrices[i], pModel->GetInverseBindPoseMatrices()[i]);
    }
    fout.write(reinterpret_cast<char*>(inverseBoneMatrices.data()), sizeof(DirectX::XMFLOAT4X4) * pModel->GetNumBones());

    for (std::shared_ptr<IMesh>& pMesh : pModel->GetMeshes()) {
        ROXMODL::MESH_HEADER meshHeader;
        if (dynamic_cast<Mesh*>(pMesh.get()))
            meshHeader.IsSkinned = false;
        else if (dynamic_cast<SkinnedMesh*>(pMesh.get()))
            meshHeader.IsSkinned = true;
        else
            throw std::runtime_error("Failed to downcast IMesh.");
        meshHeader.NameSizeInBytes = pMesh->GetName().length();
        meshHeader.NumBoneInfluences = pMesh->GetNumBoneInfluences();
        meshHeader.NumSubmeshes = pMesh->GetNumSubmeshes();

        fout.write(reinterpret_cast<char*>(&meshHeader), sizeof(ROXMODL::MESH_HEADER));
        fout.write(pMesh->GetName().c_str(), meshHeader.NameSizeInBytes);
        if (meshHeader.NumBoneInfluences)
            fout.write(reinterpret_cast<char*>(pMesh->GetBoneInfluences().data()), sizeof(std::uint32_t) * meshHeader.NameSizeInBytes);

        for (std::unique_ptr<Submesh>& pSubmesh : pMesh->GetSubmeshes()) {
            ROXMODL::SUBMESH_HEADER submeshHeader;
            submeshHeader.NameSizeInBytes = pSubmesh->GetName().length();
            submeshHeader.MaterialIndex = pSubmesh->GetMaterialIndex();
            submeshHeader.IndexCount = pSubmesh->GetIndexCount();
            submeshHeader.StartIndex = pSubmesh->GetStartIndex();
            submeshHeader.VertexOffset = pSubmesh->GetVertexOffset();

            fout.write(reinterpret_cast<char*>(&submeshHeader), sizeof(ROXMODL::SUBMESH_HEADER));
            fout.write(pSubmesh->GetName().c_str(), submeshHeader.NameSizeInBytes);
        }

        ROXMODL::INDEX_BUFFER_HEADER ibHeader;
        ibHeader.NumIndices = pMesh->GetNumIndices();
        ibHeader.IndexSizeInBytes = sizeof(std::uint16_t);

        fout.write(reinterpret_cast<char*>(&ibHeader), sizeof(ROXMODL::INDEX_BUFFER_HEADER));
        fout.write(reinterpret_cast<char*>(pMesh->GetIndices().data()), ibHeader.IndexSizeInBytes * ibHeader.NumIndices);

        ROXMODL::VERTEX_BUFFER_HEADER vbHeader;
        vbHeader.NumVertices = pMesh->GetNumVertices();
        if (auto p = dynamic_cast<Mesh*>(pMesh.get())) {
            vbHeader.VertexSizeInBytes = sizeof(VertexPositionNormalTexture);

            fout.write(reinterpret_cast<char*>(&vbHeader), sizeof(ROXMODL::VERTEX_BUFFER_HEADER));
            fout.write(reinterpret_cast<char*>(p->GetVertices().data()), vbHeader.VertexSizeInBytes * vbHeader.NumVertices);
        } else if (auto p = dynamic_cast<SkinnedMesh*>(pMesh.get())) {
            vbHeader.VertexSizeInBytes = sizeof(VertexPositionNormalTextureSkinning);

            fout.write(reinterpret_cast<char*>(&vbHeader), sizeof(ROXMODL::VERTEX_BUFFER_HEADER));
            fout.write(reinterpret_cast<char*>(p->GetVertices().data()), vbHeader.VertexSizeInBytes * vbHeader.NumVertices);
        } else
            throw std::runtime_error("Failed to downcast IMesh.");
    }

    fout.close();
}

std::shared_ptr<Animation> AssetIO::ImportRoXAnim(std::string filePath) {
    std::ifstream fin = std::ifstream(filePath, std::ios::binary);
    if (!fin.is_open())
        throw std::runtime_error("Failed to open file: '" + filePath + ".roxmodl'");

    fin.seekg(0);

    ROXANIM::ANIM_HEADER animHeader;
    fin.read(reinterpret_cast<char*>(&animHeader), sizeof(ROXANIM::ANIM_HEADER));

    auto pAnim = std::make_shared<Animation>();
    pAnim->BoneAnimations.resize(animHeader.NumBoneAnimations);

    for (std::uint32_t i = 0; i < animHeader.NumBoneAnimations; ++i) {
        ROXANIM::BONE_ANIM_HEADER boneAnimHeader;
        fin.read(reinterpret_cast<char*>(&boneAnimHeader), sizeof(ROXANIM::BONE_ANIM_HEADER));

        pAnim->BoneAnimations[i].Keyframes.resize(boneAnimHeader.NumKeyframes);
        fin.read(reinterpret_cast<char*>(pAnim->BoneAnimations[i].Keyframes.data()), boneAnimHeader.KeyframeSizeInBytes * boneAnimHeader.NumKeyframes);
    }

    fin.close();
    return pAnim;
}

void AssetIO::ExportRoXAnim(std::shared_ptr<Animation>& pAnim, std::string filePath) {
    std::ofstream fout = std::ofstream(filePath, std::ios::binary);
    if (!fout.is_open())
        throw std::runtime_error("Failed to open file: '" + filePath + ".roxmodl'");

    fout.seekp(0);

    ROXANIM::ANIM_HEADER animHeader;
    animHeader.NumBoneAnimations = pAnim->GetNumBoneAnimations();
    fout.write(reinterpret_cast<char*>(&animHeader), sizeof(ROXANIM::ANIM_HEADER));

    for (std::uint32_t i = 0; i < animHeader.NumBoneAnimations; ++i) {
        ROXANIM::BONE_ANIM_HEADER boneAnimHeader;
        boneAnimHeader.NumKeyframes = pAnim->BoneAnimations[i].GetNumKeyframes();
        boneAnimHeader.KeyframeSizeInBytes = sizeof(Keyframe);

        fout.write(reinterpret_cast<char*>(&boneAnimHeader), sizeof(ROXANIM::BONE_ANIM_HEADER));
        fout.write(reinterpret_cast<char*>(pAnim->BoneAnimations[i].Keyframes.data()), boneAnimHeader.KeyframeSizeInBytes * boneAnimHeader.NumKeyframes);
    }

    fout.close();
}

