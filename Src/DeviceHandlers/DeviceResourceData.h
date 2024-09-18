#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/Scene.h"

#include "DeviceDataBatch.h"
#include "IDeviceObserver.h"
#include "DeviceResources.h"

class DeviceResourceData : public IDeviceObserver {
    public:
        DeviceResourceData(DeviceResources& deviceResources) noexcept;
        ~DeviceResourceData() noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void Load(Scene& scene, bool& msaaEnabled);
        void Update();

        void CreateDeviceDependentResources();
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch);
        void CreateWindowSizeDependentResources();

        void CreateImGuiResources();

    private:
        void FreshLoad(Scene& scene, bool& msaaEnabled); // First scene ever loaded in.
        void DirtyLoad(Scene& scene, bool& msaaEnabled); // Every scene following the first.

    public:
        Scene& GetScene() const noexcept;

        std::uint8_t GetNumDataBatches() const noexcept;

        const std::vector<std::unique_ptr<DeviceDataBatch>>& GetDataBatches() const noexcept;
        DirectX::DescriptorHeap* GetImGuiDescriptorHeap() noexcept;
        DirectX::CommonStates* GetImGuiStates() noexcept;

        bool SceneLoaded() const noexcept;

    private:
        Scene* m_pScene;
        DeviceResources& m_deviceResources;

        std::vector<std::unique_ptr<DeviceDataBatch>> m_dataBatches;

        std::unique_ptr<DirectX::DescriptorHeap> m_pImGuiDescriptorHeap;
        std::unique_ptr<DirectX::CommonStates> m_pImGuiStates;
};
