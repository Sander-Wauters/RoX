#include "TextureDeviceData.h"

TextureDeviceData::TextureDeviceData(
        DeviceResources& deviceResources, 
        DirectX::DescriptorHeap* pDescriptorHeap,
        std::uint32_t heapIndex, 
        std::wstring filePath) 
    : m_filePath(filePath),
    m_deviceResources(deviceResources),
    m_pDescriptorHeap(pDescriptorHeap),
    m_desciptorHeapIndex(heapIndex)
{
    m_deviceResources.Attach(this);
    CreateTextureResource().wait();
}

TextureDeviceData::~TextureDeviceData() {
    m_deviceResources.Detach(this);
}

void TextureDeviceData::OnDeviceLost() {
    m_pTexture.Reset();
}

void TextureDeviceData::OnDeviceRestored() {
    CreateTextureResource().wait();
}

std::future<void> TextureDeviceData::CreateTextureResource() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    if (FAILED(DirectX::CreateWICTextureFromFile(pDevice, resourceUploadBatch, m_filePath.c_str(), m_pTexture.GetAddressOf())))
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(pDevice, resourceUploadBatch, m_filePath.c_str(), m_pTexture.GetAddressOf()));

    DirectX::CreateShaderResourceView(
            pDevice, 
            m_pTexture.Get(),
            m_pDescriptorHeap->GetCpuHandle(m_desciptorHeapIndex));

    return resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
}

std::uint8_t TextureDeviceData::GetHeapIndex() const noexcept {
    return m_desciptorHeapIndex;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& TextureDeviceData::GetTexture() noexcept {
    return m_pTexture;
}

void TextureDeviceData::SetDescriptorHeap(DirectX::DescriptorHeap* pDescriptorHeap) noexcept {
    m_pDescriptorHeap = pDescriptorHeap;
}

