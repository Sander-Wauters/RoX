#pragma once

#include <unordered_map>

#include <Effects.h>

#include "RoX/Model.h"

#include "MeshDeviceData.h"

class ModelDeviceData {
    public:
        ModelDeviceData(ID3D12Device* pDevice, Model* pModel, 
            std::unordered_map<std::shared_ptr<IMesh>, std::unique_ptr<MeshDeviceData>>& sharedMeshes);

        void DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model* pModel);
        void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);

    public:
        std::vector<std::unique_ptr<DirectX::IEffect>*>& GetEffects() noexcept;
        std::vector<MeshDeviceData*>& GetMeshes() noexcept;

    private:
        std::vector<std::unique_ptr<DirectX::IEffect>*> m_effects; 
        std::vector<MeshDeviceData*> m_meshes;
};
