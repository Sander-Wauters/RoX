#pragma once

#include <unordered_map>

#include "../../Lib/DirectXTK12/Inc/GraphicsMemory.h"
#include "../../Lib/DirectXTK12/Inc/DescriptorHeap.h"
#include "../../Lib/DirectXTK12/Inc/SpriteBatch.h"
#include "../../Lib/DirectXTK12/Inc/CommonStates.h"
#include "../../Lib/DirectXTK12/Inc/SimpleMath.h"
#include "../../Lib/DirectXTK12/Inc/Effects.h"
#include "../../Lib/DirectXTK12/Inc/Model.h"
#include "../../Lib/DirectXTK12/Inc/PrimitiveBatch.h"
#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"
#include "../../Lib/DirectXTK12/Inc/GeometricPrimitive.h"

#include "../../Src/Util/pch.h"
#include "../../Src/IDeviceNotify.h"
#include "../../Src/DeviceResources.h"
#include "../../Src/ObjectData.h"
#include "Camera.h"
#include "Timer.h"
#include "Sprite.h"
#include "Text.h"
#include "Texture.h"
#include "StaticGeometry.h"

class Renderer : public IDeviceNotify {
    public:
        Renderer(Timer& timer, Camera& camera) noexcept;
        ~Renderer();

        Renderer(Renderer&&) = default;

        Renderer(Renderer const&) = delete;
        Renderer& operator= (Renderer const&) = delete;

    public:
        void Initialize(HWND window, int width, int height);

        void Update();
        void Render();

        void Add(Sprite* pSprite);
        void Add(Text* pText);
        void Add(StaticGeometry::Base* pStaticGeo);

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;
        void OnActivated();
        void OnDeactivated();
        void OnSuspending();
        void OnResuming();
        void OnWindowMoved();
        void OnDisplayChange();
        void OnWindowSizeChanged(int width, int height);

    public:
        void SetMsaa(bool state) noexcept;
        bool MsaaEnabled() const noexcept;

    private:
        void Clear();

        void RenderSprites();
        void RenderText();
        void RenderStaticGeometry();

        void CreateDeviceDependentResources();
        void CreateRenderTargetDependentResources();
        void BuildSpriteDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildTextureDataResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void BuildStaticGeoDataResources(bool instanced, DirectX::RenderTargetState& renderTargetState, DirectX::ResourceUploadBatch& resourceUploadBatch);

        void BuildDebugDisplayResources(DirectX::RenderTargetState& renderTargetState);

        void CreateWindowSizeDependentResources();

    private:
        Timer& m_timer;
        Camera& m_camera;
        
        bool m_msaaEnabled = false;

        std::unique_ptr<DeviceResources> m_pDeviceResources = nullptr;
        std::unique_ptr<DirectX::GraphicsMemory> m_pGraphicsMemory = nullptr;

        UINT m_nextDescriptorHeapIndex = 0;
        std::unique_ptr<DirectX::DescriptorHeap> m_pResourceDescriptors = nullptr;
        std::unique_ptr<DirectX::CommonStates> m_pStates = nullptr;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch = nullptr;

        std::unordered_map<Sprite*, std::unique_ptr<ObjectData::Sprite>> m_spriteData = {};
        std::unordered_map<Text*, std::unique_ptr<ObjectData::Text>> m_textData = {};
        std::unordered_map<StaticGeometry::Base*, std::unique_ptr<ObjectData::StaticGeometry>> m_staticGeoData = {}; 
        std::unordered_map<StaticGeometry::Base*, std::unique_ptr<ObjectData::StaticGeometry>> m_instancedStaticGeoData = {}; 

        std::unordered_map<Texture*, std::unique_ptr<ObjectData::Texture>> m_textures;
        std::unordered_map<Texture*, std::unique_ptr<ObjectData::Texture>> m_normalMaps;
        std::unordered_map<Texture*, std::unique_ptr<ObjectData::Texture>> m_specularMaps;

    private: 
        // TEMP: debug drawing.
        std::unique_ptr<DirectX::BasicEffect> m_pDebugDisplayEffect;
        std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_pDebugDisplayPrimitiveBatch;

        // TEMP: model drawing.
        std::unique_ptr<DirectX::IEffectFactory> m_pFxFactory;
        std::unique_ptr<DirectX::EffectTextureFactory> m_pModelResources;
        std::unique_ptr<DirectX::Model> m_pModel;
        DirectX::Model::EffectCollection m_modelNormal;

        DirectX::SimpleMath::Matrix m_view;
        DirectX::SimpleMath::Matrix m_proj;
        DirectX::SimpleMath::Matrix m_world;

};
