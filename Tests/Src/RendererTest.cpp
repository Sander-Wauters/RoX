#include <gtest/gtest.h>

#include "RoX/Renderer.h"
#include "RoX/Window.h"

class RendererTest : public testing::Test {
    protected: 
        RendererTest() {
            pRenderer = std::make_unique<Renderer>();
            pWindow = std::make_unique<Window>(*pRenderer, L"RendererTest", nullptr, NULL);
        }

        std::unique_ptr<Renderer> pRenderer;
        std::unique_ptr<Window> pWindow;
};

TEST_F(RendererTest, Initialize) {
    EXPECT_NO_THROW(pRenderer->Initialize(pWindow->GetHwnd(), pWindow->GetWidth(), pWindow->GetHeight()));
}
