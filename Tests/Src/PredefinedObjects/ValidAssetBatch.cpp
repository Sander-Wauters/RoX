#include "ValidAssetBatch.h"

ValidAssetBatch::ValidAssetBatch() {
    auto pSubmesh = std::make_unique<Submesh>();
    auto pMesh = std::make_shared<Mesh>();
    pMesh->Add(std::move(pSubmesh));

    pMaterial = std::make_shared<Material>(L"", L"");

    pModel1 = std::make_shared<Model>(pMaterial);
    pModel1->Add(pMesh);

    pModel2 = std::make_shared<Model>(pMaterial);
    pModel2->Add(pMesh);

    pSprite = std::make_shared<Sprite>(L"");
    pText = std::make_shared<Text>(L"", L"");

    pOutline = std::make_shared<RayOutline>();

    MaterialGUID = pMaterial->GetGUID();
    Model1GUID   = pModel1->GetGUID();
    Model2GUID   = pModel2->GetGUID();
    SpriteGUID   = pSprite->GetGUID();
    TextGUID     = pText->GetGUID();
    OutlineGUID  = pOutline->GetGUID();

    MaterialName = pMaterial->GetName();
    Model1Name   = pModel1->GetName();
    Model2Name   = pModel2->GetName();
    SpriteName   = pSprite->GetName();
    TextName     = pText->GetName();
    OutlineName  = pOutline->GetName();

    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", true, 128);
}

