#include "ValidAssetBatch.h"

ValidAssetBatch::ValidAssetBatch() {
    auto pSubmesh = std::make_unique<Submesh>();
    auto pMesh = std::make_shared<Mesh>();
    pMesh->Add(std::move(pSubmesh));

    pMaterial = std::make_shared<Material>(TEXTURE_FILE_PATH, TEXTURE_FILE_PATH);

    pModel = std::make_shared<Model>(pMaterial);
    pModel->Add(pMesh);

    pSprite = std::make_shared<Sprite>(TEXTURE_FILE_PATH);
    pText = std::make_shared<Text>(TEXTURE_FILE_PATH, L"");

    pOutline = std::make_shared<RayOutline>();

    MaterialGUID = pMaterial->GetGUID();
    ModelGUID    = pModel->GetGUID();
    SpriteGUID   = pSprite->GetGUID();
    TextGUID     = pText->GetGUID();
    OutlineGUID  = pOutline->GetGUID();

    MaterialName = pMaterial->GetName();
    ModelName    = pModel->GetName();
    SpriteName   = pSprite->GetName();
    TextName     = pText->GetName();
    OutlineName  = pOutline->GetName();

    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 128, true);
}

