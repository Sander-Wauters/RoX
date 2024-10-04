#pragma once

#include <gmock/gmock.h>

#include <RoX/Model.h>

class MockModelObserver : public IModelObserver {
    public:
        MOCK_METHOD(void, OnAdd,            (const std::shared_ptr<Material>& pMaterial), (override));
        MOCK_METHOD(void, OnAdd,            (const std::shared_ptr<IMesh>& pIMesh),       (override));

        MOCK_METHOD(void, OnRemoveMaterial, (std::uint8_t index),                         (override));
        MOCK_METHOD(void, OnRemoveIMesh,    (std::uint8_t index),                         (override));
};
