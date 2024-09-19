#pragma once

#include "../Util/pch.h"

#include "RoX/Model.h"

#include "MeshDeviceData.h"

class ModelDeviceData {
    public:
        ModelDeviceData(ID3D12Device* pDevice, Model* pModel, 
            std::unordered_map<std::shared_ptr<IMesh>, std::shared_ptr<MeshDeviceData>>& sharedMeshes);

        void DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model* pModel);
        void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);

    public:
        std::vector<DirectX::IEffect*>& GetEffects() noexcept;
        std::vector<MeshDeviceData*>& GetMeshes() noexcept;

    private:
        std::vector<DirectX::IEffect*> m_effects; 
        std::vector<std::shared_ptr<MeshDeviceData>> m_meshes;
};
