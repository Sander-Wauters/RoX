#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "PredefinedObjects/ValidAssetBatch.h"

class MockAssetBatchObserver : public IAssetBatchObserver {
    public:
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Material>& pMaterial), (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Model>& pModel),       (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Sprite>& pSprite),     (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Text>& pText),         (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Outline>& pOutline),   (override));

        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Material>& pMaterial), (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Model>& pModel),       (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Sprite>& pSprite),     (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Text>& pText),         (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Outline>& pOutline),   (override));
};

class GeneralAssetBatchTest : public testing::Test {
    protected:
        GeneralAssetBatchTest() {}

        ValidAssetBatch validBatch;
};

class EmptyAssetBatchTest : public testing::Test {
    protected:
        EmptyAssetBatchTest() {}

        ValidAssetBatch validBatch;
};

class FilledAssetBatchTest : public testing::Test {
    protected:
        FilledAssetBatchTest() {
            validBatch.pBatch->Add(validBatch.pMaterial);
            validBatch.pBatch->Add(validBatch.pModel1);
            validBatch.pBatch->Add(validBatch.pSprite);
            validBatch.pBatch->Add(validBatch.pText);
            validBatch.pBatch->Add(validBatch.pOutline);
        }

        ValidAssetBatch validBatch;
};

TEST_F(GeneralAssetBatchTest, RegisterAssetBatchObserver_WithValidObserver) {
    MockAssetBatchObserver observer;
    EXPECT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));
}

TEST_F(GeneralAssetBatchTest, RegisterAssetBatchObserver_WithInvalidObserver) {
    EXPECT_THROW(validBatch.pBatch->RegisterAssetBatchObserver(nullptr), std::invalid_argument);
}

TEST_F(GeneralAssetBatchTest, DeregisterAssetBatchObserver_WithValidObserver) {
    MockAssetBatchObserver observer;
    EXPECT_NO_THROW(validBatch.pBatch->DeregisterAssetBatchObserver(&observer));
}

TEST_F(GeneralAssetBatchTest, DeregisterAssetBatchObserver_WithInvalidObserver) {
    EXPECT_NO_THROW(validBatch.pBatch->DeregisterAssetBatchObserver(nullptr));
}

TEST_F(EmptyAssetBatchTest, FindGUID_ByName_WithValidName) {
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.MaterialName, validBatch.pBatch->GetMaterials()), Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.Model1Name,   validBatch.pBatch->GetModels()),    Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.SpriteName,   validBatch.pBatch->GetSprites()),   Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.TextName,     validBatch.pBatch->GetTexts()),     Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.OutlineName,  validBatch.pBatch->GetOutlines()),  Asset::INVALID_GUID);
}

TEST_F(EmptyAssetBatchTest, FindGUID_ByName_WithInvalidName) {
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetMaterials()), Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetModels()),    Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetSprites()),   Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetTexts()),     Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetOutlines()),  Asset::INVALID_GUID);
}

TEST_F(EmptyAssetBatchTest, Add_WithValidAsset) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pMaterial));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pModel1));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pSprite));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pText));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pOutline));
}

TEST_F(EmptyAssetBatchTest, Add_WithInvalidAsset) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Material>()), std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Model>()),    std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Sprite>()),   std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Text>()),     std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Outline>()),  std::invalid_argument);
}

TEST_F(EmptyAssetBatchTest, Remove_ByGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialGUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveModel(validBatch.Model1GUID),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveSprite(validBatch.SpriteGUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveText(validBatch.TextGUID),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialGUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.Model1GUID),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.SpriteGUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.TextGUID),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveModel(Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveSprite(Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveText(Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveOutline(Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialName), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveModel(validBatch.Model1Name),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveSprite(validBatch.SpriteName),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveText(validBatch.TextName),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveOutline(validBatch.OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialName), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.Model1Name),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.SpriteName),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.TextName),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveModel(validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveSprite(validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveText(validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->RemoveOutline(validBatch.INVALID_NAME),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, Remove_ByTypeAndName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.INVALID_NAME),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByGUID_WithValidGUID) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1GUID),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextGUID),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByGUID_WithInvalidGUID) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByName_WithValidName) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialName), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1Name),      std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteName),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextName),         std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineName),   std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetAsset_ByName_WithInvalidName) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.INVALID_NAME),  std::out_of_range);
}

TEST_F(EmptyAssetBatchTest, GetStats) {
    EXPECT_EQ(validBatch.pBatch->GetNumMaterials(), 0);
    EXPECT_EQ(validBatch.pBatch->GetNumModels(),    0);
    EXPECT_EQ(validBatch.pBatch->GetNumMeshes(),    0);
    EXPECT_EQ(validBatch.pBatch->GetNumSubmeshes(), 0);
    EXPECT_EQ(validBatch.pBatch->GetNumSprites(),   0);
    EXPECT_EQ(validBatch.pBatch->GetNumTexts(),     0);
    EXPECT_EQ(validBatch.pBatch->GetNumOutlines(),  0);

    EXPECT_EQ(validBatch.pBatch->GetNumSubmeshInstances(),         0);
    EXPECT_EQ(validBatch.pBatch->GetNumRenderedSubmeshInstances(), 0);
    EXPECT_EQ(validBatch.pBatch->GetNumLoadedVertices(),           0);
    EXPECT_EQ(validBatch.pBatch->GetNumRenderedVertices(),         0);
}

TEST_F(FilledAssetBatchTest, FindGUID_ByName_WithValidName) {
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.MaterialName, validBatch.pBatch->GetMaterials()), validBatch.MaterialGUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.Model1Name,   validBatch.pBatch->GetModels()),    validBatch.Model1GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.SpriteName,   validBatch.pBatch->GetSprites()),   validBatch.SpriteGUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.TextName,     validBatch.pBatch->GetTexts()),     validBatch.TextGUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.OutlineName,  validBatch.pBatch->GetOutlines()),  validBatch.OutlineGUID);
}

TEST_F(FilledAssetBatchTest, FindGUID_ByName_WithInvalidName) {
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetMaterials()), Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetModels()),    Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetSprites()),   Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetTexts()),     Asset::INVALID_GUID);
    EXPECT_EQ(validBatch.pBatch->FindGUID(validBatch.INVALID_NAME, validBatch.pBatch->GetOutlines()),  Asset::INVALID_GUID);
}

TEST_F(FilledAssetBatchTest, Add_WithValidAsset) {
    // Asset batch already contains these assets so OnAdd should be skipped.
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pMaterial));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pModel1));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pSprite));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pText));
    EXPECT_NO_THROW(validBatch.pBatch->Add(validBatch.pOutline));
}

TEST_F(FilledAssetBatchTest, Add_WithInValidAsset) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Material>()), std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Model>()),    std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Sprite>()),   std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Text>()),     std::invalid_argument);
    EXPECT_THROW(validBatch.pBatch->Add(std::shared_ptr<Outline>()),  std::invalid_argument);
}

TEST_F(FilledAssetBatchTest, Remove_ByGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(1));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel1
    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialGUID), std::runtime_error);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveModel(validBatch.Model1GUID));
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1GUID),      std::out_of_range);

    // Material no longer in use by pModel1
    EXPECT_NO_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialGUID));
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveSprite(validBatch.SpriteGUID));
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveText(validBatch.TextGUID));
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextGUID),         std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveOutline(validBatch.OutlineGUID));
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndGUID_WithValidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(1));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel1
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialGUID), std::runtime_error);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.Model1GUID));
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1GUID),      std::out_of_range);

    // Material no longer in use by pModel1
    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialGUID));
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.SpriteGUID));
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.TextGUID));
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextGUID),         std::out_of_range);
    
    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.OutlineGUID));
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), validBatch.pMaterial);

    EXPECT_THROW(validBatch.pBatch->RemoveModel(Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1GUID), validBatch.pModel1);

    EXPECT_THROW(validBatch.pBatch->RemoveSprite(Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteGUID), validBatch.pSprite);

    EXPECT_THROW(validBatch.pBatch->RemoveText(Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextGUID), validBatch.pText);

    EXPECT_THROW(validBatch.pBatch->RemoveOutline(Asset::INVALID_GUID),  std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineGUID), validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndGUID_WithInvalidGUID) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, Asset::INVALID_GUID), std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), validBatch.pMaterial);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1GUID), validBatch.pModel1);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteGUID), validBatch.pSprite);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextGUID), validBatch.pText);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, Asset::INVALID_GUID),  std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineGUID), validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, Remove_ByName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(1));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel1
    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialName), std::runtime_error);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveModel(validBatch.Model1Name));
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1GUID),      std::out_of_range);

    // Material no longer in use by pModel1
    EXPECT_NO_THROW(validBatch.pBatch->RemoveMaterial(validBatch.MaterialName));
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveSprite(validBatch.SpriteName));
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveText(validBatch.TextName));
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextGUID),         std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->RemoveOutline(validBatch.OutlineName));
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndName_WithValidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(1));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(1));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    // Material still in use by pModel1
    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialName), std::runtime_error);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.Model1Name));
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.Model1GUID),      std::out_of_range);

    // Material no longer in use by pModel1
    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.MaterialName));
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.SpriteName));
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.TextName));
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.TextGUID),         std::out_of_range);

    EXPECT_NO_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.OutlineName));
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   std::out_of_range);
}

TEST_F(FilledAssetBatchTest, Remove_ByName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->RemoveMaterial(validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), validBatch.pMaterial);

    EXPECT_THROW(validBatch.pBatch->RemoveModel(validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1GUID), validBatch.pModel1);

    EXPECT_THROW(validBatch.pBatch->RemoveSprite(validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteGUID), validBatch.pSprite);

    EXPECT_THROW(validBatch.pBatch->RemoveText(validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextGUID), validBatch.pText);

    EXPECT_THROW(validBatch.pBatch->RemoveOutline(validBatch.INVALID_NAME),  std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineGUID), validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, Remove_ByTypeAndName_WithInvalidName) {
    MockAssetBatchObserver observer;
    EXPECT_CALL(observer, OnAdd(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnAdd(validBatch.pOutline) ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pMaterial)).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pModel1)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pSprite)  ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pText)    ).Times(::testing::Exactly(0));
    EXPECT_CALL(observer, OnRemove(validBatch.pOutline) ).Times(::testing::Exactly(0));
    ASSERT_NO_THROW(validBatch.pBatch->RegisterAssetBatchObserver(&observer));

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Material, validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), validBatch.pMaterial);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Model, validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1GUID), validBatch.pModel1);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Sprite, validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteGUID), validBatch.pSprite);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Text, validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextGUID), validBatch.pText);

    EXPECT_THROW(validBatch.pBatch->Remove(AssetBatch::AssetType::Outline, validBatch.INVALID_NAME),  std::out_of_range);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineGUID), validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByGUID_WithValidGUID) {
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialGUID), validBatch.pMaterial);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1GUID),      validBatch.pModel1);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteGUID),     validBatch.pSprite);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextGUID),         validBatch.pText);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineGUID),   validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByGUID_WithInvalidGUID) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(Asset::INVALID_GUID), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(Asset::INVALID_GUID),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(Asset::INVALID_GUID),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(Asset::INVALID_GUID),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(Asset::INVALID_GUID),  std::out_of_range);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByName_WithValidName) {
    EXPECT_EQ(validBatch.pBatch->GetMaterial(validBatch.MaterialName), validBatch.pMaterial);
    EXPECT_EQ(validBatch.pBatch->GetModel(validBatch.Model1Name),      validBatch.pModel1);
    EXPECT_EQ(validBatch.pBatch->GetSprite(validBatch.SpriteName),     validBatch.pSprite);
    EXPECT_EQ(validBatch.pBatch->GetText(validBatch.TextName),         validBatch.pText);
    EXPECT_EQ(validBatch.pBatch->GetOutline(validBatch.OutlineName),   validBatch.pOutline);
}

TEST_F(FilledAssetBatchTest, GetAsset_ByName_WithInvalidName) {
    EXPECT_THROW(validBatch.pBatch->GetMaterial(validBatch.INVALID_NAME), std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetModel(validBatch.INVALID_NAME),    std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetSprite(validBatch.INVALID_NAME),   std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetText(validBatch.INVALID_NAME),     std::out_of_range);
    EXPECT_THROW(validBatch.pBatch->GetOutline(validBatch.INVALID_NAME),  std::out_of_range);
}

TEST_F(FilledAssetBatchTest, GetStats) {
    EXPECT_EQ(validBatch.pBatch->GetNumMaterials(), 1);
    EXPECT_EQ(validBatch.pBatch->GetNumModels(),    1);
    EXPECT_EQ(validBatch.pBatch->GetNumMeshes(),    1);
    EXPECT_EQ(validBatch.pBatch->GetNumSubmeshes(), 1);
    EXPECT_EQ(validBatch.pBatch->GetNumSprites(),   1);
    EXPECT_EQ(validBatch.pBatch->GetNumTexts(),     1);
    EXPECT_EQ(validBatch.pBatch->GetNumOutlines(),  1);

    EXPECT_EQ(validBatch.pBatch->GetNumSubmeshInstances(),         1);
    EXPECT_EQ(validBatch.pBatch->GetNumRenderedSubmeshInstances(), 1);
    EXPECT_EQ(validBatch.pBatch->GetNumLoadedVertices(),           0);
    EXPECT_EQ(validBatch.pBatch->GetNumRenderedVertices(),         0);
}

