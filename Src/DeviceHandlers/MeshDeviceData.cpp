#include "MeshDeviceData.h"

MeshDeviceData::MeshDeviceData(ID3D12Device* pDevice, IMesh* pIMesh) :
    m_indexBufferSize(0),
    m_vertexBufferSize(0),
    m_primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
    m_indexFormat(DXGI_FORMAT_R16_UINT)
{
    m_submeshes.reserve(pIMesh->GetNumSubmeshes());
    for (std::uint64_t i = 0; i < pIMesh->GetNumSubmeshes(); ++i) {
        m_submeshes.push_back(std::make_unique<SubmeshDeviceData>(pDevice, pIMesh->GetSubmeshes()[i].get()));
    }

    LoadVertexBuffer(pDevice, pIMesh);
    LoadIndexBuffer(pDevice, pIMesh);
}

void MeshDeviceData::LoadVertexBuffer(ID3D12Device* pDevice, IMesh* pIMesh) {
    void* vertexData;

    if (auto pMesh = dynamic_cast<Mesh*>(pIMesh)) {
        m_vertexStride = sizeof(VertexPositionNormalTexture);
        vertexData = pMesh->GetVertices().data();
    } else if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(pIMesh)) {
        m_vertexStride = sizeof(VertexPositionNormalTextureSkinning);
        vertexData = pSkinnedMesh->GetVertices().data();
    } else {
        throw std::runtime_error("Mesh failed to downcast.");
    }

    if (pIMesh->GetNumVertices() >= USHRT_MAX)
        throw std::invalid_argument("Too many vertices for a 16-bit index buffer");
    if (pIMesh->GetNumIndices() > UINT32_MAX)
        throw std::invalid_argument("Too many indices");

    std::uint32_t sizeInBytes = static_cast<std::uint32_t>(pIMesh->GetNumVertices()) * m_vertexStride;
    if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
        throw std::invalid_argument("VB too large for DirectX 12");

    m_vertexBufferSize = sizeInBytes;      
    m_vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
    memcpy(m_vertexBuffer.Memory(), vertexData, sizeInBytes);
}

void MeshDeviceData::LoadIndexBuffer(ID3D12Device* pDevice, IMesh* pIMesh) {
    std::uint32_t sizeInBytes = static_cast<std::uint32_t>(pIMesh->GetNumIndices()) * sizeof(std::uint16_t);
    if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
        throw std::invalid_argument("IB too large for DirectX 12");

    m_indexBufferSize = sizeInBytes;
    m_indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
    memcpy(m_indexBuffer.Memory(), pIMesh->GetIndices().data(), sizeInBytes);
}

void MeshDeviceData::OnDeviceLost() noexcept {
    m_indexBuffer.Reset();
    m_vertexBuffer.Reset();
    m_staticIndexBuffer.Reset();
    m_staticVertexBuffer.Reset();
}

void MeshDeviceData::PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const {
    if (!m_indexBufferSize || !m_vertexBufferSize) 
        throw std::runtime_error("Submesh is missing values for vertex and/or index buffer size: vertexBufferSize=" + std::to_string(m_vertexBufferSize) + "; indexBufferSize=" + std::to_string(m_indexBufferSize));
    if (!m_staticIndexBuffer && !m_indexBuffer)
        throw std::runtime_error("Submesh is missing index buffer");
    if (!m_staticVertexBuffer && !m_vertexBuffer)
        throw std::runtime_error("Submesh is missing vertex buffer");

    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = m_staticVertexBuffer ? m_staticVertexBuffer->GetGPUVirtualAddress() : m_vertexBuffer.GpuAddress();
    vbv.StrideInBytes = m_vertexStride;
    vbv.SizeInBytes = m_vertexBufferSize;
    pCommandList->IASetVertexBuffers(0, 1, &vbv);

    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = m_staticIndexBuffer ? m_staticIndexBuffer->GetGPUVirtualAddress() : m_indexBuffer.GpuAddress();
    ibv.SizeInBytes = m_indexBufferSize;
    ibv.Format = m_indexFormat;
    pCommandList->IASetIndexBuffer(&ibv);

    pCommandList->IASetPrimitiveTopology(m_primitiveType);
}

std::vector<std::unique_ptr<SubmeshDeviceData>>& MeshDeviceData::GetSubmeshes() noexcept {
    return m_submeshes;
}

std::uint32_t MeshDeviceData::GetIndexBufferSize() const noexcept {
    return m_indexBufferSize;
}

std::uint32_t MeshDeviceData::GetVertexBufferSize() const noexcept {
    return m_vertexBufferSize;
}

DirectX::SharedGraphicsResource& MeshDeviceData::GetIndexBuffer() noexcept {
    return m_indexBuffer;
}

DirectX::SharedGraphicsResource& MeshDeviceData::GetVertexBuffer() noexcept {
    return m_vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& MeshDeviceData::GetStaticIndexBuffer() noexcept {
    return m_staticIndexBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& MeshDeviceData::GetStaticVertexBuffer() noexcept {
    return m_staticVertexBuffer;
}

void MeshDeviceData::SetIndexBufferSize(std::uint32_t size) noexcept {
    m_indexBufferSize = size;
}

void MeshDeviceData::SetVertexBufferSize(std::uint32_t size) noexcept {
    m_vertexBufferSize = size;
}

void MeshDeviceData::SetStaticIndexBuffer(ID3D12Resource* pIndexBuffer) {
    m_staticIndexBuffer = pIndexBuffer;
}

void MeshDeviceData::SetStaticVertexBuffer(ID3D12Resource* pVertexBuffer) {
    m_staticVertexBuffer = pVertexBuffer;
}

