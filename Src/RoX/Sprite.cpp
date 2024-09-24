#include "RoX/Sprite.h"

#include "../Exceptions/ThrowIfFailed.h"

// ---------------------------------------------------------------- //
//                          Sprite
// ---------------------------------------------------------------- //

Sprite::Sprite(
        const std::wstring filePath,
        std::string name,
        DirectX::XMFLOAT2 origin,
        DirectX::XMFLOAT2 offset, 
        DirectX::XMFLOAT2 scale,
        float layer,
        float angle,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Asset("sprite", name),
    m_filePath(filePath),
    m_origin(origin),
    m_offset(offset),
    m_scale(scale),
    m_layer(layer),
    m_angle(angle),
    m_visible(visible)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

Sprite::Sprite(const std::string filePath,
        std::string name,
        DirectX::XMFLOAT2 origin,
        DirectX::XMFLOAT2 offset, 
        DirectX::XMFLOAT2 scale,
        float layer, 
        float angle,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Sprite(
            AnsiToWString(filePath),
            name,
            origin, offset, scale,
            layer, angle,
            color, visible)
{}

std::wstring Sprite::GetFilePath() const noexcept {
    return m_filePath;
}

DirectX::XMFLOAT2& Sprite::GetOrigin() noexcept {
    return m_origin;
}

DirectX::XMFLOAT2& Sprite::GetOffset() noexcept {
    return m_offset;
}

DirectX::XMFLOAT2& Sprite::GetScale() noexcept {
    return m_scale;
}

DirectX::XMFLOAT4& Sprite::GetColor() noexcept {
    return m_color;
}

float Sprite::GetAngle() const noexcept {
    return m_angle;
}

float Sprite::GetLayer() const noexcept {
    return m_layer;
}

bool Sprite::IsVisible() const noexcept {
    return m_visible;
}

void Sprite::SetAngle(float angle) noexcept {
    m_angle = angle;
}

void Sprite::SetLayer(float layer) noexcept {
    m_layer = layer;
}

void Sprite::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          Text
// ---------------------------------------------------------------- //

Text::Text(const std::wstring filePath,
        std::wstring content,
        std::string name,
        DirectX::XMFLOAT2 origin,
        DirectX::XMFLOAT2 offset, 
        DirectX::XMFLOAT2 scale,
        float layer, 
        float angle,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Sprite(filePath, name, origin, offset, scale, layer, angle, color, visible),
    m_content(content)
{}

Text::Text(const std::string filePath,
        std::string content,
        std::string name,
        DirectX::XMFLOAT2 origin,
        DirectX::XMFLOAT2 offset, 
        DirectX::XMFLOAT2 scale,
        float layer, 
        float angle,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Text(
            AnsiToWString(filePath), 
            AnsiToWString(content),
            name,
            origin, offset, scale,
            layer, angle, color, visible)
{}

std::wstring Text::GetContent() const noexcept {
    return m_content;
}

void Text::SetContent(std::wstring content) noexcept {
    m_content = content;
}

void Text::SetContent(std::string content) noexcept {
    SetContent(AnsiToWString(content));
}

