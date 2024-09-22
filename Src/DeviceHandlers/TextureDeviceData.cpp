#include "TextureDeviceData.h"

TextureDeviceData::TextureDeviceData(
        DeviceResources& deviceResources, 
        DirectX::DescriptorHeap& descriptorHeap,
        std::uint32_t heapIndex, 
        std::wstring filePath) 
    : m_filePath(filePath),
    m_deviceResources(deviceResources),
    m_descriptorHeap(descriptorHeap),
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

    if (DirectX::CreateWICTextureFromFile(pDevice, resourceUploadBatch, m_filePath.c_str(), m_pTexture.GetAddressOf()) != S_OK)
        ThrowIfFailed(DirectX::CreateDDSTextureFromFile(pDevice, resourceUploadBatch, m_filePath.c_str(), m_pTexture.GetAddressOf()));

    DirectX::CreateShaderResourceView(
            pDevice, 
            m_pTexture.Get(),
            m_descriptorHeap.GetCpuHandle(m_desciptorHeapIndex));

    return resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
}

std::uint8_t TextureDeviceData::GetHeapIndex() const noexcept {
    return m_desciptorHeapIndex;
}

Microsoft::WRL::ComPtr<ID3D12Resource>& TextureDeviceData::GetTexture() noexcept {
    return m_pTexture;
}

