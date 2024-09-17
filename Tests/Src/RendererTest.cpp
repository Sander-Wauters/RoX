#include <gtest/gtest.h>

#include "RoX/Renderer.h"
#include "RoX/Window.h"

class RendererTest : public testing::Test {
    protected: 
        RendererTest() {
            pWindow = std::make_unique<Window>(L"RendererTest", nullptr, NULL);
            pRenderer = std::make_unique<Renderer>(*pWindow);
        }

        std::unique_ptr<Renderer> pRenderer;
        std::unique_ptr<Window> pWindow;
};
