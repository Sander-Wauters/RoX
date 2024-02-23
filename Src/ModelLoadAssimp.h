#pragma once

#include <Model.h>

#include <assimp/mesh.h>
#include <assimp/material.h>

#include "Util/pch.h"

class ModelLoadAssimp {
    public:
        ModelLoadAssimp();
        ~ModelLoadAssimp();

        static std::unique_ptr<DirectX::Model> CreateFromFBX(
            ID3D12Device* pDevice,
            const char* pFileName,
            DirectX::ModelLoaderFlags flags = DirectX::ModelLoader_Default);

    private:
        DirectX::Model::ModelMaterialInfo static CreateModelMaterialInfo(aiMaterial* pMaterial);
        void static CreateIndexBuffer(
                ID3D12Device* pDevice,
                const aiMesh* pMesh, 
                DirectX::SharedGraphicsResource& ib, 
                size_t& ibByteSize);
        void static CreateVertexBuffer(
                ID3D12Device* pDevice, 
                const aiMesh* pMesh, 
                DirectX::SharedGraphicsResource& vb, 
                size_t& vbByteSize);
};
