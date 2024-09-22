#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "DeviceResources.h"

class TextureDeviceData : public IDeviceObserver {
    public:
        TextureDeviceData(
                DeviceResources& deviceResources, 
                DirectX::DescriptorHeap& descriptorHeap,
                std::uint32_t heapIndex, 
                std::wstring filePath);
        ~TextureDeviceData();

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

    private:
        std::future<void> CreateTextureResource();

    public:
        std::uint8_t GetHeapIndex() const noexcept;

        Microsoft::WRL::ComPtr<ID3D12Resource>& GetTexture() noexcept;

    private:
        const std::wstring m_filePath;

        DeviceResources& m_deviceResources;
        DirectX::DescriptorHeap& m_descriptorHeap;

        std::uint8_t m_desciptorHeapIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pTexture;        
};

