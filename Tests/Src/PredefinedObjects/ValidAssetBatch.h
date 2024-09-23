#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/MeshFactory.h>

#include "ValidModel.h"

class ValidAssetBatch : public ValidModel {
    public:
        static constexpr char INVALID_NAME[128] = "INVALID_NAME";

    public:
        ValidAssetBatch();

        std::shared_ptr<Sprite>   NewValidSprite();
        std::shared_ptr<Text>     NewValidText();
        std::shared_ptr<Outline>  NewValidOutline();

    public:
        std::shared_ptr<Sprite> pSprite;
        std::shared_ptr<Text> pText;

        std::shared_ptr<Outline> pOutline;

        std::shared_ptr<AssetBatch> pBatch;

        std::uint64_t SpriteGUID;
        std::uint64_t TextGUID;
        std::uint64_t OutlineGUID;

        std::string SpriteName;
        std::string TextName;
        std::string OutlineName;
};
