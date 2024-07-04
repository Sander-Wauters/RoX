#pragma once

#include <SpriteFont.h>

#include "Util/pch.h"

class TextDeviceData {
    public:
        TextDeviceData(std::uint32_t heapIndex) noexcept :
            m_descriptorHeapIndex(heapIndex),
            m_pSpriteFont(nullptr) {}

        void OnDeviceLost() noexcept { m_pSpriteFont.reset(); }

    public:
        std::uint32_t GetHeapIndex() const noexcept { return m_descriptorHeapIndex; }
        DirectX::SpriteFont* GetSpriteFont() noexcept { return m_pSpriteFont.get(); }

        void SetHeapIndex(std::uint32_t heapIndex) noexcept { m_descriptorHeapIndex = heapIndex; }
        void SetSpriteFont(std::unique_ptr<DirectX::SpriteFont> pSpriteFont) noexcept { m_pSpriteFont = std::move(pSpriteFont); }

    private:
        std::uint32_t m_descriptorHeapIndex;
        std::unique_ptr<DirectX::SpriteFont> m_pSpriteFont;
};

