#include <gtest/gtest.h>

#include <filesystem>

#include <RoX/AssetIO.h>

#include "../PredefinedObjects/ValidModel.h"
#include "../PredefinedObjects/ValidAnimation.h"

class AssetIOTest : public testing::Test, public ValidModel,  public ValidAnimation {
    protected:
        static constexpr const char* MODL_NAME = "modl.roxmodl";
        static constexpr const char* ANIM_NAME = "anim.roxanim";

    protected:
        AssetIOTest() {
        }

        ~AssetIOTest() {
            std::filesystem::remove(MODL_NAME);
            std::filesystem::remove(ANIM_NAME);
        }
};

TEST_F(AssetIOTest, ExportRoXModl_WithValidModel) {
    EXPECT_NO_THROW(AssetIO::ExportRoXModl(pModel, MODL_NAME));
}

TEST_F(AssetIOTest, ImportRoXModl_WithValidModel_ExportedWithExportRoXModl) {
    EXPECT_NO_THROW(AssetIO::ExportRoXModl(pModel, MODL_NAME));

    std::shared_ptr<Model> pImport;
    EXPECT_NO_THROW(pImport = AssetIO::ImportRoXModl(MODL_NAME, pMaterial));

    EXPECT_EQ(pImport->GetName(), pModel->GetName());

    ASSERT_EQ(pImport->GetNumBones(), pModel->GetNumBones());
    for (std::uint32_t i = 0; i < pImport->GetNumBones(); ++i) {
        Bone& bone = pModel->GetBones()[i];
        Bone& importBone = pImport->GetBones()[i];

        EXPECT_EQ(bone.GetName(), importBone.GetName());
    }

    ASSERT_EQ(pImport->GetNumMeshes(), pModel->GetNumMeshes());
    for (std::uint32_t i = 0; i < pImport->GetNumMeshes(); ++i) {
        std::shared_ptr<IMesh>& pMesh = pModel->GetMeshes()[i];
        std::shared_ptr<IMesh>& pImportMesh = pImport->GetMeshes()[i];

        EXPECT_EQ(pMesh->GetName(), pImportMesh->GetName());
        ASSERT_EQ(pMesh->GetNumIndices(), pImportMesh->GetNumIndices());
        for (std::uint32_t j = 0; j < pImportMesh->GetNumIndices(); ++j) {
            EXPECT_EQ(pMesh->GetIndices()[j], pImportMesh->GetIndices()[j]);
        }

        ASSERT_EQ(pMesh->GetNumVertices(), pImportMesh->GetNumVertices());

        ASSERT_EQ(pMesh->GetNumSubmeshes(), pImportMesh->GetNumSubmeshes());
        for (std::uint32_t j = 0; j < pImportMesh->GetNumSubmeshes(); ++j) {
            std::unique_ptr<Submesh>& pSubmesh = pMesh->GetSubmeshes()[j];
            std::unique_ptr<Submesh>& pImportSubmesh = pImportMesh->GetSubmeshes()[j];

            EXPECT_EQ(pSubmesh->GetName(), pImportSubmesh->GetName());
        }
    }
}

TEST_F(AssetIOTest, ExportRoXAnim_WithValidAnimation) {
    EXPECT_NO_THROW(AssetIO::ExportRoXAnim(pAnimation, ANIM_NAME));
}

TEST_F(AssetIOTest, ImportRoXAnim_WithValidAnimation_ExportedWithExportRoXAnim) {
    EXPECT_NO_THROW(AssetIO::ExportRoXAnim(pAnimation, ANIM_NAME));

    std::shared_ptr<Animation> pImport;
    EXPECT_NO_THROW(pImport = AssetIO::ImportRoXAnim(ANIM_NAME));

    ASSERT_EQ(pImport->GetNumBoneAnimations(), pAnimation->GetNumBoneAnimations());
    EXPECT_EQ(pImport->GetStartTime(), pAnimation->GetStartTime());
    EXPECT_EQ(pImport->GetEndTime(), pAnimation->GetEndTime());

    for (std::uint32_t i = 0; i < pImport->GetNumBoneAnimations(); ++i) {
        ASSERT_EQ(pImport->GetBoneAnimations()[i].GetNumKeyframes(), pAnimation->GetBoneAnimations()[i].GetNumKeyframes());
        EXPECT_EQ(pImport->GetBoneAnimations()[i].GetStartTime(), pAnimation->GetBoneAnimations()[i].GetStartTime());
        EXPECT_EQ(pImport->GetBoneAnimations()[i].GetEndTime(), pAnimation->GetBoneAnimations()[i].GetEndTime());

        for (std::uint32_t j = 0; j < pImport->GetBoneAnimations()[i].GetNumKeyframes(); ++j) {
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].TimePosition, pAnimation->GetBoneAnimations()[i].Keyframes[j].TimePosition);

            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Translation.x, pAnimation->GetBoneAnimations()[i].Keyframes[j].Translation.x);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Translation.y, pAnimation->GetBoneAnimations()[i].Keyframes[j].Translation.y);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Translation.z, pAnimation->GetBoneAnimations()[i].Keyframes[j].Translation.z);

            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Scale.x, pAnimation->GetBoneAnimations()[i].Keyframes[j].Scale.x);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Scale.y, pAnimation->GetBoneAnimations()[i].Keyframes[j].Scale.y);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].Scale.z, pAnimation->GetBoneAnimations()[i].Keyframes[j].Scale.z);

            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.x, pAnimation->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.x);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.y, pAnimation->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.y);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.z, pAnimation->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.z);
            EXPECT_EQ(pImport->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.w, pAnimation->GetBoneAnimations()[i].Keyframes[j].RotationQuaternion.w);
        }
    }
}

