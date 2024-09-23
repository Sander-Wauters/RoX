#include "ValidAssetBatch.h"

#include <RoX/MeshFactory.h>

ValidAssetBatch::ValidAssetBatch() {
    pSprite = std::make_shared<Sprite>(TEXTURE_FILE_PATH);
    pText = std::make_shared<Text>(FONT_FILE_PATH, L"");

    pOutline = std::make_shared<RayOutline>();

    SpriteGUID   = pSprite->GetGUID();
    TextGUID     = pText->GetGUID();
    OutlineGUID  = pOutline->GetGUID();

    SpriteName   = pSprite->GetName();
    TextName     = pText->GetName();
    OutlineName  = pOutline->GetName();

    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 128, true);
}

std::shared_ptr<Sprite> ValidAssetBatch::NewValidSprite() {
    return std::make_shared<Sprite>(TEXTURE_FILE_PATH);
}

std::shared_ptr<Text> ValidAssetBatch::NewValidText() {
    return std::make_shared<Text>(FONT_FILE_PATH, L"");
}

std::shared_ptr<Outline> ValidAssetBatch::NewValidOutline() {
    return std::make_shared<RayOutline>();
}

