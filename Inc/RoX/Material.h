#pragma once

#include <string>

namespace RenderFlags {
   constexpr std::uint32_t None = 0x0;
   constexpr std::uint32_t Instancing = 0x1;
}

class Material {
    public:
        Material(
                const std::string name,
                const std::wstring diffuseMapFilePath,
                const std::wstring normalMapFilePath,
                std::uint32_t flags = RenderFlags::None
                ) noexcept;
        ~Material() noexcept;

    public:
        std::string GetName() const noexcept;
        std::wstring GetDiffuseMapFilePath() const noexcept;
        std::wstring GetNormalMapFilePath() const noexcept;

        std::uint32_t GetFlags() const noexcept;

    private:
        const std::string m_name;
        const std::wstring m_diffuseMapFilePath;
        const std::wstring m_normalMapFilePath;

        std::uint32_t m_flags;
};
