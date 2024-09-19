#pragma once

#include "../Util/pch.h"

class TextureDeviceData {
    public:
        TextureDeviceData(std::uint32_t heapIndex) noexcept :
            m_numReferences(1),
            m_desciptorHeapIndex(heapIndex),
            m_pTexture(nullptr) {}

        void OnDeviceLost() noexcept { m_pTexture.Reset(); }

        void IncreaseRefCount() noexcept { ++m_numReferences; }
        void DecreaseRefCount() noexcept { --m_numReferences; }

    public:
        bool HasReferences() const noexcept { return m_numReferences != 0; }

        std::uint8_t GetHeapIndex() const noexcept { return m_desciptorHeapIndex; }

        Microsoft::WRL::ComPtr<ID3D12Resource>& GetTexture() noexcept { return m_pTexture; }

        void SetHeapIndex(std::uint8_t heapIndex) noexcept { m_desciptorHeapIndex = heapIndex; }

    private:
        std::uint8_t m_numReferences;

        std::uint8_t m_desciptorHeapIndex;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pTexture;        
};

