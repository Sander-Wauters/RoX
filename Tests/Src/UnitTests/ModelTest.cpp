#include <gtest/gtest.h>

#include "../Mocks/MockModelObserver.h"
#include "../PredefinedObjects/ValidModel.h"

class ModelTest : public testing::Test, public ValidModel {
    protected:
        ModelTest() {

        }
};

TEST_F(ModelTest, UseStaticBuffers) {
    EXPECT_NO_THROW(pModel->UseStaticBuffers(true));
    EXPECT_EQ(pModel->IsUsingStaticBuffers(), true);
    for (auto& pMesh : pModel->GetMeshes()) {
        EXPECT_TRUE(pMesh->IsUsingStaticBuffers());
    }

    EXPECT_NO_THROW(pModel->UseStaticBuffers(false));
    EXPECT_EQ(pModel->IsUsingStaticBuffers(), false);
    for (auto& pMesh : pModel->GetMeshes()) {
        EXPECT_FALSE(pMesh->IsUsingStaticBuffers());
    }
}

TEST_F(ModelTest, Add_WithNewMaterial) {
    auto pNewMaterial = NewValidMaterial();

    MockModelObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewMaterial)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->Add(pNewMaterial));
    EXPECT_EQ(pModel->GetNumMaterials(), 2);

}

TEST_F(ModelTest, Add_WithInvalidMaterial) {
    MockModelObserver observer;
    EXPECT_CALL(observer, OnAdd(std::shared_ptr<Material>())).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_THROW(pModel->Add(std::shared_ptr<Material>()), std::invalid_argument);
    EXPECT_EQ(pModel->GetNumMaterials(), 1);
}

TEST_F(ModelTest, Add_WithNewMesh) {
    std::shared_ptr<IMesh> pNewMesh = NewValidMesh();

    MockModelObserver observer;
    EXPECT_CALL(observer, OnAdd(pNewMesh)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->Add(pNewMesh));
    EXPECT_EQ(pModel->GetNumMeshes(), 2);
}

TEST_F(ModelTest, Add_WithInvalidMesh) {
    MockModelObserver observer;
    EXPECT_CALL(observer, OnAdd(std::shared_ptr<IMesh>())).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_THROW(pModel->Add(std::shared_ptr<Mesh>()), std::invalid_argument);
    EXPECT_EQ(pModel->GetNumMeshes(), 1);
}

TEST_F(ModelTest, ClearGeometry) {
    EXPECT_NO_THROW(pModel->ClearGeometry());
    for (auto& pMesh : pModel->GetMeshes()) {
        EXPECT_EQ(pMesh->GetNumVertices(), 0);
        EXPECT_EQ(pMesh->GetNumIndices(), 0);
    }
}

TEST_F(ModelTest, RemoveMaterial_WithValidIndex) {
    auto pNewMaterial = NewValidMaterial();
    ASSERT_NO_THROW(pModel->Add(pNewMaterial));

    MockModelObserver observer;
    EXPECT_CALL(observer, OnRemoveMaterial(1)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->RemoveMaterial(1));
    EXPECT_EQ(pModel->GetNumMaterials(), 1);
}

TEST_F(ModelTest, RemoveMaterial_WithInalidIndex) {
    auto pNewMaterial = NewValidMaterial();
    ASSERT_NO_THROW(pModel->Add(pNewMaterial));

    MockModelObserver observer;
    EXPECT_CALL(observer, OnRemoveMaterial(-1)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->RemoveMaterial(-1));
    EXPECT_EQ(pModel->GetNumMaterials(), 2);
}

TEST_F(ModelTest, RemoveIMesh_WithValidIndex) {
    auto pNewMesh = NewValidMesh();
    ASSERT_NO_THROW(pModel->Add(pNewMesh));

    MockModelObserver observer;
    EXPECT_CALL(observer, OnRemoveIMesh(1)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->RemoveIMesh(1));
    EXPECT_EQ(pModel->GetNumMeshes(), 1);
}

TEST_F(ModelTest, RemoveIMesh_WithInalidIndex) {
    auto pNewMesh = NewValidMesh();
    ASSERT_NO_THROW(pModel->Add(pNewMesh));

    MockModelObserver observer;
    EXPECT_CALL(observer, OnRemoveIMesh(-1)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->RemoveIMesh(-1));
    EXPECT_EQ(pModel->GetNumMeshes(), 2);
}

TEST_F(ModelTest, Attach_WithValidObserver) {
    MockModelObserver observer;
    EXPECT_NO_THROW(pModel->Attach(&observer));
}

TEST_F(ModelTest, Attach_WithInvalidObserver) {
    EXPECT_THROW(pModel->Attach(nullptr), std::invalid_argument);
}

TEST_F(ModelTest, Detach_WithValidObserver) {
    MockModelObserver observer;
    ASSERT_NO_THROW(pModel->Attach(&observer));

    EXPECT_NO_THROW(pModel->Detach(&observer));
}

TEST_F(ModelTest, Detach_WithInvalidObserver) {
    EXPECT_NO_THROW(pModel->Detach(nullptr));
}

