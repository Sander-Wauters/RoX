#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "DeviceResources.h"

class TextDeviceData : public IDeviceObserver {
    public:
        TextDeviceData(
                DeviceResources& deviceResources, 
                DirectX::DescriptorHeap& descriptorHeap,
                std::uint32_t heapIndex,
                std::wstring filePath);
        ~TextDeviceData();

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

    private:
        std::future<void> CreateSpriteFont();

    public:
        std::uint8_t GetHeapIndex() const noexcept;

        const DirectX::SpriteFont& GetSpriteFont() const noexcept;

    private:
        const std::wstring m_filePath;

        DeviceResources& m_deviceResources;
        DirectX::DescriptorHeap& m_descriptorHeap;

        std::uint8_t m_descriptorHeapIndex;
        std::unique_ptr<DirectX::SpriteFont> m_pSpriteFont;
};

