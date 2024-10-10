#include <gtest/gtest.h>

#include <filesystem>

#include <RoX/AssetIO.h>

#include "../PredefinedObjects/ValidModel.h"

class AssetIOTest : public testing::Test, public ValidModel {
    protected:
        AssetIOTest() {

        }

        ~AssetIOTest() {
            std::filesystem::remove(pModel->GetName() + ".roxmodl");
        }
};

TEST_F(AssetIOTest, ExportRoXModl_WithValidModel) {
    EXPECT_NO_THROW(AssetIO::ExportRoXModl(pModel, pModel->GetName() + ".roxmodl"));
}

TEST_F(AssetIOTest, ImportRoXModl_WithModelExportedWithExportRoXModl) {
    EXPECT_NO_THROW(AssetIO::ExportRoXModl(pModel, pModel->GetName() + ".roxmodl"));

    std::shared_ptr<Model> pImport;
    EXPECT_NO_THROW(pImport = AssetIO::ImportRoXModl(pModel->GetName() + ".roxmodl", pMaterial));

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
