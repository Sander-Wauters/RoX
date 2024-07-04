#include "MeshDeviceData.h"


MeshDeviceData::MeshDeviceData(ID3D12Device* pDevice, Mesh* pMesh) {
    m_submeshes.reserve(pMesh->GetNumSubmeshes());
    for (std::uint64_t i = 0; i < pMesh->GetNumSubmeshes(); ++i) {
        m_submeshes.push_back(std::make_unique<SubmeshDeviceData>(pDevice, pMesh->GetSubmeshes()[i].get()));
    }
    m_submeshes.shrink_to_fit();
}

void MeshDeviceData::OnDeviceLost() noexcept {
    for (std::unique_ptr<SubmeshDeviceData>& pSubmesh : m_submeshes) {
        pSubmesh->OnDeviceLost();
    }
}

std::vector<std::unique_ptr<SubmeshDeviceData>>& MeshDeviceData::GetSubmeshes() noexcept {
    return m_submeshes;
}
