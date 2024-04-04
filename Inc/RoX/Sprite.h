#pragma once

#include "../../Src/Util/pch.h"

// Positioning
//      -y
//      |
// -x --+-- +x
//      |
//      +y
class Sprite {
    public: 
        Sprite(
                const std::string name,
                const std::wstring filePath,
                DirectX::XMFLOAT2 origin = { 0.f, 0.f },
                DirectX::XMFLOAT2 offset = { 0.f, 0.f }, 
                DirectX::XMFLOAT2 scale = { 1.f, 1.f },
                float layer = 0.f, float angle = 0.f,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
              ) noexcept :
            m_name(name),
            m_filePath(filePath),
            m_origin(origin),
            m_offset(offset), 
            m_scale(scale),
            m_color(color),
            m_layer(layer), m_angle(angle),
            m_visible(visible)
            {}
        virtual ~Sprite() noexcept {}

    public:
        std::string GetName() const noexcept { return m_name; }
        std::wstring GetFilePath() const noexcept { return m_filePath; }

        const DirectX::XMFLOAT2& GetOrigin() const noexcept { return m_origin; }
        const DirectX::XMFLOAT2& GetOffset() const noexcept { return m_offset; }
        const DirectX::XMFLOAT2& GetScale() const noexcept { return m_scale; }
        const DirectX::XMVECTOR& GetColor() const noexcept { return m_color; }

        float GetAngle() const noexcept { return m_angle; }  
        float GetLayer() const noexcept { return m_layer; }

        bool IsVisible() const noexcept { return m_visible; }

        void SetOrigin(DirectX::XMFLOAT2 origin) noexcept { m_origin = origin; }
        void SetOffset(DirectX::XMFLOAT2 offset) noexcept { m_offset = offset; }
        void SetScale(DirectX::XMFLOAT2 scale) noexcept { m_scale = scale; }
        void SetColor(DirectX::XMVECTOR color) noexcept { m_color = color; }

        void SetAngle(float angle) noexcept { m_angle = angle; }
        void SetLayer(float layer) noexcept { m_layer = layer; }

        void SetVisible(bool visible) noexcept { m_visible = visible; }

    private:
        const std::string m_name;
        const std::wstring m_filePath;

        DirectX::XMFLOAT2 m_origin;
        DirectX::XMFLOAT2 m_offset;
        DirectX::XMFLOAT2 m_scale;
        DirectX::XMVECTOR m_color;

        float m_layer;
        float m_angle; // In radians.

        bool m_visible;
};
