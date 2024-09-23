#include <gmock/gmock.h>

#include <RoX/Model.h>

class MockMeshObserver : public IMeshObserver {
    public:
        MOCK_METHOD(void, OnAddMock,       (Submesh* pSubmesh));

        MOCK_METHOD(void, OnRemoveSubmesh, (std::uint8_t index),                       (override));

        void OnAdd(const std::unique_ptr<Submesh>& pSubmesh) override {
            OnAddMock(pSubmesh.get()); 
        }
};
