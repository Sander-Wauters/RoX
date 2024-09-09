#include "RoX/Material.h"

Material::Material(
        const std::wstring diffuseMapFilePath,
        const std::wstring normalMapFilePath,
        std::string name,
        std::uint32_t flags)
    noexcept : Asset("material", name),
    m_diffuseMapFilePath(diffuseMapFilePath),
    m_normalMapFilePath(normalMapFilePath),
    m_flags(flags),
    m_diffuseColor({ 1.f, 1.f, 1.f, 1.f }),
    m_emissiveColor({ 0.f, 0.f, 0.f, 1.f }),
    m_specularColor({ 1.f, 1.f, 1.f, 1.f })
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

