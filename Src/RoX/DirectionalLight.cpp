#include "RoX/DirectionalLight.h"

DirectionalLight::DirectionalLight(
        std::string name,
        DirectX::XMFLOAT3 direction, 
        DirectX::XMVECTOR diffuseColor, 
        DirectX::XMVECTOR specularColor,
        bool visible) 
    noexcept : Identifiable("directional_light", name),
    m_visible(visible),
    m_direction(direction)
{
    DirectX::XMStoreFloat3(&m_diffuseColor, diffuseColor); 
    DirectX::XMStoreFloat3(&m_specularColor, specularColor);
}

bool DirectionalLight::IsVisible() const noexcept {
    return m_visible;
}

DirectX::XMFLOAT3& DirectionalLight::GetDirection() noexcept {
    return m_direction;
}

DirectX::XMFLOAT3& DirectionalLight::GetDiffuseColor() noexcept {
    return m_diffuseColor;
}

DirectX::XMFLOAT3& DirectionalLight::GetSpecularColor() noexcept {
    return m_specularColor;
}

void DirectionalLight::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

