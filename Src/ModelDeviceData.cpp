#include "ModelDeviceData.h"

#include <DirectXHelpers.h>

#include <set>

#include "Exceptions/ThrowIfFailed.h"

ModelDeviceData::ModelDeviceData(ID3D12Device* pDevice, Model* pModel, 
        std::unordered_map<std::shared_ptr<Mesh>, std::unique_ptr<MeshDeviceData>>& sharedMeshes) 
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
        Mesh* pMesh = pModel->GetMeshes()[meshIndex].get();

        for (std::uint64_t submeshIndex = 0; submeshIndex < pMesh->GetNumSubmeshes(); ++submeshIndex) {
            Submesh* pSubmesh = pMesh->GetSubmeshes()[submeshIndex].get();
            DirectX::IEffect* pEffect = m_effects[pSubmesh->GetMaterialIndex()]->get();

            auto iMatrices = dynamic_cast<DirectX::IEffectMatrices*>(pEffect);
            if (iMatrices)
                iMatrices->SetWorld(DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0]));

            auto iSkinning = dynamic_cast<DirectX::IEffectSkinning*>(pEffect);
            if (iSkinning) {
                if (pMesh->GetBoneInfluences().empty())
                    iSkinning->SetBoneTransforms(pModel->GetBoneMatrices(), pModel->GetNumBones());
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
            m_meshes[meshIndex]->GetSubmeshes()[submeshIndex]->PrepareForDraw(pCommandList);
            pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
        }
    }

}

void ModelDeviceData::LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory) {
    std::set<SubmeshDeviceData*> uniqueSubmeshes;
    for (const auto& mesh : m_meshes) {
        for (const auto& submesh : mesh->GetSubmeshes()) {
            uniqueSubmeshes.insert(submesh.get());
        }
    }

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    for (auto it = uniqueSubmeshes.cbegin(); it != uniqueSubmeshes.cend(); ++it) {
        SubmeshDeviceData* pSubmesh = *it;

        if (!pSubmesh->GetStaticVertexBuffer()) {
            if (!pSubmesh->GetVertexBuffer())
                std::runtime_error("Submesh is missing vertex buffer");

            pSubmesh->SetVertexBufferSize(static_cast<std::uint32_t>(pSubmesh->GetVertexBuffer().Size()));

            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(pSubmesh->GetVertexBuffer().Size());
            ThrowIfFailed(pDevice->CreateCommittedResource(
                        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                        IID_GRAPHICS_PPV_ARGS(pSubmesh->GetStaticVertexBuffer().GetAddressOf())));

            resourceUploadBatch.Upload(pSubmesh->GetStaticVertexBuffer().Get(), pSubmesh->GetVertexBuffer());
            resourceUploadBatch.Transition(pSubmesh->GetStaticVertexBuffer().Get(),
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

            for (auto sit = std::next(it); sit != uniqueSubmeshes.cend(); ++sit) {
                SubmeshDeviceData* pSharedSubMesh = *sit;
                assert(pSharedSubMesh != pSubmesh);

                if (pSharedSubMesh->GetStaticVertexBuffer())
                    continue;

                if (pSharedSubMesh->GetVertexBuffer() == pSubmesh->GetVertexBuffer()) {
                    pSharedSubMesh->SetVertexBufferSize(pSubmesh->GetVertexBufferSize());
                    pSharedSubMesh->SetStaticVertexBuffer(pSubmesh->GetStaticVertexBuffer().Get());

                    if (!keepMemory)
                        pSharedSubMesh->GetVertexBuffer().Reset();
                }
            }

            if (!keepMemory)
                pSubmesh->GetVertexBuffer().Reset();
        }

        if (!pSubmesh->GetStaticIndexBuffer()) {
            if (!pSubmesh->GetIndexBuffer())
                std::runtime_error("Submesh is missing index buffer");

            pSubmesh->SetIndexBufferSize(static_cast<std::uint32_t>(pSubmesh->GetIndexBuffer().Size()));
            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(pSubmesh->GetIndexBuffer().Size());

            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                IID_GRAPHICS_PPV_ARGS(pSubmesh->GetStaticIndexBuffer().GetAddressOf())));

            resourceUploadBatch.Upload(pSubmesh->GetStaticIndexBuffer().Get(), pSubmesh->GetIndexBuffer());
            resourceUploadBatch.Transition(pSubmesh->GetStaticIndexBuffer().Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

            // Scan for any other part with the same index buffer for sharing
            for (auto sit = std::next(it); sit != uniqueSubmeshes.cend(); ++sit) {
                SubmeshDeviceData* pSharedSubMesh = *sit;
                assert(pSharedSubMesh != pSubmesh);

                if (pSharedSubMesh->GetStaticIndexBuffer())
                    continue;

                if (pSharedSubMesh->GetIndexBuffer() == pSubmesh->GetIndexBuffer()) {
                    pSharedSubMesh->SetIndexBufferSize(pSubmesh->GetIndexBufferSize());
                    pSharedSubMesh->SetStaticIndexBuffer(pSubmesh->GetStaticIndexBuffer().Get());

                    if (!keepMemory)
                        pSharedSubMesh->GetIndexBuffer().Reset();
                }
            }

            if (!keepMemory)
                pSubmesh->GetIndexBuffer().Reset();
        }
    }
}

std::vector<std::unique_ptr<DirectX::IEffect>*>& ModelDeviceData::GetEffects() noexcept {
    return m_effects;
}

std::vector<MeshDeviceData*>& ModelDeviceData::GetMeshes() noexcept {
    return m_meshes;
}