#pragma once

#include <unordered_map>

#include "../../DirectXTK12/Inc/GraphicsMemory.h"
#include "../../DirectXTK12/Inc/DescriptorHeap.h"
#include "../../DirectXTK12/Inc/SpriteBatch.h"
#include "../../DirectXTK12/Inc/CommonStates.h"
#include "../../DirectXTK12/Inc/SimpleMath.h"

#include "../Src/Util/pch.h"
#include "../Src/IDeviceNotify.h"
#include "../Src/DeviceResources.h"
#include "../Src/SpriteData.h"
#include "../Src/TextData.h"
#include "Timer.h"
#include "Sprite.h"
#include "Text.h"

class Renderer : public IDeviceNotify {
    public:
        Renderer(Timer& timer) noexcept;
        ~Renderer();

        Renderer(Renderer&&) = default;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public: // Initialization and management.
        void Initialize(HWND window, int width, int height);

        void Update();
        void Render();

        void AddSprite(Sprite* pSprite);
        void AddText(Text* pText);

    public: // Messages.
        void OnDeviceLost() override;
        void OnDeviceRestored() override;
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowMoved();
        void OnDisplayChange();
        void OnWindowSizeChanged(int width, int height);

    private:
        void Clear();

        void RenderSprites();
        void RenderText();

        void CreateDeviceDependentResources();
        void BuildSpriteDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch);

        void CreateWindowSizeDependentResources();
        void BuildSpriteDataSize();

    private: // Application state.
        Timer& m_timer;
        std::unique_ptr<DeviceResources> m_pDeviceResources = nullptr;
        std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory = nullptr;

        std::unique_ptr<DirectX::DescriptorHeap> m_pResourceDescriptors = nullptr;
        std::unique_ptr<DirectX::CommonStates> m_pStates = nullptr;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch = nullptr;

        UINT m_nextSpriteDescriptorHeapIndex = 0;
        std::unordered_map<Sprite*, std::unique_ptr<SpriteData>> m_spriteData = {};
        std::unordered_map<Text*, std::unique_ptr<TextData>> m_textData = {};
};
