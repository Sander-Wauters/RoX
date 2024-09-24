#include "RoX/Material.h"

#include "../Exceptions/ThrowIfFailed.h"

Material::Material(
        const std::wstring diffuseMapFilePath,
        const std::wstring normalMapFilePath,
        std::string name,
        std::uint32_t flags,
        DirectX::XMVECTOR diffuseColor,
        DirectX::XMVECTOR emissiveColor,
        DirectX::XMVECTOR specularColor)
    noexcept : Asset("material", name),
    m_diffuseMapFilePath(diffuseMapFilePath),
    m_normalMapFilePath(normalMapFilePath),
    m_flags(flags)
{
    DirectX::XMStoreFloat4(&m_diffuseColor, diffuseColor);
    DirectX::XMStoreFloat4(&m_emissiveColor, emissiveColor);
    DirectX::XMStoreFloat4(&m_specularColor, specularColor);
}

Material::Material(
        const std::string diffuseMapFilePath,
        const std::string normalMapFilePath,
        std::string name,
        std::uint32_t flags,
        DirectX::XMVECTOR diffuseColor,
        DirectX::XMVECTOR emissiveColor,
        DirectX::XMVECTOR specularColor) 
noexcept : Material(
        AnsiToWString(diffuseMapFilePath), 
        AnsiToWString(normalMapFilePath),
        name, flags,
        diffuseColor,
        emissiveColor,
        specularColor)
{}

Material::~Material() noexcept {
}

std::wstring Material::GetDiffuseMapFilePath() const noexcept {
    return m_diffuseMapFilePath;
}

std::wstring Material::GetNormalMapFilePath() const noexcept {
    return m_normalMapFilePath;
}

std::uint32_t Material::GetFlags() const noexcept {
    return m_flags;
}

DirectX::XMFLOAT4& Material::GetDiffuseColor() noexcept {
    return m_diffuseColor;
}

DirectX::XMFLOAT4& Material::GetEmissiveColor() noexcept {
    return m_emissiveColor;
}

DirectX::XMFLOAT4& Material::GetSpecularColor() noexcept {
    return m_specularColor;
}

