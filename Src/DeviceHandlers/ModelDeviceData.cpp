#include "ModelDeviceData.h"

ModelDeviceData::ModelDeviceData(ID3D12Device* pDevice, Model* pModel, 
        std::unordered_map<std::shared_ptr<IMesh>, std::unique_ptr<MeshDeviceData>>& sharedMeshes) 
{
    m_meshes.reserve(pModel->GetNumMeshes()); 
    for (std::uint64_t i = 0; i < pModel->GetNumMeshes(); ++i) {
        std::unique_ptr<MeshDeviceData>& pMeshData = sharedMeshes[pModel->GetMeshes()[i]];
        if (!pMeshData) {
            pMeshData = std::make_unique<MeshDeviceData>(pDevice, pModel->GetMeshes()[i].get());
        }
        m_meshes.push_back(pMeshData.get());
    }
    m_meshes.shrink_to_fit();
}

void ModelDeviceData::DrawSkinned(ID3D12GraphicsCommandList* pCommandList, Model* pModel) {
    assert(pModel->GetNumBones() > 0 && pModel->GetBoneMatrices() != nullptr);

    Bone::TransformArray temp;
    for (std::uint64_t meshIndex = 0; meshIndex < pModel->GetNumMeshes(); ++meshIndex) {
        IMesh* pMesh = pModel->GetMeshes()[meshIndex].get();

        m_meshes[meshIndex]->PrepareForDraw(pCommandList);

        for (std::uint64_t submeshIndex = 0; submeshIndex < pMesh->GetNumSubmeshes(); ++submeshIndex) {
            Submesh* pSubmesh = pMesh->GetSubmeshes()[submeshIndex].get();
            DirectX::IEffect* pEffect = m_effects[pSubmesh->GetMaterialIndex()];

            auto iMatrices = dynamic_cast<DirectX::IEffectMatrices*>(pEffect);
            if (iMatrices)
                iMatrices->SetWorld(DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0]));

            auto iSkinning = dynamic_cast<DirectX::IEffectSkinning*>(pEffect);
            if (iSkinning) {
                if (pMesh->GetBoneInfluences().empty())
                    iSkinning->SetBoneTransforms(pModel->GetBoneMatrices().get(), pModel->GetNumBones());
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

                    iSkinning->SetBoneTransforms(temp.get(), pMesh->GetBoneInfluences().size());
                }
            } else if (iMatrices) {
                DirectX::XMMATRIX boneTransforms = (pMesh->GetBoneIndex() != Bone::INVALID_INDEX && pMesh->GetBoneIndex() < pModel->GetNumBones()) 
                    ? pModel->GetBoneMatrices()[pMesh->GetBoneIndex()] : DirectX::XMMatrixIdentity();

                iMatrices->SetWorld(DirectX::XMMatrixMultiply(boneTransforms, DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0])));
            }

            pEffect->Apply(pCommandList);
            pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
        }
    }
}

void ModelDeviceData::LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory) {
    std::set<MeshDeviceData*> uniqueMeshes;
    for (MeshDeviceData* pMeshData : m_meshes) {
        uniqueMeshes.insert(pMeshData);
    }

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    for (auto it = uniqueMeshes.cbegin(); it != uniqueMeshes.cend(); ++it) {
        MeshDeviceData* pMeshData = *it;

        if (!pMeshData->GetStaticVertexBuffer()) {
            if (!pMeshData->GetVertexBuffer())
                std::runtime_error("Mesh is missing vertex buffer");

            pMeshData->SetVertexBufferSize(static_cast<std::uint32_t>(pMeshData->GetVertexBuffer().Size()));

            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(pMeshData->GetVertexBuffer().Size());
            ThrowIfFailed(pDevice->CreateCommittedResource(
                        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                        IID_GRAPHICS_PPV_ARGS(pMeshData->GetStaticVertexBuffer().GetAddressOf())));

            resourceUploadBatch.Upload(pMeshData->GetStaticVertexBuffer().Get(), pMeshData->GetVertexBuffer());
            resourceUploadBatch.Transition(pMeshData->GetStaticVertexBuffer().Get(),
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

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

            pMeshData->SetIndexBufferSize(static_cast<std::uint32_t>(pMeshData->GetIndexBuffer().Size()));
            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(pMeshData->GetIndexBuffer().Size());

            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                IID_GRAPHICS_PPV_ARGS(pMeshData->GetStaticIndexBuffer().GetAddressOf())));

            resourceUploadBatch.Upload(pMeshData->GetStaticIndexBuffer().Get(), pMeshData->GetIndexBuffer());
            resourceUploadBatch.Transition(pMeshData->GetStaticIndexBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

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

std::vector<DirectX::IEffect*>& ModelDeviceData::GetEffects() noexcept {
    return m_effects;
}

std::vector<MeshDeviceData*>& ModelDeviceData::GetMeshes() noexcept {
    return m_meshes;
}
