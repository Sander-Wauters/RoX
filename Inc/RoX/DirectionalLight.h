#pragma once

#include "DirectXMath.h"
#include "DirectXColors.h"

#include "Asset.h"

class DirectionalLight : public Asset {
    public:
        DirectionalLight(
                std::string name = "",
                DirectX::XMFLOAT3 direction = { 0.f, 1.f, 0.f }, 
                DirectX::XMVECTOR diffuseColor = DirectX::Colors::White, 
                DirectX::XMVECTOR specularColor = DirectX::Colors::White,
                bool visible = true) noexcept;

    public:
        bool IsVisible() const noexcept;

        DirectX::XMFLOAT3& GetDirection() noexcept;
        DirectX::XMFLOAT3& GetDiffuseColor() noexcept;
        DirectX::XMFLOAT3& GetSpecularColor() noexcept;

        void SetVisible(bool visible) noexcept;

    private:
        bool m_visible;

        DirectX::XMFLOAT3 m_direction;
        DirectX::XMFLOAT3 m_diffuseColor;
        DirectX::XMFLOAT3 m_specularColor;
};
