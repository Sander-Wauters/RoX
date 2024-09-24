#pragma once

#include <string>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Asset.h"

// Describes a 2D texture.
// Positioning
//      -y
//      |
// -x --+-- +x
//      |
//      +y
class Sprite : public Asset {
    public: 
        Sprite(const std::wstring filePath,
                std::string name = "",
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, 
                float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
              ) noexcept;
        Sprite(const std::string filePath,
                std::string name = "",
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, 
                float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
              ) noexcept;
        virtual ~Sprite() noexcept = default;

    public:
        std::wstring GetFilePath() const noexcept;

        DirectX::XMFLOAT2& GetOrigin() noexcept;
        DirectX::XMFLOAT2& GetOffset() noexcept;
        DirectX::XMFLOAT2& GetScale() noexcept;
        DirectX::XMFLOAT4& GetColor() noexcept;

        float GetAngle() const noexcept;  
        float GetLayer() const noexcept;

        bool IsVisible() const noexcept;

        void SetAngle(float angle) noexcept;
        void SetLayer(float layer) noexcept;

        void SetVisible(bool visible) noexcept;

    private:
        const std::wstring m_filePath;

        DirectX::XMFLOAT2 m_origin;
        DirectX::XMFLOAT2 m_offset;
        DirectX::XMFLOAT2 m_scale;
        DirectX::XMFLOAT4 m_color;

        float m_layer;
        float m_angle; // In radians.

        bool m_visible;
};

// Describes a 2D texture containing a text string.
class Text : public Sprite {
    public:
        Text(const std::wstring filePath,
                std::wstring content,
                std::string name = "",
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, 
                float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::Black,
                bool visible = true
            ) noexcept;
        Text(const std::string filePath,
                std::string content,
                std::string name = "",
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, 
                float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::Black,
                bool visible = true
            ) noexcept;

    public:
        std::wstring GetContent() const noexcept;

        void SetContent(std::wstring content) noexcept;
        void SetContent(std::string content) noexcept;

    private:
        std::wstring m_content;
};

