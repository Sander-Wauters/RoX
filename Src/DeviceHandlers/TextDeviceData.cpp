#include "TextDeviceData.h"

TextDeviceData::TextDeviceData(
        DeviceResources& deviceResources, 
        DirectX::DescriptorHeap* pDescriptorHeap,
        std::uint32_t heapIndex,
        std::wstring filePath) 
    : m_filePath(filePath),
    m_deviceResources(deviceResources),
    m_pDescriptorHeap(pDescriptorHeap),
    m_descriptorHeapIndex(heapIndex)
{
    deviceResources.Attach(this);
    CreateSpriteFont().wait();
}

TextDeviceData::~TextDeviceData() {
    m_deviceResources.Detach(this);
}

void TextDeviceData::OnDeviceLost() {
    m_pSpriteFont.reset();
}

void TextDeviceData::OnDeviceRestored() {
    CreateSpriteFont().wait();
}

std::future<void> TextDeviceData::CreateSpriteFont() {
    ID3D12Device* pDevice = m_deviceResources.GetDevice();

    DirectX::ResourceUploadBatch resourceUploadBatch(pDevice);
    resourceUploadBatch.Begin();

    if (!(m_filePath.substr(m_filePath.find_last_of(L".") + 1) == L"spritefont")) 
        throw std::invalid_argument("Unsupported file extension detected.");

    m_pSpriteFont = std::make_unique<DirectX::SpriteFont>(
            pDevice,
            resourceUploadBatch,
            m_filePath.c_str(),
            m_pDescriptorHeap->GetCpuHandle(m_descriptorHeapIndex),
            m_pDescriptorHeap->GetGpuHandle(m_descriptorHeapIndex));

    return resourceUploadBatch.End(m_deviceResources.GetCommandQueue());
}

std::uint8_t TextDeviceData::GetHeapIndex() const noexcept {
    return m_descriptorHeapIndex;
}

const DirectX::SpriteFont& TextDeviceData::GetSpriteFont() const noexcept {
    return *m_pSpriteFont;
}

void TextDeviceData::SetDescriptorHeap(DirectX::DescriptorHeap* pDescriptorHeap) noexcept {
    m_pDescriptorHeap = pDescriptorHeap;
}

