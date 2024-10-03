#include "MeshDeviceData.h"

MeshDeviceData::MeshDeviceData(DeviceResources& deviceResources, IMesh& iMesh) 
    : m_deviceResources(deviceResources),
    m_indexBufferSizeInBytes(0),
    m_vertexBufferSizeInBytes(0),
    m_numIndices(iMesh.GetNumIndices()),
    m_numVertices(iMesh.GetNumVertices()),
    m_primitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
    m_indexFormat(DXGI_FORMAT_R16_UINT),
    m_usingStaticBuffers(iMesh.IsUsingStaticBuffers())
{
    m_deviceResources.Attach(this);
    iMesh.Attach(this);

    m_submeshes.reserve(iMesh.GetNumSubmeshes());
    for (std::uint64_t i = 0; i < iMesh.GetNumSubmeshes(); ++i) {
        OnAdd(iMesh.GetSubmeshes()[i]);
    }

    LoadIndexBuffer(&iMesh);
    LoadVertexBuffer(&iMesh);

    if (m_usingStaticBuffers) {
        LoadStaticIndexBuffer(true);
        LoadStaticVertexBuffer(true);
    }
}

MeshDeviceData::~MeshDeviceData() {
    m_deviceResources.Detach(this);
}

void MeshDeviceData::OnDeviceLost() {
    m_indexData.resize(m_indexBufferSizeInBytes);
    memcpy(m_indexData.data(), m_indexBuffer.Memory(), m_indexBufferSizeInBytes);

    m_vertexData.resize(m_vertexBufferSizeInBytes);
    memcpy(m_vertexData.data(), m_vertexBuffer.Memory(), m_vertexBufferSizeInBytes);

    m_indexBuffer.Reset();
    m_vertexBuffer.Reset();
    m_pStaticIndexBuffer.Reset();
    m_pStaticVertexBuffer.Reset();
}

void MeshDeviceData::OnDeviceRestored() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    m_indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(m_indexBufferSizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
    memcpy(m_indexBuffer.Memory(), m_indexData.data(), m_indexBufferSizeInBytes);

    m_vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(m_vertexBufferSizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
    memcpy(m_vertexBuffer.Memory(), m_vertexData.data(), m_vertexBufferSizeInBytes);

    if (m_usingStaticBuffers) {
        LoadStaticIndexBuffer(true);
        LoadStaticVertexBuffer(true);
    }
    m_indexData.clear();
    m_vertexData.clear();
}

void MeshDeviceData::OnUseStaticBuffers(IMesh* pIMesh, bool useStaticBuffers) {
    m_usingStaticBuffers = useStaticBuffers;

    m_deviceResources.WaitForGpu();

    if (!m_indexBuffer && !m_vertexBuffer) {
        LoadIndexBuffer(pIMesh);
        LoadVertexBuffer(pIMesh);
    }

    if (useStaticBuffers && !m_pStaticIndexBuffer && !m_pStaticVertexBuffer) {
        LoadStaticIndexBuffer(true);
        LoadStaticVertexBuffer(true);
    } else {
        m_pStaticIndexBuffer.Reset();
        m_pStaticVertexBuffer.Reset();
    }
}

void MeshDeviceData::OnUpdateBuffers(IMesh* pIMesh) {
    m_deviceResources.WaitForGpu();

    m_numIndices = pIMesh->GetNumIndices();
    m_numVertices = pIMesh->GetNumVertices();

    if (m_indexBuffer && m_vertexBuffer) {
        LoadIndexBuffer(pIMesh);
        LoadVertexBuffer(pIMesh);
    }
}

void MeshDeviceData::OnRebuildFromBuffers(Mesh* pMesh) {
    pMesh->GetIndices().resize(m_numIndices);
    memcpy(pMesh->GetIndices().data(), m_indexBuffer.Memory(), m_indexBufferSizeInBytes);

    pMesh->GetVertices().resize(m_numVertices);
    memcpy(pMesh->GetVertices().data(), m_vertexBuffer.Memory(), m_vertexBufferSizeInBytes);
}

void MeshDeviceData::OnRebuildFromBuffers(SkinnedMesh* pMesh) {
    pMesh->GetIndices().resize(m_numIndices);
    memcpy(pMesh->GetIndices().data(), m_indexBuffer.Memory(), m_indexBufferSizeInBytes);

    pMesh->GetVertices().resize(m_numVertices);
    memcpy(pMesh->GetVertices().data(), m_vertexBuffer.Memory(), m_vertexBufferSizeInBytes);
}

void MeshDeviceData::OnAdd(const std::unique_ptr<Submesh>& pSubmesh) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();
    m_submeshes.push_back(std::make_unique<SubmeshDeviceData>(pDevice, pSubmesh.get()));
}

void MeshDeviceData::OnRemoveSubmesh(std::uint8_t index) {
    m_submeshes.erase(m_submeshes.begin() + index);
}

void MeshDeviceData::LoadIndexBuffer(IMesh* pIMesh) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    std::uint32_t sizeInBytes = static_cast<std::uint32_t>(pIMesh->GetNumIndices()) * sizeof(std::uint16_t);
    if (sizeInBytes > static_cast<std::uint32_t>(D3D12_REQ_RESOURCE_SIZE_IN_MEGABYTES_EXPRESSION_A_TERM * 1024u * 1024u))
        throw std::invalid_argument("IB too large for DirectX 12");

    m_indexBufferSizeInBytes = sizeInBytes;
    m_indexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_INDEX);
    memcpy(m_indexBuffer.Memory(), pIMesh->GetIndices().data(), sizeInBytes);
}

void MeshDeviceData::LoadVertexBuffer(IMesh* pIMesh) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

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

    m_vertexBufferSizeInBytes = sizeInBytes;      
    m_vertexBuffer = DirectX::GraphicsMemory::Get(pDevice).Allocate(sizeInBytes, 16, DirectX::GraphicsMemory::TAG_VERTEX);
    memcpy(m_vertexBuffer.Memory(), vertexData, sizeInBytes);
}

std::future<void> MeshDeviceData::LoadStaticIndexBuffer(bool keepMemory) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    auto const desc = CD3DX12_RESOURCE_DESC::Buffer(m_indexBufferSizeInBytes);

    ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                IID_GRAPHICS_PPV_ARGS(m_pStaticIndexBuffer.GetAddressOf())));

    resourceUploadBatch.Upload(m_pStaticIndexBuffer.Get(), m_indexBuffer);
    resourceUploadBatch.Transition(m_pStaticIndexBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    if (!keepMemory)
        m_indexBuffer.Reset();

    return resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
}

std::future<void> MeshDeviceData::LoadStaticVertexBuffer(bool keepMemory) {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    auto const desc = CD3DX12_RESOURCE_DESC::Buffer(m_vertexBufferSizeInBytes);
    ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties, D3D12_HEAP_FLAG_NONE, &desc, DirectX::c_initialCopyTargetState, nullptr,
                IID_GRAPHICS_PPV_ARGS(m_pStaticVertexBuffer.GetAddressOf())));

    resourceUploadBatch.Upload(m_pStaticVertexBuffer.Get(), m_vertexBuffer);
    resourceUploadBatch.Transition(m_pStaticVertexBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    if (!keepMemory)
        m_vertexBuffer.Reset();

    return resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
}

void MeshDeviceData::PrepareForDraw() const {
    ID3D12GraphicsCommandList* pCommandList = m_deviceResources.GetCommandList();

    if (!m_indexBufferSizeInBytes || !m_vertexBufferSizeInBytes) 
        throw std::runtime_error("Submesh is missing values for vertex and/or index buffer size: vertexBufferSize=" + std::to_string(m_vertexBufferSizeInBytes) + "; indexBufferSize=" + std::to_string(m_indexBufferSizeInBytes));
    if (!m_pStaticIndexBuffer && !m_indexBuffer)
        throw std::runtime_error("Submesh is missing index buffer");
    if (!m_pStaticVertexBuffer && !m_vertexBuffer)
        throw std::runtime_error("Submesh is missing vertex buffer");

    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.BufferLocation = m_pStaticVertexBuffer ? m_pStaticVertexBuffer->GetGPUVirtualAddress() : m_vertexBuffer.GpuAddress();
    vbv.StrideInBytes = m_vertexStride;
    vbv.SizeInBytes = m_vertexBufferSizeInBytes;
    pCommandList->IASetVertexBuffers(0, 1, &vbv);

    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.BufferLocation = m_pStaticIndexBuffer ? m_pStaticIndexBuffer->GetGPUVirtualAddress() : m_indexBuffer.GpuAddress();
    ibv.SizeInBytes = m_indexBufferSizeInBytes;
    ibv.Format = m_indexFormat;
    pCommandList->IASetIndexBuffer(&ibv);

    pCommandList->IASetPrimitiveTopology(m_primitiveType);
}

std::vector<std::unique_ptr<SubmeshDeviceData>>& MeshDeviceData::GetSubmeshes() noexcept {
    return m_submeshes;
}

std::uint32_t MeshDeviceData::GetIndexBufferSize() const noexcept {
    return m_indexBufferSizeInBytes;
}

std::uint32_t MeshDeviceData::GetVertexBufferSize() const noexcept {
    return m_vertexBufferSizeInBytes;
}

DirectX::SharedGraphicsResource& MeshDeviceData::GetIndexBuffer() noexcept {
    return m_indexBuffer;
}

DirectX::SharedGraphicsResource& MeshDeviceData::GetVertexBuffer() noexcept {
    return m_vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& MeshDeviceData::GetStaticIndexBuffer() noexcept {
    return m_pStaticIndexBuffer;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& MeshDeviceData::GetStaticVertexBuffer() noexcept {
    return m_pStaticVertexBuffer;
}

void MeshDeviceData::SetIndexBufferSize(std::uint32_t size) noexcept {
    m_indexBufferSizeInBytes = size;
}

void MeshDeviceData::SetVertexBufferSize(std::uint32_t size) noexcept {
    m_vertexBufferSizeInBytes = size;
}

void MeshDeviceData::SetStaticIndexBuffer(ID3D12Resource* pIndexBuffer) {
    m_pStaticIndexBuffer = pIndexBuffer;
}

void MeshDeviceData::SetStaticVertexBuffer(ID3D12Resource* pVertexBuffer) {
    m_pStaticVertexBuffer = pVertexBuffer;
}

std::uint32_t MeshDeviceData::GetNumSubmeshes() const noexcept {
    return m_submeshes.size();
}

