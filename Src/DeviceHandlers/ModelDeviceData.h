#pragma once

#include "../Util/pch.h"

#include "RoX/Model.h"

#include "MaterialDeviceData.h"
#include "MeshDeviceData.h"
#include "IDeviceDataSupplier.h"

class ModelDeviceData : public IModelObserver {
    public:
        ModelDeviceData(IDeviceDataSupplier& deviceDataSupplier, Model& model);

    public:
        void OnAdd(const std::shared_ptr<Material>& pMaterial) override;
        void OnAdd(const std::shared_ptr<IMesh>& pIMesh) override;

        void OnRemoveMaterial(std::uint8_t index) override;
        void OnRemoveIMesh(std::uint8_t index) override;

    public:
        void DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model* pModel);
        void LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory = false);

    public:
        std::vector<std::shared_ptr<MaterialDeviceData>>& GetMaterials() noexcept;
        std::vector<std::shared_ptr<MeshDeviceData>>& GetMeshes() noexcept;

    private:
        IDeviceDataSupplier& m_deviceDataSupplier;

        std::vector<std::shared_ptr<MaterialDeviceData>> m_materials; 
        std::vector<std::shared_ptr<MeshDeviceData>> m_meshes;
};
