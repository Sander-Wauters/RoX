#pragma once

#include "RoX/Model.h"

#include "SubmeshDeviceData.h"

class MeshDeviceData {
    public:
        MeshDeviceData(ID3D12Device* pDevice, Mesh* pMesh);

        void OnDeviceLost() noexcept;

    public:
        std::vector<std::unique_ptr<SubmeshDeviceData>>& GetSubmeshes() noexcept;

    private:
        std::vector<std::unique_ptr<SubmeshDeviceData>> m_submeshes;
};
