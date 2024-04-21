#pragma once

#include <string>

class Material {
    public:
        Material(
                const std::string name,
                const std::wstring diffuseMapFilePath,
                const std::wstring normalMapFilePath
                ) noexcept;
        ~Material() noexcept;

    public:
        std::string GetName() const noexcept;
        std::wstring GetDiffuseMapFilePath() const noexcept;
        std::wstring GetNormalMapFilePath() const noexcept;

    private:
        const std::string m_name;
        const std::wstring m_diffuseMapFilePath;
        const std::wstring m_normalMapFilePath;

};
