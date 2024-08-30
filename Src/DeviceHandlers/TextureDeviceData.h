#pragma once

#include "../Util/pch.h"

class TextureDeviceData {
    public:
        TextureDeviceData(std::uint32_t heapIndex) noexcept :
            m_desciptorHeapIndex(heapIndex),
            m_pTexture(nullptr) {}

        void OnDeviceLost() noexcept { m_pTexture.Reset(); }

    public:
        std::uint32_t GetHeapIndex() const noexcept { return m_desciptorHeapIndex; }

        Microsoft::WRL::ComPtr<ID3D12Resource>& GetTexture() noexcept { return m_pTexture; }

        void SetHeapIndex(std::uint32_t heapIndex) noexcept { m_desciptorHeapIndex = heapIndex; }

    private:
        std::uint32_t m_desciptorHeapIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pTexture;        
};

