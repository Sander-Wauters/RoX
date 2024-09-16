#pragma once
#include "../../../Inc/RoX/AssetBatch.h"

class ValidAssetBatch {
    public:
        static constexpr char INVALID_NAME[13] = "INVALID_NAME";

    public:
        ValidAssetBatch();

    public:
        std::shared_ptr<Material> pMaterial;

        // Models share meshes and materials.
        std::shared_ptr<Model> pModel1;
        std::shared_ptr<Model> pModel2;

        std::shared_ptr<Sprite> pSprite;
        std::shared_ptr<Text> pText;

        std::shared_ptr<Outline> pOutline;

        std::shared_ptr<AssetBatch> pBatch;

        std::uint64_t MaterialGUID;
        std::uint64_t Model1GUID;
        std::uint64_t Model2GUID;
        std::uint64_t SpriteGUID;
        std::uint64_t TextGUID;
        std::uint64_t OutlineGUID;

        std::string MaterialName;
        std::string Model1Name;
        std::string Model2Name;
        std::string SpriteName;
        std::string TextName;
        std::string OutlineName;

};
