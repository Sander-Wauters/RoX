#include <gtest/gtest.h>

#include <RoX/Model.h>

#include "MockMeshObserver.h"
#include "PredefinedObjects/ValidMesh.h"

class MeshTest : public testing::Test, public ValidMesh {
    protected:
        MeshTest() {}
};

class SkinnedMeshTest : public testing::Test, public ValidMesh {
    protected:
        SkinnedMeshTest() {}
};

TEST_F(MeshTest, Add_WithNewSubmesh) {
    auto pNewSubmesh = NewValidSubmesh();

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(testing::NotNull())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->Add(std::move(pNewSubmesh)));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 2);
}

TEST_F(MeshTest, Add_WithInvalidSubmesh) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(nullptr)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_THROW(pMesh->Add(std::unique_ptr<Submesh>()), std::invalid_argument);
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(MeshTest, RemoveSubmesh_WithValidIndex) {
    pMesh->Add(NewValidSubmesh());

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(0)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->RemoveSubmesh(0));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(MeshTest, RemoveSubmesh_WithInvalidIndex) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(-1)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->RemoveSubmesh(-1));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);
}

TEST_F(MeshTest, Attach_WithNewObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pMesh->Attach(&observer));
}

TEST_F(MeshTest, Attach_WithInvalidObserver) {
    EXPECT_THROW(pMesh->Attach(nullptr), std::invalid_argument);
}

TEST_F(MeshTest, Detach_WithValidObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pMesh->Attach(&observer));

    EXPECT_NO_THROW(pMesh->Detach(&observer));
}

TEST_F(MeshTest, Detach_WithInvalidObserver) {
    EXPECT_NO_THROW(pMesh->Detach(nullptr));
}

TEST_F(SkinnedMeshTest, Add_WithNewSubmesh) {
    auto pNewSubmesh = NewValidSubmesh();

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(testing::NotNull())).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_NO_THROW(pSkinnedMesh->Add(std::move(pNewSubmesh)));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 2);
}

TEST_F(SkinnedMeshTest, Add_WithInvalidSubmesh) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnAddMock(nullptr)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_THROW(pSkinnedMesh->Add(std::unique_ptr<Submesh>()), std::invalid_argument);
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);
}

TEST_F(SkinnedMeshTest, RemoveSubmesh_WithValidIndex) {
    pSkinnedMesh->Add(NewValidSubmesh());

    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(0)).Times(testing::Exactly(1));
    ASSERT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_NO_THROW(pSkinnedMesh->RemoveSubmesh(0));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);
}

TEST_F(SkinnedMeshTest, RemoveSubmesh_WithInvalidIndex) {
    MockMeshObserver observer;
    EXPECT_CALL(observer, OnRemoveSubmesh(-1)).Times(testing::Exactly(0));
    ASSERT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_NO_THROW(pSkinnedMesh->RemoveSubmesh(-1));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);
}

TEST_F(SkinnedMeshTest, Attach_WithNewObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pSkinnedMesh->Attach(&observer));
}

TEST_F(SkinnedMeshTest, Attach_WithInvalidObserver) {
    EXPECT_THROW(pSkinnedMesh->Attach(nullptr), std::invalid_argument);
}

TEST_F(SkinnedMeshTest, Detach_WithValidObserver) {
    MockMeshObserver observer;
    
    EXPECT_NO_THROW(pSkinnedMesh->Attach(&observer));

    EXPECT_NO_THROW(pSkinnedMesh->Detach(&observer));
}

TEST_F(SkinnedMeshTest, Detach_WithInvalidObserver) {
    EXPECT_NO_THROW(pSkinnedMesh->Detach(nullptr));
}

