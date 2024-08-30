#pragma once

#include <string>

#include "Sprite.h"

// Describes a 2D texture containing a text string.
class Text : public Sprite {
    public:
        Text(
                const std::string name,
                const std::wstring filePath,
                std::wstring content,
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::Black,
                bool visible = true
            ) noexcept :
            Sprite(name, filePath, origin, offset, scale, layer, angle, color, visible),
            m_content(content)
            {}
        ~Text() noexcept {};

    public:
        std::wstring GetContent() const noexcept { return m_content; }

        void SetContent(std::wstring content) noexcept { m_content = content; }

    private:
        std::wstring m_content;
};

