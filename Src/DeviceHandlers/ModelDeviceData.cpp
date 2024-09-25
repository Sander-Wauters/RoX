#include "ModelDeviceData.h"

ModelDeviceData::ModelDeviceData(IDeviceDataSupplier& deviceDataSupplier, Model& model) 
    : m_deviceDataSupplier(deviceDataSupplier)
{
    m_meshes.reserve(model.GetNumMeshes()); 
    for (std::uint8_t i = 0; i < model.GetNumMeshes(); ++i) {
        std::shared_ptr<IMesh>& pIMesh = model.GetMeshes()[i];
        m_meshes.push_back(m_deviceDataSupplier.GetMeshDeviceData(pIMesh));
    }
    for (std::uint8_t i = 0; i < model.GetNumMaterials(); ++i) {
        m_materials.push_back(m_deviceDataSupplier.GetMaterialDeviceData(model.GetMaterials()[i]));
    }
    m_meshes.shrink_to_fit();
}

void ModelDeviceData::OnAdd(const std::shared_ptr<Material>& pMaterial) {
    m_materials.push_back(m_deviceDataSupplier.GetMaterialDeviceData(pMaterial));
}

void ModelDeviceData::OnAdd(const std::shared_ptr<IMesh>& pIMesh) {
    m_meshes.push_back(m_deviceDataSupplier.GetMeshDeviceData(pIMesh));
}

void ModelDeviceData::OnRemoveMaterial(std::uint8_t index) {
    m_materials.erase(m_materials.begin() + index);
}

void ModelDeviceData::OnRemoveIMesh(std::uint8_t index) {
    m_meshes.erase(m_meshes.begin() + index);
    m_deviceDataSupplier.SignalMeshRemoved();
}

void ModelDeviceData::DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model* pModel) {
    assert(pModel->GetNumBones() > 0 && pModel->GetBoneMatrices() != nullptr);

    Bone::TransformArray temp;
    for (std::uint64_t meshIndex = 0; meshIndex < pModel->GetNumMeshes(); ++meshIndex) {
        IMesh* pMesh = pModel->GetMeshes()[meshIndex].get();

        m_meshes[meshIndex]->PrepareForDraw();

        for (std::uint64_t submeshIndex = 0; submeshIndex < pMesh->GetNumSubmeshes(); ++submeshIndex) {
            Submesh* pSubmesh = pMesh->GetSubmeshes()[submeshIndex].get();
            DirectX::IEffect* pIEffect = m_materials[pSubmesh->GetMaterialIndex()]->GetIEffect();

            auto pIMatrices = dynamic_cast<DirectX::IEffectMatrices*>(pIEffect);
            if (pIMatrices)
                pIMatrices->SetWorld(DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0]));

            auto pISkinning = dynamic_cast<DirectX::IEffectSkinning*>(pIEffect);
            if (pISkinning) {
                if (pMesh->GetBoneInfluences().empty())
                    pISkinning->SetBoneTransforms(pModel->GetBoneMatrices().get(), pModel->GetNumBones());
                else {
                    if (!temp) {
                        temp = Bone::MakeArray(DirectX::IEffectSkinning::MaxBones);

                        std::uint64_t count = 0;
                        for (std::uint32_t& influence : pMesh->GetBoneInfluences()) {
                            ++count;
                            if (count > DirectX::IEffectSkinning::MaxBones)
                                throw std::runtime_error("Too many bones for skinning.");
                            if (influence >= pModel->GetNumBones())
                                throw std::runtime_error("Invalid bone influence index.");
                            temp[count - 1] = pModel->GetBoneMatrices()[influence];
                        }
                    }

                    pISkinning->SetBoneTransforms(temp.get(), pMesh->GetBoneInfluences().size());
                }
            } else if (pIMatrices) {
                DirectX::XMMATRIX boneTransforms = (pMesh->GetBoneIndex() != Bone::INVALID_INDEX && pMesh->GetBoneIndex() < pModel->GetNumBones()) 
                    ? pModel->GetBoneMatrices()[pMesh->GetBoneIndex()] : DirectX::XMMatrixIdentity();

                pIMatrices->SetWorld(DirectX::XMMatrixMultiply(boneTransforms, DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0])));
            }

            pIEffect->Apply(pCommandList);
            pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
        }
    }
}

void ModelDeviceData::LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory) {
    std::set<MeshDeviceData*> uniqueMeshes;
    for (std::shared_ptr<MeshDeviceData>& pMeshData : m_meshes) {
        uniqueMeshes.insert(pMeshData.get());
    }

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    for (auto it = uniqueMeshes.cbegin(); it != uniqueMeshes.cend(); ++it) {
        MeshDeviceData* pMeshData = *it;

        if (!pMeshData->GetStaticVertexBuffer()) {
            if (!pMeshData->GetVertexBuffer())
                std::runtime_error("Mesh is missing vertex buffer");

            pMeshData->LoadStaticIndexBuffer(keepMemory);

            // Scan for any other part with the same index buffer for sharing
            for (auto sit = std::next(it); sit != uniqueMeshes.cend(); ++sit) {
                MeshDeviceData* pSharedMesh = *sit;
                assert(pSharedMesh != pMeshData);

                if (pSharedMesh->GetStaticVertexBuffer())
                    continue;

                if (pSharedMesh->GetVertexBuffer() == pMeshData->GetVertexBuffer()) {
                    pSharedMesh->SetVertexBufferSize(pMeshData->GetVertexBufferSize());
                    pSharedMesh->SetStaticVertexBuffer(pMeshData->GetStaticVertexBuffer().Get());

                    if (!keepMemory)
                        pSharedMesh->GetVertexBuffer().Reset();
                }
            }

            if (!keepMemory)
                pMeshData->GetVertexBuffer().Reset();
        }

        if (!pMeshData->GetStaticIndexBuffer()) {
            if (!pMeshData->GetIndexBuffer())
                std::runtime_error("Submesh is missing index buffer");

            pMeshData->LoadStaticIndexBuffer(keepMemory);

            // Scan for any other part with the same index buffer for sharing
            for (auto sit = std::next(it); sit != uniqueMeshes.cend(); ++sit) {
                MeshDeviceData* pSharedMeshData = *sit;
                assert(pSharedMeshData != pMeshData);

                if (pSharedMeshData->GetStaticIndexBuffer())
                    continue;

                if (pSharedMeshData->GetIndexBuffer() == pMeshData->GetIndexBuffer()) {
                    pSharedMeshData->SetIndexBufferSize(pMeshData->GetIndexBufferSize());
                    pSharedMeshData->SetStaticIndexBuffer(pMeshData->GetStaticIndexBuffer().Get());

                    if (!keepMemory)
                        pSharedMeshData->GetIndexBuffer().Reset();
                }
            }

            if (!keepMemory)
                pMeshData->GetIndexBuffer().Reset();
        }
    }
}

std::vector<std::shared_ptr<MaterialDeviceData>>& ModelDeviceData::GetMaterials() noexcept {
    return m_materials;
}

std::vector<std::shared_ptr<MeshDeviceData>>& ModelDeviceData::GetMeshes() noexcept {
    return m_meshes;
}
