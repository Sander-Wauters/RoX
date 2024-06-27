#include "DeviceData.h"

#include <set>

#include <DirectXHelpers.h>

#include "Exceptions/ThrowIfFailed.h"

// ---------------------------------------------------------------- //
//                          TextureDeviceData
// ---------------------------------------------------------------- //

TextureDeviceData::TextureDeviceData(std::uint32_t heapIndex) noexcept : descriptorHeapIndex(heapIndex) {}

// ---------------------------------------------------------------- //
//                          TextDeviceData
// ---------------------------------------------------------------- //

TextDeviceData::TextDeviceData(std::uint32_t heapIndex) noexcept : descriptorHeapIndex(heapIndex) {}

// ---------------------------------------------------------------- //
//                          SubmeshDeviceData
// ---------------------------------------------------------------- //

SubmeshDeviceData::SubmeshDeviceData::SubmeshDeviceData() 
    noexcept : vertexStride(sizeof(DirectX::VertexPositionNormalTexture)),
    indexBufferSize(0),
    vertexBufferSize(0),
    primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
    indexFormat(DXGI_FORMAT_R16_UINT)
{}

void SubmeshDeviceData::PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const {
    if (!indexBufferSize || !vertexBufferSize) 
        throw std::runtime_error("Submesh is missing values for vertex and/or index buffer size: vertexBufferSize=" + std::to_string(vertexBufferSize) + "; indexBufferSize=" + std::to_string(indexBufferSize));
    if (!staticIndexBuffer && !indexBuffer)
        throw std::runtime_error("Submesh is missing index buffer");
    if (!staticVertexBuffer && !vertexBuffer)
        throw std::runtime_error("Submesh is missing vertex buffer");

    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = staticVertexBuffer ? staticVertexBuffer->GetGPUVirtualAddress() : vertexBuffer.GpuAddress();
    vbv.StrideInBytes = vertexStride;
    vbv.SizeInBytes = vertexBufferSize;
    pCommandList->IASetVertexBuffers(0, 1, &vbv);

    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = staticIndexBuffer ? staticIndexBuffer->GetGPUVirtualAddress() : indexBuffer.GpuAddress();
    ibv.SizeInBytes = indexBufferSize;
    ibv.Format = indexFormat;
    pCommandList->IASetIndexBuffer(&ibv);

    pCommandList->IASetPrimitiveTopology(primitiveType);
}

void SubmeshDeviceData::Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    assert(pSubmesh != nullptr);
    
    PrepareForDraw(pCommandList);
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

void SubmeshDeviceData::Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh, DirectX::IEffect* pEffect) const {
    assert(pEffect != nullptr);
    
    if (auto pEffectMatrices = dynamic_cast<DirectX::NormalMapEffect*>(pEffect))
        pEffectMatrices->SetWorld(DirectX::XMLoadFloat3x4(&pSubmesh->GetInstances()[0]));
    
    pEffect->Apply(pCommandList);
    Draw(pCommandList, pSubmesh);
}

void SubmeshDeviceData::DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    assert(pSubmesh != nullptr);

    PrepareForDraw(pCommandList);
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), pSubmesh->GetNumVisibleInstances(), pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

void SubmeshDeviceData::DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh, DirectX::IEffect* pEffect) const {
    assert(pEffect != nullptr);

    pEffect->Apply(pCommandList);
    DrawInstanced(pCommandList, pSubmesh);
}

DirectX::IEffect* SubmeshDeviceData::GetEffect(ModelDeviceData* pModel, Submesh* pSubmesh) const {
    return pModel->effects[pSubmesh->GetMaterialIndex()]->get();
}

// ---------------------------------------------------------------- //
//                          MeshDeviceData
// ---------------------------------------------------------------- //



// ---------------------------------------------------------------- //
//                          ModelDeviceData
// ---------------------------------------------------------------- //

void ModelDeviceData::LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory) {
    std::set<SubmeshDeviceData*> uniqueSubmeshes;
    for (const auto& mesh : meshes) {
        for (const auto& submesh : mesh->submeshes) {
            uniqueSubmeshes.insert(submesh.get());
        }
    }

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);

    for (auto it = uniqueSubmeshes.cbegin(); it != uniqueSubmeshes.cend(); ++it) {
        auto submesh = *it;

        if (!submesh->staticVertexBuffer) {
            if (!submesh->vertexBuffer)
                std::runtime_error("Submesh is missing vertex buffer");

            submesh->vertexBufferSize = static_cast<std::uint32_t>(submesh->vertexBuffer.Size());

            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(submesh->vertexBuffer.Size());
            ThrowIfFailed(pDevice->CreateCommittedResource(
                        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                        IID_GRAPHICS_PPV_ARGS(submesh->staticVertexBuffer.GetAddressOf())));

            resourceUploadBatch.Upload(submesh->staticVertexBuffer.Get(), submesh->vertexBuffer);
            resourceUploadBatch.Transition(submesh->staticVertexBuffer.Get(), 
                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

            for (auto sit = std::next(it); sit != uniqueSubmeshes.cend(); ++sit) {
                SubmeshDeviceData* sharedSubMesh = *sit;

                if (sharedSubMesh->staticVertexBuffer)
                    continue;

                if (sharedSubMesh->vertexBuffer == submesh->vertexBuffer) {
                    sharedSubMesh->vertexBufferSize = submesh->vertexBufferSize;
                    sharedSubMesh->staticVertexBuffer = submesh->staticVertexBuffer;

                    if (!keepMemory)
                        sharedSubMesh->vertexBuffer.Reset();
                }
            }

            if (!keepMemory)
                submesh->vertexBuffer.Reset();
        }

        if (!submesh->staticIndexBuffer) {
            if (!submesh->indexBuffer)
                std::runtime_error("Submesh is missing index buffer");

            submesh->indexBufferSize = static_cast<std::uint32_t>(submesh->indexBuffer.Size());
            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(submesh->indexBuffer.Size());

            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                IID_GRAPHICS_PPV_ARGS(submesh->staticIndexBuffer.GetAddressOf())
            ));

            resourceUploadBatch.Upload(submesh->staticIndexBuffer.Get(), submesh->indexBuffer);
            resourceUploadBatch.Transition(submesh->staticIndexBuffer.Get(),
                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

            // Scan for any other part with the same index buffer for sharing
            for (auto sit = std::next(it); sit != uniqueSubmeshes.cend(); ++sit) {
                auto sharedSubMesh = *sit;
                assert(sharedSubMesh != submesh);

                if (sharedSubMesh->staticIndexBuffer)
                    continue;

                if (sharedSubMesh->indexBuffer == submesh->indexBuffer) {
                    sharedSubMesh->indexBufferSize = submesh->indexBufferSize;
                    sharedSubMesh->staticIndexBuffer = submesh->staticIndexBuffer;

                    if (!keepMemory)
                        sharedSubMesh->indexBuffer.Reset();
                }
            }

            if (!keepMemory)
                submesh->indexBuffer.Reset();
        }
    }

}

// ---------------------------------------------------------------- //
//                            DEPRICATED
// ---------------------------------------------------------------- //

//DeviceData::Material::Material() noexcept : pEffect(nullptr) {}
//
//DeviceData::Texture::Texture(std::uint32_t heapIndex) 
//    noexcept : DescriptorHeapIndex(heapIndex),
//    pTexture(nullptr)
//{}
//
//DeviceData::Text::Text(std::uint32_t heapIndex)
//    noexcept : DescriptorHeapIndex(heapIndex),
//    pSpriteFont(nullptr)
//{}
//
//DeviceData::SubMesh::SubMesh(
//        ID3D12Device* pDevice,
//        const std::vector<DirectX::VertexPositionNormalTexture>& vertices,
//        const std::vector<std::uint16_t>& indices) : 
//    VertexStride(0),
//    IndexBufferSize(0),
//    VertexBufferSize(0),
//    PrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
//    IndexFormat(DXGI_FORMAT_R16_UINT)
//{
//    if (vertices.size() >= USHRT_MAX)
//        throw std::invalid_argument("Too many vertices for a 16-bit index buffer");
//    if (indices.size() > UINT32_MAX)
//        throw std::invalid_argument("Too many indices");
//
//    std::uint64_t sizeInBytes = std::uint64_t(vertices.size()) * sizeof(vertices[0]);
//    if (sizeInBytes > std::uint64_t(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
//        throw std::invalid_argument("VB too large for DirectX 12");
//
//    VertexBufferSize = static_cast<std::uint32_t>(sizeInBytes);      
//    VertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
//    memcpy(VertexBuffer.Memory(), vertices.data(), sizeInBytes);
//
//    sizeInBytes = std::uint64_t(indices.size()) * sizeof(indices[0]);
//    if (sizeInBytes > std::uint64_t(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
//        throw std::invalid_argument("IB too large for DirectX 12");
//    
//    IndexBufferSize = static_cast<std::uint32_t>(sizeInBytes);
//    IndexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
//    memcpy(IndexBuffer.Memory(), indices.data(), sizeInBytes);
//}
//
//void DeviceData::SubMesh::DrawInstanced(
//        ID3D12GraphicsCommandList* pCommandList, 
//        std::uint32_t indexCount, 
//        std::uint32_t instanceCount, 
//        std::uint32_t startIndex,
//        std::uint32_t vertexOffset,
//        std::uint32_t startInstance) const {
//    if (!IndexBufferSize || !VertexBufferSize)
//        throw std::runtime_error("SubMesh: missing values for vertex and/or index buffer size; IndexBufferSize: " + std::to_string(IndexBufferSize) + "; VertexBufferSize:" + std::to_string(VertexBufferSize));
//    if (!StaticIndexBuffer && !IndexBuffer)
//        throw std::runtime_error("SubMesh: missing index buffer");
//    if (!StaticVertexBuffer && !VertexBuffer)
//        throw std::runtime_error("SubMesh: missing vertex buffer");
//
//    D3D12_VERTEX_BUFFER_VIEW vbv;
//    vbv.BufferLocation = StaticVertexBuffer ? StaticVertexBuffer->GetGPUVirtualAddress() : VertexBuffer.GpuAddress();
//    vbv.StrideInBytes = VertexStride;
//    vbv.SizeInBytes = VertexBufferSize;
//    pCommandList->IASetVertexBuffers(0, 1, &vbv);
//
//    D3D12_INDEX_BUFFER_VIEW ibv;
//    ibv.BufferLocation = StaticIndexBuffer ? StaticIndexBuffer->GetGPUVirtualAddress() : IndexBuffer.GpuAddress();
//    ibv.SizeInBytes = IndexBufferSize;
//    ibv.Format = IndexFormat;
//    pCommandList->IASetIndexBuffer(&ibv);
//    pCommandList->IASetPrimitiveTopology(PrimitiveType);
//    pCommandList->DrawIndexedInstanced(indexCount, instanceCount, startIndex, vertexOffset, startInstance);
//}
//
//DeviceData::Mesh::Mesh() : pGeometricPrimitive(nullptr) {}
//
//void DeviceData::Mesh::Initialize(
//        ID3D12Device* pDevice,
//        std::uint16_t numSubMeshes,
//        const std::vector<DirectX::VertexPositionNormalTexture>& vertices,
//        const std::vector<std::uint16_t>& indices
//        ) 
//{
//    SubMeshes.resize(numSubMeshes);
//    for (std::uint16_t i = 0; i < numSubMeshes; ++i) {
//        std::unique_ptr<SubMesh> sub = std::make_unique<SubMesh>(pDevice, vertices, indices);
//        sub->pMaterialData = pMaterialData;
//        SubMeshes[i] = std::move(sub);
//    }
//}
//
//// Load VB/IB resources for static geometry.
//void DeviceData::Mesh::LoadStaticBuffers(ID3D12Device* pDevice, DirectX::ResourceUploadBatch& resourceUploadBatch, bool keepMemory) {
//    std::set<DeviceData::SubMesh*> uniqueSubMeshes;
//    for (const std::unique_ptr<DeviceData::SubMesh>& submesh : SubMeshes) {
//        uniqueSubMeshes.insert(submesh.get());
//    }
//
//    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
//
//    for (auto it = uniqueSubMeshes.cbegin(); it != uniqueSubMeshes.cend(); ++it) {
//        auto submesh = *it;
//
//        if (!submesh->StaticVertexBuffer) {
//            if (!submesh->VertexBuffer)
//                std::runtime_error("SubMesh is missing vertex buffer");
//
//            submesh->VertexBufferSize = static_cast<std::uint32_t>(submesh->VertexBuffer.Size());
//
//            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(submesh->VertexBuffer.Size());
//            ThrowIfFailed(pDevice->CreateCommittedResource(
//                        &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
//                        IID_GRAPHICS_PPV_ARGS(submesh->StaticVertexBuffer.GetAddressOf())));
//
//            resourceUploadBatch.Upload(submesh->StaticVertexBuffer.Get(), submesh->VertexBuffer);
//            resourceUploadBatch.Transition(submesh->StaticVertexBuffer.Get(), 
//                    D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
//
//            for (auto sit = std::next(it); sit != uniqueSubMeshes.cend(); ++sit) {
//                auto sharedSubMesh = *sit;
//
//                if (sharedSubMesh->StaticVertexBuffer)
//                    continue;
//
//                if (sharedSubMesh->VertexBuffer == submesh->VertexBuffer) {
//                    sharedSubMesh->VertexBufferSize = submesh->VertexBufferSize;
//                    sharedSubMesh->StaticVertexBuffer = submesh->StaticVertexBuffer;
//
//                    if (!keepMemory)
//                        sharedSubMesh->VertexBuffer.Reset();
//                }
//            }
//
//            if (!keepMemory)
//                submesh->VertexBuffer.Reset();
//        }
//
//        if (!submesh->StaticIndexBuffer) {
//            if (!submesh->IndexBuffer)
//                std::runtime_error("SubMesh is missing index buffer");
//
//            submesh->IndexBufferSize = static_cast<std::uint32_t>(submesh->IndexBuffer.Size());
//            auto const desc = CD3DX12_RESOURCE_DESC::Buffer(submesh->IndexBuffer.Size());
//
//            ThrowIfFailed(pDevice->CreateCommittedResource(
//                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
//                IID_GRAPHICS_PPV_ARGS(submesh->StaticIndexBuffer.GetAddressOf())
//            ));
//
//            resourceUploadBatch.Upload(submesh->StaticIndexBuffer.Get(), submesh->IndexBuffer);
//            resourceUploadBatch.Transition(submesh->StaticIndexBuffer.Get(),
//                D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
//
//            // Scan for any other part with the same index buffer for sharing
//            for (auto sit = std::next(it); sit != uniqueSubMeshes.cend(); ++sit) {
//                auto sharedSubMesh = *sit;
//                assert(sharedSubMesh != submesh);
//
//                if (sharedSubMesh->StaticIndexBuffer)
//                    continue;
//
//                if (sharedSubMesh->IndexBuffer == submesh->IndexBuffer) {
//                    sharedSubMesh->IndexBufferSize = submesh->IndexBufferSize;
//                    sharedSubMesh->StaticIndexBuffer = submesh->StaticIndexBuffer;
//
//                    if (!keepMemory)
//                        sharedSubMesh->IndexBuffer.Reset();
//                }
//            }
//
//            if (!keepMemory)
//                submesh->IndexBuffer.Reset();
//        }
//    }
//}
//
