#include "ModelLoadAssimp.h"

#include <VertexTypes.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Util/Logger.h"

std::unique_ptr<DirectX::Model> ModelLoadAssimp::CreateFromFBX(
        ID3D12Device* pDevice,
        const char* pFileName,
        DirectX::ModelLoaderFlags flags) 
{
    Assimp::Importer importer;
    const aiScene* pScene = importer.ReadFile(pFileName,
            aiProcess_Triangulate |
            aiProcess_MakeLeftHanded |
            aiProcess_JoinIdenticalVertices);
    if (!pScene)
        throw std::runtime_error(importer.GetErrorString());
    if (!pScene->HasMeshes())
        throw std::runtime_error("No meshes found");
    if (pScene->mNumSkeletons > 1)
        throw std::runtime_error("More than 1 skeleton");

    std::vector<DirectX::Model::ModelMaterialInfo> materials;
    materials.resize(pScene->mNumMaterials);

    // TEMP: skeleton animation
    DirectX::ModelBone::Collection bones;
    DirectX::ModelBone::TransformArray boneTransforms;
    DirectX::ModelBone::TransformArray invBoneTransforms;
    if (pScene->mNumSkeletons > 0) {
        bones.reserve(pScene->mSkeletons[0]->mNumBones);
        boneTransforms = DirectX::ModelBone::MakeArray(pScene->mSkeletons[0]->mNumBones);
        invBoneTransforms = DirectX::ModelBone::MakeArray(pScene->mSkeletons[0]->mNumBones);
    } 

    std::unique_ptr<DirectX::Model> pModel = std::make_unique<DirectX::Model>();
    pModel->meshes.reserve(pScene->mNumMeshes);

    std::uint32_t partCount = 0;
    for (size_t meshIndex = 0; meshIndex < pScene->mNumMeshes; ++meshIndex) {
        const aiMesh* pMesh = pScene->mMeshes[meshIndex];

        std::shared_ptr<DirectX::ModelMesh> pModelMesh = std::make_shared<DirectX::ModelMesh>(); 
        std::wstring meshName(
                &pMesh->mName.C_Str()[0], 
                &pMesh->mName.C_Str()[0] + pMesh->mName.length);
        pModelMesh->name = meshName;

        materials[pMesh->mMaterialIndex] = CreateModelMaterialInfo(pScene->mMaterials[pMesh->mMaterialIndex]);

        size_t ibByteSize;
        DirectX::SharedGraphicsResource ib;
        CreateIndexBuffer(pDevice, pMesh, ib, ibByteSize);

        size_t vbByteSize;
        DirectX::SharedGraphicsResource vb;
        CreateVertexBuffer(pDevice, pMesh, vb, vbByteSize);

        DirectX::ModelMeshPart* pPart = new DirectX::ModelMeshPart(partCount++); 
        pPart->materialIndex = pMesh->mMaterialIndex;
        pPart->indexCount = pMesh->mNumFaces * 3;
        pPart->startIndex = 0;
        pPart->vertexOffset;
        pPart->vertexStride = static_cast<std::uint32_t>(sizeof(DirectX::VertexPositionNormalTexture));
        pPart->vertexCount = pMesh->mNumVertices;
        pPart->indexBufferSize = static_cast<std::uint32_t>(ibByteSize);
        pPart->vertexBufferSize = static_cast<std::uint32_t>(vbByteSize);
        pPart->primitiveType;
        pPart->indexFormat;
        pPart->indexBuffer = std::move(ib);
        pPart->vertexBuffer = std::move(vb);
        pPart->vbDecl = std::make_shared<DirectX::ModelMeshPart::InputLayoutCollection>(
                DirectX::VertexPositionNormalTexture::InputLayout.pInputElementDescs,
                DirectX::VertexPositionNormalTexture::InputLayout.pInputElementDescs +
                DirectX::VertexPositionNormalTexture::InputLayout.NumElements);
     
        // TODO: if opaque
        pModelMesh->opaqueMeshParts.emplace_back(pPart);
        // else
        //pModelMesh->alphaMeshParts.emplace_back(pPart);

        pModel->meshes.emplace_back(pModelMesh);
    }

    pModel->materials = std::move(materials);
    pModel->bones = std::move(bones);
    pModel->boneMatrices = std::move(boneTransforms);
    pModel->invBindPoseMatrices = std::move(invBoneTransforms);

    return pModel;
}

DirectX::Model::ModelMaterialInfo ModelLoadAssimp::CreateModelMaterialInfo(aiMaterial* pMaterial) {
        aiString name;
        aiColor3D ambient;
        aiColor3D diffuse;
        aiColor3D specular;
        aiColor3D emissive;
        
        pMaterial->Get(AI_MATKEY_NAME, name);
        pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);

        DirectX::Model::ModelMaterialInfo matInfo = {};
        matInfo.name = std::wstring(name.C_Str(), name.C_Str() + strlen(name.C_Str()));

        matInfo.perVertexColor;
        matInfo.enableSkinning;
        matInfo.enableDualTexture;
        matInfo.enableNormalMaps;
        matInfo.biasedVertexNormals;

        matInfo.specularPower;
        matInfo.alphaValue = 1.0f;

        matInfo.ambientColor = { ambient.r, ambient.g, ambient.b };
        matInfo.diffuseColor = { diffuse.r, diffuse.g, diffuse.b };
        matInfo.specularColor = { specular.r, specular.g, specular.b };
        matInfo.emissiveColor = { emissive.r, emissive.g, emissive.b };

        matInfo.diffuseTextureIndex;
        matInfo.specularTextureIndex;
        matInfo.normalTextureIndex;
        matInfo.emissiveTextureIndex;
        matInfo.samplerIndex;
        matInfo.samplerIndex2;

        return matInfo;
}

void ModelLoadAssimp::CreateIndexBuffer(
        ID3D12Device* pDevice, 
        const aiMesh* pMesh, 
        DirectX::SharedGraphicsResource& ib, 
        size_t& ibByteSize) 
{
        std::vector<std::uint16_t> indices;
        indices.reserve(pMesh->mNumFaces * 3);
        for (int i = 0; i < pMesh->mNumFaces; ++i) {
            for (int j = 0; j < pMesh->mFaces[i].mNumIndices; ++j){
                indices.emplace_back(pMesh->mFaces[i].mIndices[j]);
            }
        }

        ibByteSize = pMesh->mNumFaces * 3 * sizeof(std::uint16_t);
        ib = DirectX::GraphicsMemory::Get(pDevice).Allocate(ibByteSize, 16, DirectX::GraphicsMemory::TAG_INDEX);
        memcpy(ib.Memory(), indices.data(), ibByteSize);
    
}

void ModelLoadAssimp::CreateVertexBuffer(ID3D12Device* pDevice, 
        const aiMesh* pMesh, 
        DirectX::SharedGraphicsResource& vb, 
        size_t& vbByteSize) 
{
        std::vector<DirectX::VertexPositionNormalTexture> vertices;
        vertices.reserve(pMesh->mNumVertices);
        for (int i = 0; i < pMesh->mNumVertices; ++i) {
            DirectX::XMFLOAT3 position = {
                pMesh->mVertices[i].x,
                pMesh->mVertices[i].y,
                pMesh->mVertices[i].z
            };
            DirectX::XMFLOAT3 normal = {
                pMesh->mNormals[i].x,
                pMesh->mNormals[i].y,
                pMesh->mNormals[i].z
            };
            DirectX::XMFLOAT2 texture = {
                pMesh->mTextureCoords[0][i].x,
                pMesh->mTextureCoords[0][i].y,
            };
            DirectX::VertexPositionNormalTexture vertex(position, normal, texture);
            vertices.emplace_back(vertex);
        }

        vbByteSize = pMesh->mNumVertices * sizeof(DirectX::VertexPositionNormalTexture);
        vb = DirectX::GraphicsMemory::Get(pDevice).Allocate(vbByteSize, 16, DirectX::GraphicsMemory::TAG_VERTEX);
        memcpy(vb.Memory(), vertices.data(), vbByteSize);
}
