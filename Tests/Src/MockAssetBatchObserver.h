#pragma once
#include <gmock/gmock.h>

#include "RoX/AssetBatch.h"

class MockAssetBatchObserver : public IAssetBatchObserver {
    public:
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Material>& pMaterial),    (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Model>& pModel),          (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Sprite>& pSprite),        (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Text>& pText),            (override));
        MOCK_METHOD(void, OnAdd, (const std::shared_ptr<Outline>& pOutline),      (override));

        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Material>& pMaterial), (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Model>& pModel),       (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Sprite>& pSprite),     (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Text>& pText),         (override));
        MOCK_METHOD(void, OnRemove, (const std::shared_ptr<Outline>& pOutline),   (override));
};
