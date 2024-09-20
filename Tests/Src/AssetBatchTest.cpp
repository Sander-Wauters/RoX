#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <RoX/MeshFactory.h>

#include "MockAssetBatchObserver.h"

#include "PredefinedObjects/ValidAssetBatch.h"

class GeneralAssetBatchTest : public testing::Test, public ValidAssetBatch {
    protected:
        GeneralAssetBatchTest() {}
};

class EmptyAssetBatchTest : public testing::Test, public ValidAssetBatch {
    protected:
        EmptyAssetBatchTest() {}
};

class FilledAssetBatchTest : public testing::Test, public ValidAssetBatch {
    protected:
        FilledAssetBatchTest() {
            pBatch->Add(pMaterial);
            pBatch->Add(pModel);
            pBatch->Add(pSprite);
            pBatch->Add(pText);
            pBatch->Add(pOutline);
        }
};

TEST_F(GeneralAssetBatchTest, Add_WithNewMaterial_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);

    auto pNewMaterial1 = std::make_shared<Material>(L"unique_texture_1", L"unique_texture_1");
    auto pNewMaterial2 = std::make_shared<Material>(L"unique_texture_1", L"unique_texture_2");

    EXPECT_NO_THROW(pBatch->Add(pNewMaterial1));
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);

    EXPECT_THROW(pBatch->Add(pNewMaterial2), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);
}

TEST_F(GeneralAssetBatchTest, Add_WithNewSprite_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);

    auto pNewSprite1 = std::make_shared<Sprite>(L"unique_texture_1");
    auto pNewSprite2 = std::make_shared<Sprite>(L"unique_texture_2");

    EXPECT_NO_THROW(pBatch->Add(pNewSprite1));
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);

    EXPECT_THROW(pBatch->Add(pNewSprite2), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);
}

TEST_F(GeneralAssetBatchTest, Add_WithNewText_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);

    auto pNewText1 = std::make_shared<Text>(L"unique_texture_1", L"");
    auto pNewText2 = std::make_shared<Text>(L"unique_texture_2", L"");

    EXPECT_NO_THROW(pBatch->Add(pNewText1));
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);

    EXPECT_THROW(pBatch->Add(pNewText2), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumUniqueTextures(), 1);
}

TEST_F(GeneralAssetBatchTest, RegisterAssetBatchObserver_WithValidObserver) {
    MockAssetBatchObserver observer;
    EXPECT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));
}

TEST_F(GeneralAssetBatchTest, RegisterAssetBatchObserver_WithInvalidObserver) {
    EXPECT_THROW(pBatch->RegisterAssetBatchObserver(nullptr), std::invalid_argument);
}

TEST_F(GeneralAssetBatchTest, DeregisterAssetBatchObserver_WithValidObserver) {
    MockAssetBatchObserver observer;
    EXPECT_NO_THROW(pBatch->DeregisterAssetBatchObserver(&observer));
}

TEST_F(GeneralAssetBatchTest, DeregisterAssetBatchObserver_WithInvalidObserver) {
    EXPECT_NO_THROW(pBatch->DeregisterAssetBatchObserver(nullptr));
}

TEST_F(GeneralAssetBatchTest, FindGUID_ByName_WithInvalidName) {
    EXPECT_EQ(pBatch->FindGUID(INVALID_NAME, pBatch->GetMaterials()), Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(INVALID_NAME, pBatch->GetModels()),    Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(INVALID_NAME, pBatch->GetSprites()),   Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(INVALID_NAME, pBatch->GetTexts()),     Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(INVALID_NAME, pBatch->GetOutlines()),  Asset::INVALID_GUID);
}

TEST_F(GeneralAssetBatchTest, Add_WithInvalidMaterial) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Material>()), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumMaterials(), 0);
}

TEST_F(GeneralAssetBatchTest, Add_WithNewModelWithInvalidMaterial) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    pModel->GetMaterials() = { nullptr };
    EXPECT_THROW(pBatch->Add(pModel), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumMaterials(), 0);
    EXPECT_EQ(pBatch->GetNumModels(),    0);
}

TEST_F(GeneralAssetBatchTest, Add_WithInvalidModel) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Model>()), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumModels(), 0);

    auto pModelWithoutGeo = std::make_shared<Model>(pMaterial);
    EXPECT_THROW(pBatch->Add(pModelWithoutGeo), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumModels(), 0);
}

TEST_F(GeneralAssetBatchTest, Add_WithInvalidSprite) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pSprite)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Sprite>()), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumSprites(), 0);
}

TEST_F(GeneralAssetBatchTest, Add_WithInvalidText) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pText)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Text>()), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumTexts(), 0);
}

TEST_F(GeneralAssetBatchTest, Add_WithInvalidOutline) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pOutline)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Outline>()), std::invalid_argument);
    EXPECT_EQ(pBatch->GetNumOutlines(), 0);
}

TEST_F(GeneralAssetBatchTest, Remove_ByGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->RemoveMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->RemoveModel(Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(pBatch->RemoveSprite(Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(pBatch->RemoveText(Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(pBatch->RemoveOutline(Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, Remove_ByTypeAndGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, Remove_ByName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->RemoveMaterial(INVALID_NAME), std::out_of_range);
    EXPECT_THROW(pBatch->RemoveModel(INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(pBatch->RemoveSprite(INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(pBatch->RemoveText(INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(pBatch->RemoveOutline(INVALID_NAME),  std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, Remove_ByTypeAndName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, INVALID_NAME), std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, INVALID_NAME),  std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, UpdateIMesh_WithInvalidModelGUIDAndValidIMeshGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnUpdate(pModel->GetMeshes().front())).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->UpdateIMesh(Asset::INVALID_GUID, pModel->GetMeshes().front()->GetGUID()), std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, UpdateIMesh_WithInvalidModelGUIDAndInvalidIMeshGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnUpdate(pModel->GetMeshes().front())).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->UpdateIMesh(Asset::INVALID_GUID, Asset::INVALID_GUID), std::out_of_range);
}

TEST_F(GeneralAssetBatchTest, GetAsset_ByGUID_WithGUID) {
    EXPECT_THROW(pBatch->GetMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->GetModel(   Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->GetSprite(  Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->GetText(    Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(pBatch->GetOutline( Asset::INVALID_GUID), std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, FindGUID_ByName_WithValidName) {
    EXPECT_EQ(pBatch->FindGUID(MaterialName, pBatch->GetMaterials()), Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(ModelName,    pBatch->GetModels()),    Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(SpriteName,   pBatch->GetSprites()),   Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(TextName,     pBatch->GetTexts()),     Asset::INVALID_GUID);
    EXPECT_EQ(pBatch->FindGUID(OutlineName,  pBatch->GetOutlines()),  Asset::INVALID_GUID);
}

TEST_F(EmptyAssetBatchTest, Add_WithNewMaterial) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pMaterial));
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
}

TEST_F(EmptyAssetBatchTest, Add_WithNewModelWithNewMaterial) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pModel));
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
    EXPECT_EQ(pBatch->GetNumModels(),    1);
}

TEST_F(EmptyAssetBatchTest, Add_WithNewSprite) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pSprite)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pSprite)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pSprite));
    EXPECT_EQ(pBatch->GetNumSprites(), 1);
}

TEST_F(EmptyAssetBatchTest, Add_WithNewText) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pText)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pText)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pText));
    EXPECT_EQ(pBatch->GetNumTexts(), 1);
}

TEST_F(EmptyAssetBatchTest, Add_WithNewOutline) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pOutline)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pOutline)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pOutline));
    EXPECT_EQ(pBatch->GetNumOutlines(), 1);
}

TEST_F(EmptyAssetBatchTest, Remove_ByGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->RemoveMaterial(MaterialGUID), std::out_of_range);
    EXPECT_THROW(pBatch->RemoveModel(ModelGUID),       std::out_of_range);
    EXPECT_THROW(pBatch->RemoveSprite(SpriteGUID),     std::out_of_range);
    EXPECT_THROW(pBatch->RemoveText(TextGUID),         std::out_of_range);
    EXPECT_THROW(pBatch->RemoveOutline(OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialGUID), std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelGUID),       std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteGUID),     std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextGUID),         std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->RemoveMaterial(MaterialName), std::out_of_range);
    EXPECT_THROW(pBatch->RemoveModel(ModelName),       std::out_of_range);
    EXPECT_THROW(pBatch->RemoveSprite(SpriteName),     std::out_of_range);
    EXPECT_THROW(pBatch->RemoveText(TextName),         std::out_of_range);
    EXPECT_THROW(pBatch->RemoveOutline(OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialName), std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelName),      std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteName),     std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextName),         std::out_of_range);
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByGUID_WithValidGUID) {
    EXPECT_THROW(pBatch->GetMaterial(MaterialGUID), std::out_of_range);
    EXPECT_THROW(pBatch->GetModel(ModelGUID),       std::out_of_range);
    EXPECT_THROW(pBatch->GetSprite(SpriteGUID),     std::out_of_range);
    EXPECT_THROW(pBatch->GetText(TextGUID),         std::out_of_range);
    EXPECT_THROW(pBatch->GetOutline(OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByName_WithValidName) {
    EXPECT_THROW(pBatch->GetMaterial(MaterialName), std::out_of_range);
    EXPECT_THROW(pBatch->GetModel(ModelName),       std::out_of_range);
    EXPECT_THROW(pBatch->GetSprite(SpriteName),     std::out_of_range);
    EXPECT_THROW(pBatch->GetText(TextName),         std::out_of_range);
    EXPECT_THROW(pBatch->GetOutline(OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByName_WithInvalidName) {
    EXPECT_THROW(pBatch->GetMaterial(INVALID_NAME), std::out_of_range);
    EXPECT_THROW(pBatch->GetModel(INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(pBatch->GetSprite(INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(pBatch->GetText(INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(pBatch->GetOutline(INVALID_NAME),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetStats) {
    EXPECT_EQ(pBatch->GetNumMaterials(), 0);
    EXPECT_EQ(pBatch->GetNumModels(),    0);
    EXPECT_EQ(pBatch->GetNumMeshes(),    0);
    EXPECT_EQ(pBatch->GetNumSubmeshes(), 0);
    EXPECT_EQ(pBatch->GetNumSprites(),   0);
    EXPECT_EQ(pBatch->GetNumTexts(),     0);
    EXPECT_EQ(pBatch->GetNumOutlines(),  0);

    EXPECT_EQ(pBatch->GetNumSubmeshInstances(),         0);
    EXPECT_EQ(pBatch->GetNumRenderedSubmeshInstances(), 0);
    EXPECT_EQ(pBatch->GetNumLoadedVertices(),           0);
    EXPECT_EQ(pBatch->GetNumRenderedVertices(),         0);
}

TEST_F(FilledAssetBatchTest, FindGUID_ByName_WithValidName) {
    EXPECT_EQ(pBatch->FindGUID(MaterialName, pBatch->GetMaterials()), MaterialGUID);
    EXPECT_EQ(pBatch->FindGUID(ModelName,    pBatch->GetModels()),    ModelGUID);
    EXPECT_EQ(pBatch->FindGUID(SpriteName,   pBatch->GetSprites()),   SpriteGUID);
    EXPECT_EQ(pBatch->FindGUID(TextName,     pBatch->GetTexts()),     TextGUID);
    EXPECT_EQ(pBatch->FindGUID(OutlineName,  pBatch->GetOutlines()),  OutlineGUID);
}

TEST_F(FilledAssetBatchTest, Add_WithNewMaterial) {
    auto pNewMaterial = std::make_shared<Material>(L"", L"");

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewMaterial)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pNewMaterial)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewMaterial));
    EXPECT_EQ(pBatch->GetNumMaterials(), 2);
}

TEST_F(FilledAssetBatchTest, Add_WithExistingMaterial) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pMaterial));
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
}

TEST_F(FilledAssetBatchTest, Add_WithExistingModel) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pModel));
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
    EXPECT_EQ(pBatch->GetNumModels(),    1);
}

TEST_F(FilledAssetBatchTest, Add_WithNewModelWithExistingMaterial) {
    auto pNewMesh = std::make_shared<Mesh>();
    MeshFactory::AddCube(*pNewMesh);
    auto pNewModel = std::make_shared<Model>(pMaterial);
    pNewModel->Add(pNewMesh);

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pNewModel)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pNewModel)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewModel));
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
    EXPECT_EQ(pBatch->GetNumModels(),    2);
}

TEST_F(FilledAssetBatchTest, Add_WithNewModelWithNewMaterial) {
    auto pNewMesh = std::make_shared<Mesh>();
    MeshFactory::AddCube(*pNewMesh);

    auto pNewMaterial = std::make_shared<Material>(L"", L"");

    auto pNewModel = std::make_shared<Model>(pNewMaterial);
    pNewModel->Add(pNewMesh);

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewMaterial)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(pNewModel)      ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pNewMaterial)).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pNewModel)   ).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewModel));
    EXPECT_EQ(pBatch->GetNumMaterials(), 2);
    EXPECT_EQ(pBatch->GetNumModels(),    2);
}

TEST_F(FilledAssetBatchTest, Add_WithNewSprite) {
    auto pNewSprite = std::make_shared<Sprite>(L"");

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewSprite)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pNewSprite)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewSprite));
    EXPECT_EQ(pBatch->GetNumSprites(), 2);
}

TEST_F(FilledAssetBatchTest, Add_WithExistingSprite) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pSprite)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pSprite)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pSprite));
    EXPECT_EQ(pBatch->GetNumSprites(), 1);
}

TEST_F(FilledAssetBatchTest, Add_WithNewText) {
    auto pNewText = std::make_shared<Text>(L"", L"");

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewText)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pNewText)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewText));
    EXPECT_EQ(pBatch->GetNumTexts(), 2);
}

TEST_F(FilledAssetBatchTest, Add_WithExistingText) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pText)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pText)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pText));
    EXPECT_EQ(pBatch->GetNumTexts(), 1);
}

TEST_F(FilledAssetBatchTest, Add_WithNewOutline) {
    std::shared_ptr<Outline> pNewOutline = std::make_shared<RayOutline>();

    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewOutline)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pNewOutline)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pNewOutline));
    EXPECT_EQ(pBatch->GetNumOutlines(), 2);
}

TEST_F(FilledAssetBatchTest, Add_WithExistingOutline) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pOutline)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pOutline)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->Add(pOutline));
    EXPECT_EQ(pBatch->GetNumOutlines(), 1);
}

TEST_F(FilledAssetBatchTest, Remove_ByGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->RemoveMaterial(MaterialGUID), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);

    EXPECT_NO_THROW(pBatch->RemoveModel(ModelGUID));
    EXPECT_THROW(pBatch->GetModel(ModelGUID), std::out_of_range);

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->RemoveMaterial(MaterialGUID));
    EXPECT_THROW(pBatch->GetMaterial(MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveSprite(SpriteGUID));
    EXPECT_THROW(pBatch->GetSprite(SpriteGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveText(TextGUID));
    EXPECT_THROW(pBatch->GetText(TextGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveOutline(OutlineGUID));
    EXPECT_THROW(pBatch->GetOutline(OutlineGUID), std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialGUID), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelGUID));
    EXPECT_THROW(pBatch->GetModel(ModelGUID), std::out_of_range);

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialGUID));
    EXPECT_THROW(pBatch->GetMaterial(MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteGUID));
    EXPECT_THROW(pBatch->GetSprite(SpriteGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextGUID));
    EXPECT_THROW(pBatch->GetText(TextGUID), std::out_of_range);
    
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineGUID));
    EXPECT_THROW(pBatch->GetOutline(OutlineGUID), std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->RemoveMaterial(MaterialName), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);

    EXPECT_NO_THROW(pBatch->RemoveModel(ModelName));
    EXPECT_THROW(pBatch->GetModel(ModelGUID),      std::out_of_range);

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->RemoveMaterial(MaterialName));
    EXPECT_THROW(pBatch->GetMaterial(MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveSprite(SpriteName));
    EXPECT_THROW(pBatch->GetSprite(SpriteGUID),     std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveText(TextName));
    EXPECT_THROW(pBatch->GetText(TextGUID),         std::out_of_range);

    EXPECT_NO_THROW(pBatch->RemoveOutline(OutlineName));
    EXPECT_THROW(pBatch->GetOutline(OutlineGUID),   std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(pMaterial)   ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pModel)      ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pSprite)     ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pText)       ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(pOutline)    ).Times(testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(pMaterial)).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pModel)   ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pSprite)  ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pText)    ).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(pOutline) ).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialName), std::runtime_error);
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelName));
    EXPECT_THROW(pBatch->GetModel(ModelGUID), std::out_of_range);

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialName));
    EXPECT_THROW(pBatch->GetMaterial(MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteName));
    EXPECT_THROW(pBatch->GetSprite(SpriteGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextName));
    EXPECT_THROW(pBatch->GetText(TextGUID), std::out_of_range);

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineName));
    EXPECT_THROW(pBatch->GetOutline(OutlineGUID), std::out_of_range);
}

TEST_F(FilledAssetBatchTest, UpdateIMesh_WithValidModelGUIDAndValidIMeshGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnUpdate(pModel->GetMeshes().front())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(pBatch->UpdateIMesh(pModel->GetGUID(), pModel->GetMeshes().front()->GetGUID()));
}

TEST_F(FilledAssetBatchTest, UpdateIMesh_WithValidModelGUIDAndInvalidIMeshGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnUpdate(pModel->GetMeshes().front())).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(pBatch->UpdateIMesh(pModel->GetGUID(), Asset::INVALID_GUID), std::invalid_argument);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByGUID_WithValidGUID) {
    EXPECT_EQ(pBatch->GetMaterial(MaterialGUID), pMaterial);
    EXPECT_EQ(pBatch->GetModel(ModelGUID),       pModel);
    EXPECT_EQ(pBatch->GetSprite(SpriteGUID),     pSprite);
    EXPECT_EQ(pBatch->GetText(TextGUID),         pText);
    EXPECT_EQ(pBatch->GetOutline(OutlineGUID),   pOutline);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByName_WithValidName) {
    EXPECT_EQ(pBatch->GetMaterial(MaterialName), pMaterial);
    EXPECT_EQ(pBatch->GetModel(ModelName),       pModel);
    EXPECT_EQ(pBatch->GetSprite(SpriteName),     pSprite);
    EXPECT_EQ(pBatch->GetText(TextName),         pText);
    EXPECT_EQ(pBatch->GetOutline(OutlineName),   pOutline);
}

TEST_F(FilledAssetBatchTest, GetStats) {
    EXPECT_EQ(pBatch->GetNumMaterials(), 1);
    EXPECT_EQ(pBatch->GetNumModels(),    1);
    EXPECT_EQ(pBatch->GetNumMeshes(),    1);
    EXPECT_EQ(pBatch->GetNumSubmeshes(), 1);
    EXPECT_EQ(pBatch->GetNumSprites(),   1);
    EXPECT_EQ(pBatch->GetNumTexts(),     1);
    EXPECT_EQ(pBatch->GetNumOutlines(),  1);

    EXPECT_EQ(pBatch->GetNumSubmeshInstances(),         1);
    EXPECT_EQ(pBatch->GetNumRenderedSubmeshInstances(), 1);
    EXPECT_EQ(pBatch->GetNumLoadedVertices(),           24);
    EXPECT_EQ(pBatch->GetNumRenderedVertices(),         24);
}

