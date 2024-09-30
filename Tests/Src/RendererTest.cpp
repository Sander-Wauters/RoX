#include <gtest/gtest.h>

#include "PredefinedObjects/ValidAssetBatch.h"

#include "RoX/Scene.h"
#include "RoX/Camera.h"
#include "RoX/Renderer.h"
#include "RoX/Window.h"
#include "RoX/DXException.h"

std::unique_ptr<Window> g_pWindow = std::make_unique<Window>(L"RendererTest", GetModuleHandle(NULL), NULL);

class RendererTest : public testing::Test, public ValidAssetBatch {
    protected: 
        RendererTest() {
            pRenderer = std::make_unique<Renderer>(*g_pWindow);

            pCamera = std::make_unique<Camera>();
            pScene = std::make_unique<Scene>("RendererTest", *pCamera);

            pBatch->Add(pMaterial);
            pBatch->Add(pModel);
            pBatch->Add(pSprite);
            pBatch->Add(pText);
            pBatch->Add(pOutline);

            pScene->Add(pBatch);
        }

        ~RendererTest() {
            g_pWindow->Detach(pRenderer.get());
            pRenderer.reset();
        }

        std::unique_ptr<Renderer> pRenderer;
        std::unique_ptr<Camera> pCamera;
        std::unique_ptr<Scene> pScene;
};

testing::AssertionResult SimulateMainLoop(Renderer& renderer, std::uint8_t count = 2) {
    try {
        for (std::uint8_t i = 0; i < count; ++i) {
            renderer.Update();
            renderer.Render();

            renderer.OnActivated();
            renderer.OnDeactivated();
            renderer.OnSuspending();
            renderer.OnResuming();
            renderer.OnWindowMoved();
            renderer.OnDisplayChanged();
            renderer.OnWindowSizeChanged(g_pWindow->GetWidth(), g_pWindow->GetHeight());
        }
    } catch(DXException ex) {
        return testing::AssertionFailure() << ex.ToString();
    } catch(std::exception ex) {
        return testing::AssertionFailure() << ex.what();
    }
    return testing::AssertionSuccess();
}

// ---------------------------------------------------------------- //
//                          Renderer
// ---------------------------------------------------------------- //

TEST_F(RendererTest, PreLoad_SimulateMainLoop) {
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PreLoad_ForceDeviceReset) {
    ASSERT_NO_THROW(pRenderer->ForceDeviceReset());
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PreLoad_ToggleMSAA_StartOn) {
    ASSERT_NO_THROW(pRenderer->SetMsaa(true));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->SetMsaa(false));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PreLoad_ToggleMSAA_StartOff) {
    ASSERT_NO_THROW(pRenderer->SetMsaa(false));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->SetMsaa(true));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, Load_Fresh_WithEmptyScene) {
    pScene = std::make_unique<Scene>("RendererTest", *pCamera);

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, Load_Fresh_WithOccupiedScene) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, Load_Dirty_WithEmptyScene) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, Load_Dirty_WithOccupiedScene) {
    auto pNewScene = std::make_unique<Scene>("RendererTest", *pCamera);

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->Load(*pNewScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_ForceDeviceReset) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));

    ASSERT_NO_THROW(pRenderer->ForceDeviceReset());
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_ToggleMSAA_StartOn) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));

    ASSERT_NO_THROW(pRenderer->SetMsaa(true));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->SetMsaa(false));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_ToggleMSAA_StartOff) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));

    ASSERT_NO_THROW(pRenderer->SetMsaa(false));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pRenderer->SetMsaa(true));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

// ---------------------------------------------------------------- //
//                          AssetBatch
// ---------------------------------------------------------------- //

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewMaterial_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);
    pScene->GetAssetBatches()[0] = pBatch;

    auto pNewMaterial1 = NewValidMaterial();
    auto pNewMaterial2 = std::make_shared<Material>(L"unique_texture_1", L"unique_texture_2");

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Add(pNewMaterial1));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(pNewMaterial2), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewSprite_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);
    pScene->GetAssetBatches()[0] = pBatch;

    auto pNewSprite1 = NewValidSprite();
    auto pNewSprite2 = std::make_shared<Sprite>(L"unique_texture_2");

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Add(pNewSprite1));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(pNewSprite2), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewText_NumUniqueTexturesExceedsMaxNumUniqueTextures) {
    pBatch = std::make_shared<AssetBatch>("ValidAssetBatch", 1, true);
    pScene->GetAssetBatches()[0] = pBatch;

    auto pNewText1 = NewValidText();
    auto pNewText2 = std::make_shared<Text>(L"unique_texture_2", L"");

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Add(pNewText1));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(pNewText2), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewMaterial) {
    auto pNewMaterial = NewValidMaterial();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewMaterial));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithExistingMaterial) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pMaterial));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithInvalidMaterial) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Material>()), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithExistingModel) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pModel));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewModelExistingMaterial) {
    auto pNewModel = NewValidModelWithExistingMaterial();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewModel));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewModelNewMaterial) {
    auto pNewModel = NewValidModelWithNewValidMaterial();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewModel));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewModelWithInvalidMaterial) {
    auto pNewModel = NewValidModelWithNewValidMaterial();
    pNewModel->GetMaterials() = { nullptr };

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(pNewModel), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithInvalidModel) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Model>()), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pModelWithoutGeo = std::make_shared<Model>(pMaterial);
    EXPECT_THROW(pBatch->Add(pModelWithoutGeo), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewSprite) {
    auto pNewSprite = NewValidSprite();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewSprite));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithExistingSprite) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pSprite));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithInvalidSprite) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Sprite>()), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewText) {
    auto pNewText = NewValidText();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewText));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithExistingText) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pText));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithInvalidText) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Text>()), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithNewOutline) {
    auto pNewOutline = NewValidOutline();

    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pNewOutline));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithExistingOutline) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    ASSERT_NO_THROW(pBatch->Add(pOutline));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Add_WithInvalidOutline) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Add(std::shared_ptr<Outline>()), std::invalid_argument);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByGUID_WithValidGUID) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->RemoveMaterial(MaterialGUID), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveModel(ModelGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->RemoveMaterial(MaterialGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveSprite(SpriteGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveText(TextGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveOutline(OutlineGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByGUID_WithInvalidGUID) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveMaterial(Asset::INVALID_GUID), std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveModel(Asset::INVALID_GUID),    std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveSprite(Asset::INVALID_GUID),   std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveText(Asset::INVALID_GUID),     std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveOutline(Asset::INVALID_GUID),  std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByTypeAndGUID_WithValidGUID) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialGUID), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
    
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineGUID));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByTypeAndGUID_WithInvalidGUID) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, Asset::INVALID_GUID), std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, Asset::INVALID_GUID),    std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, Asset::INVALID_GUID),   std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, Asset::INVALID_GUID),     std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, Asset::INVALID_GUID),  std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByName_WithValidName) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->RemoveMaterial(MaterialName), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveModel(ModelName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->RemoveMaterial(MaterialName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveSprite(SpriteName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveText(TextName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->RemoveOutline(OutlineName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByName_WithInvalidName) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveMaterial(INVALID_NAME), std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveModel(INVALID_NAME),    std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveSprite(INVALID_NAME),   std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveText(INVALID_NAME),     std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->RemoveOutline(INVALID_NAME),  std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByTypeAndName_WithValidName) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material still in use by pModel
    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialName), std::runtime_error);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Model, ModelName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    // Material no longer in use by pModel
    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Material, MaterialName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, SpriteName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Text, TextName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, OutlineName));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_AssetBatch_Remove_ByTypeAndName_WithInvalidName) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Material, INVALID_NAME), std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Model, INVALID_NAME),    std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Sprite, INVALID_NAME),   std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Text, INVALID_NAME),     std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pBatch->Remove(AssetBatch::AssetType::Outline, INVALID_NAME),  std::out_of_range);
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

// ---------------------------------------------------------------- //
//                          Mesh
// ---------------------------------------------------------------- //

TEST_F(RendererTest, PostFreshLoad_Mesh_Add_WithNewSubmesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewSubmesh = NewValidSubmesh();

    EXPECT_NO_THROW(pMesh->Add(std::move(pNewSubmesh)));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Mesh_Add_WithInvalidSubmesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pMesh->Add(std::unique_ptr<Submesh>()), std::invalid_argument);
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Mesh_RemoveSubmesh_WithValidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    pMesh->Add(NewValidSubmesh());
    EXPECT_NO_THROW(pMesh->RemoveSubmesh(0));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Mesh_RemoveSubmesh_WithInvalidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pMesh->RemoveSubmesh(-1));
    EXPECT_EQ(pMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_SkinnedMesh_Add_WithNewSubmesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewSubmesh = NewValidSubmesh();

    EXPECT_NO_THROW(pSkinnedMesh->Add(std::move(pNewSubmesh)));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_SkinnedMesh_Add_WithInvalidSubmesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pSkinnedMesh->Add(std::unique_ptr<Submesh>()), std::invalid_argument);
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_SkinnedMesh_RemoveSubmesh_WithValidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    pSkinnedMesh->Add(NewValidSubmesh());

    EXPECT_NO_THROW(pSkinnedMesh->RemoveSubmesh(0));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_SkinnedMesh_RemoveSubmesh_WithInvalidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_NO_THROW(pSkinnedMesh->RemoveSubmesh(-1));
    EXPECT_EQ(pSkinnedMesh->GetNumSubmeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

// ---------------------------------------------------------------- //
//                          Model
// ---------------------------------------------------------------- //

TEST_F(RendererTest, PostFreshLoad_Model_Add_WithNewMaterial) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewMaterial = NewValidMaterial();
    EXPECT_NO_THROW(pModel->Add(pNewMaterial));
    EXPECT_EQ(pModel->GetNumMaterials(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_Add_WithInvalidMaterial) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pModel->Add(std::shared_ptr<Material>()), std::invalid_argument);
    EXPECT_EQ(pModel->GetNumMaterials(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_Add_WithNewMesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    std::shared_ptr<IMesh> pNewMesh = NewValidMesh();
    EXPECT_NO_THROW(pModel->Add(pNewMesh));
    EXPECT_EQ(pModel->GetNumMeshes(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_Add_WithInvalidMesh) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    EXPECT_THROW(pModel->Add(std::shared_ptr<Mesh>()), std::invalid_argument);
    EXPECT_EQ(pModel->GetNumMeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_RemoveMaterial_WithValidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewMaterial = NewValidMaterial();

    ASSERT_NO_THROW(pModel->Add(pNewMaterial));

    EXPECT_NO_THROW(pModel->RemoveMaterial(1));
    EXPECT_EQ(pModel->GetNumMaterials(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_RemoveMaterial_WithInalidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewMaterial = NewValidMaterial();
    ASSERT_NO_THROW(pModel->Add(pNewMaterial));

    EXPECT_NO_THROW(pModel->RemoveMaterial(-1));
    EXPECT_EQ(pModel->GetNumMaterials(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_RemoveIMesh_WithValidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewMesh = NewValidMesh();
    ASSERT_NO_THROW(pModel->Add(pNewMesh));

    EXPECT_NO_THROW(pModel->RemoveIMesh(1));
    EXPECT_EQ(pModel->GetNumMeshes(), 1);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

TEST_F(RendererTest, PostFreshLoad_Model_RemoveIMesh_WithInalidIndex) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));
    ASSERT_TRUE(SimulateMainLoop(*pRenderer));

    auto pNewMesh = NewValidMesh();
    ASSERT_NO_THROW(pModel->Add(pNewMesh));

    EXPECT_NO_THROW(pModel->RemoveIMesh(-1));
    EXPECT_EQ(pModel->GetNumMeshes(), 2);

    ASSERT_TRUE(SimulateMainLoop(*pRenderer));
}

