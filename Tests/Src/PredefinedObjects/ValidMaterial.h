#pragma once

#include <RoX/Material.h>

#include <memory>

class ValidMaterial {
    public:
        static constexpr wchar_t TEXTURE_FILE_PATH[128] = L"texture.png";
        static constexpr wchar_t FONT_FILE_PATH[128] = L"font.spritefont";

    public:
        ValidMaterial();

        std::shared_ptr<Material> NewValidMaterial();

        std::shared_ptr<Material> pMaterial;
        std::uint64_t MaterialGUID;
        std::string MaterialName;
};
