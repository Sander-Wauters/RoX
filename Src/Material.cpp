#include "RoX/Material.h"

Material::Material(
        const std::string name,
        const std::wstring diffuseMapFilePath,
        const std::wstring normalMapFilePath,
        std::uint32_t flags)
    noexcept : m_name(name),
    m_diffuseMapFilePath(diffuseMapFilePath),
    m_normalMapFilePath(normalMapFilePath),
    m_flags(flags),
    m_diffuseColor({{ 1.f, 1.f, 1.f, 1.f }}),
    m_emissiveColor({{ 0.f, 0.f, 0.f }}),
    m_specularColor({{ 1.f, 1.f, 1.f }})
{}

Material::~Material() noexcept {
}

std::string Material::GetName() const noexcept {
    return m_name;
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

DirectX::XMVECTOR& Material::GetDiffuseColor() noexcept {
    return m_diffuseColor;
}

DirectX::XMVECTOR& Material::GetEmissiveColor() noexcept {
    return m_emissiveColor;
}

DirectX::XMVECTOR& Material::GetSpecularColor() noexcept {
    return m_specularColor;
}

