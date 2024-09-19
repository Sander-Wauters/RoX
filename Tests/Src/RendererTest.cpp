#include <gtest/gtest.h>

#include "PredefinedObjects/ValidAssetBatch.h"

#include "RoX/Scene.h"
#include "RoX/Camera.h"
#include "RoX/Renderer.h"
#include "RoX/Window.h"


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
            g_pWindow->DeregisterWindowObserver(pRenderer.get());
        }

        std::unique_ptr<Renderer> pRenderer;
        std::unique_ptr<Camera> pCamera;
        std::unique_ptr<Scene> pScene;
};

TEST_F(RendererTest, Load_Fresh_WithEmptyScene) {
    pScene = std::make_unique<Scene>("RendererTest", *pCamera);

    ASSERT_NO_THROW(pRenderer->Load(*pScene));

    ASSERT_NO_THROW(pRenderer->Update());
    ASSERT_NO_THROW(pRenderer->Render());

    ASSERT_NO_THROW(pRenderer->OnActivated());
    ASSERT_NO_THROW(pRenderer->OnDeactivated());
    ASSERT_NO_THROW(pRenderer->OnSuspending());
    ASSERT_NO_THROW(pRenderer->OnResuming());
    ASSERT_NO_THROW(pRenderer->OnWindowMoved());
    ASSERT_NO_THROW(pRenderer->OnDisplayChanged());
    ASSERT_NO_THROW(pRenderer->OnWindowSizeChanged(g_pWindow->GetWidth(), g_pWindow->GetHeight()));
}

TEST_F(RendererTest, Load_Fresh_WithOccupiedScene) {
    ASSERT_NO_THROW(pRenderer->Load(*pScene));

    ASSERT_NO_THROW(pRenderer->Update());
    ASSERT_NO_THROW(pRenderer->Render());

    ASSERT_NO_THROW(pRenderer->OnActivated());
    ASSERT_NO_THROW(pRenderer->OnDeactivated());
    ASSERT_NO_THROW(pRenderer->OnSuspending());
    ASSERT_NO_THROW(pRenderer->OnResuming());
    ASSERT_NO_THROW(pRenderer->OnWindowMoved());
    ASSERT_NO_THROW(pRenderer->OnDisplayChanged());
    ASSERT_NO_THROW(pRenderer->OnWindowSizeChanged(g_pWindow->GetWidth(), g_pWindow->GetHeight()));
}

