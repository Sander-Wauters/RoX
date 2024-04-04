#include "RoX/Material.h"

Material::Material(
        const std::string name,
        const std::wstring diffuseMapFilePath,
        const std::wstring normalMapFilePath
        ) 
    noexcept : m_name(name),
    m_diffuseMapFilePath(diffuseMapFilePath),
    m_normalMapFilePath(normalMapFilePath)
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

