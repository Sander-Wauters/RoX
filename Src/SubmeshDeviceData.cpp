#include "SubmeshDeviceData.h"

#include <DirectXHelpers.h>

#include "Exceptions/ThrowIfFailed.h"

SubmeshDeviceData::SubmeshDeviceData(ID3D12Device* pDevice, Submesh* pSubmesh) :
    m_vertexStride(sizeof(DirectX::VertexPositionNormalTexture)),
    m_indexBufferSize(0),
    m_vertexBufferSize(0),
    m_primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
    m_indexFormat(DXGI_FORMAT_R16_UINT)
{
    if (pSubmesh->GetNumVertices() >= USHRT_MAX)
        throw std::invalid_argument("Too many vertices for a 16-bit index buffer");
    if (pSubmesh->GetNumIndices() > UINT32_MAX)
        throw std::invalid_argument("Too many indices");

    std::uint32_t sizeInBytes = static_cast<std::uint32_t>(pSubmesh->GetNumVertices()) * sizeof(DirectX::VertexPositionNormalTexture);
    if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
        throw std::invalid_argument("VB too large for DirectX 12");

    m_vertexBufferSize = sizeInBytes;      
    m_vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
    memcpy(m_vertexBuffer.Memory(), pSubmesh->GetVertices()->data(), sizeInBytes);

    sizeInBytes = static_cast<std::uint32_t>(pSubmesh->GetNumIndices()) * sizeof(std::uint16_t);
    if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
        throw std::invalid_argument("IB too large for DirectX 12");

    m_indexBufferSize = sizeInBytes;
    m_indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
    memcpy(m_indexBuffer.Memory(), pSubmesh->GetIndices()->data(), sizeInBytes);
}

void SubmeshDeviceData::PrepareForDraw(ID3D12GraphicsCommandList* pCommandList) const {
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

void SubmeshDeviceData::Draw(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    PrepareForDraw(pCommandList);
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), 1, pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

void SubmeshDeviceData::DrawInstanced(ID3D12GraphicsCommandList* pCommandList, Submesh* pSubmesh) const {
    PrepareForDraw(pCommandList);
    pCommandList->DrawIndexedInstanced(pSubmesh->GetIndexCount(), pSubmesh->GetNumVisibleInstances(), pSubmesh->GetStartIndex(), pSubmesh->GetVertexOffset(), 0);
}

std::uint32_t SubmeshDeviceData::GetIndexBufferSize() const noexcept {
    return m_indexBufferSize;
}

std::uint32_t SubmeshDeviceData::GetVertexBufferSize() const noexcept {
    return m_vertexBufferSize;
}

DirectX::SharedGraphicsResource& SubmeshDeviceData::GetIndexBuffer() noexcept {
    return m_indexBuffer;
}

DirectX::SharedGraphicsResource& SubmeshDeviceData::GetVertexBuffer() noexcept {
    return m_vertexBuffer;
}

ID3D12Resource* SubmeshDeviceData::GetStaticIndexBuffer() noexcept {
    return m_staticIndexBuffer.Get();
}

ID3D12Resource* SubmeshDeviceData::GetStaticVertexBuffer() noexcept {
    return m_staticVertexBuffer.Get();
}

ID3D12Resource** SubmeshDeviceData::GetAddressOfStaticIndexBuffer() noexcept {
    return m_staticIndexBuffer.GetAddressOf();
}

ID3D12Resource** SubmeshDeviceData::GetAddressOfStaticVertexBuffer() noexcept {
    return m_staticVertexBuffer.GetAddressOf();
}

void SubmeshDeviceData::SetIndexBufferSize(std::uint32_t size) noexcept {
    m_indexBufferSize = size;
}

void SubmeshDeviceData::SetVertexBufferSize(std::uint32_t size) noexcept {
    m_vertexBufferSize = size;
}

void SubmeshDeviceData::SetStaticIndexBuffer(ID3D12Resource* pIndexBuffer) {
    m_staticIndexBuffer = pIndexBuffer;
}

void SubmeshDeviceData::SetStaticVertexBuffer(ID3D12Resource* pVertexBuffer) {
    m_staticVertexBuffer = pVertexBuffer;
}

