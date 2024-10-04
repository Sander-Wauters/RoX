#include <gtest/gtest.h>

#include <RoX/Model.h>

#include "../Mocks/MockMeshObserver.h"
#include "../PredefinedObjects/ValidMesh.h"

class BaseMeshTest : public testing::Test, public ValidMesh {
    protected:
        BaseMeshTest() {}
};

class MeshTest : public testing::Test, public ValidMesh {
    protected:
        MeshTest() {}
};

class SkinnedMeshTest : public testing::Test, public ValidMesh {
    protected:
        SkinnedMeshTest() {}
};

// ---------------------------------------------------------------- //
//                          BaseMesh
// ---------------------------------------------------------------- //

TEST_F(BaseMeshTest, UseStaticBuffers) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnUseStaticBuffers(pMesh.get(), true)).Times(testing::Exactly(1));
    EXPECT_CALL(observer, OnUseStaticBuffers(pMesh.get(), false)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->UseStaticBuffers(true));
    EXPECT_TRUE(pMesh->IsUsingStaticBuffers());

    EXPECT_NO_THROW(pMesh->UseStaticBuffers(false));
    EXPECT_FALSE(pMesh->IsUsingStaticBuffers());
}

TEST_F(BaseMeshTest, UpdateBuffers) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnUpdateBuffers(pMesh.get())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->UpdateBuffers());
}

TEST_F(BaseMeshTest, Add_WithNewSubmesh) {
    auto pNewSubmesh = NewValidSubmesh();

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(testing::NotNull())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->Add(std::move(pNewSubmesh)));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 2);
}

TEST_F(BaseMeshTest, Add_WithInvalidSubmesh) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(nullptr)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_THROW(pMesh->Add(std::unique_ptr<Submesh>()), std::invalid_argument);
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(BaseMeshTest, RemoveSubmesh_WithValidIndex) {
    pMesh->Add(NewValidSubmesh());

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(0)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->RemoveSubmesh(0));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(BaseMeshTest, RemoveSubmesh_WithInvalidIndex) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(-1)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->RemoveSubmesh(-1));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(BaseMeshTest, Attach_WithNewObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pMesh->Attach(&observer));
}

TEST_F(BaseMeshTest, Attach_WithInvalidObserver) {
    EXPECT_THROW(pMesh->Attach(nullptr), std::invalid_argument);
}

TEST_F(BaseMeshTest, Detach_WithValidObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->Detach(&observer));
}

TEST_F(BaseMeshTest, Detach_WithInvalidObserver) {
    EXPECT_NO_THROW(pMesh->Detach(nullptr));
}

// ---------------------------------------------------------------- //
//                          Mesh
// ---------------------------------------------------------------- //

TEST_F(MeshTest, ClearGeometry) {
    EXPECT_NO_THROW(pMesh->ClearGeometry());
    EXPECT_EQ(pMesh->GetNumIndices(), 0);
    EXPECT_EQ(pMesh->GetNumVertices(), 0);
}

TEST_F(MeshTest, RebuildFromBuffers) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRebuildFromBuffers(pMesh.get())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->RebuildFromBuffers());
}

// ---------------------------------------------------------------- //
//                          SkinnedMesh
// ---------------------------------------------------------------- //

TEST_F(SkinnedMeshTest, ClearGeometry) {
    EXPECT_NO_THROW(pSkinnedMesh->ClearGeometry());
    EXPECT_EQ(pSkinnedMesh->GetNumIndices(), 0);
    EXPECT_EQ(pSkinnedMesh->GetNumVertices(), 0);
}

TEST_F(SkinnedMeshTest, RebuildFromBuffers) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRebuildFromBuffers(pSkinnedMesh.get())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_NO_THROW(pSkinnedMesh->RebuildFromBuffers());
}

