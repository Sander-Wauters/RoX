#pragma once
#include <RoX/AssetBatch.h>
#include <RoX/MeshFactory.h>

class ValidAssetBatch {
    public:
        static constexpr char INVALID_NAME[128] = "INVALID_NAME";
        static constexpr wchar_t TEXTURE_FILE_PATH[128] = L"texture.png";
        static constexpr wchar_t FONT_FILE_PATH[128] = L"font.spritefont";

    public:
        ValidAssetBatch();

        std::shared_ptr<Material> NewValidMaterial();
        std::shared_ptr<Sprite>   NewValidSprite();
        std::shared_ptr<Text>     NewValidText();
        std::shared_ptr<Model>    NewValidModelWithNewValidMaterial();
        std::shared_ptr<Model>    NewValidModelWithExistingMaterial();
        std::shared_ptr<Outline>  NewValidOutline();

    public:
        std::shared_ptr<Material> pMaterial;

        std::shared_ptr<Model> pModel;

        std::shared_ptr<Sprite> pSprite;
        std::shared_ptr<Text> pText;

        std::shared_ptr<Outline> pOutline;

        std::shared_ptr<AssetBatch> pBatch;

        std::uint64_t MaterialGUID;
        std::uint64_t ModelGUID;
        std::uint64_t SpriteGUID;
        std::uint64_t TextGUID;
        std::uint64_t OutlineGUID;

        std::string MaterialName;
        std::string ModelName;
        std::string SpriteName;
        std::string TextName;
        std::string OutlineName;

};
