#pragma once

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "RoX/Scene.h"

#include "DeviceDataBatch.h"
#include "IDeviceObserver.h"
#include "DeviceResources.h"

class DeviceResourceData : public IDeviceObserver {
    public:
        DeviceResourceData(Scene& scene, DeviceResources& deviceResources) noexcept;
        ~DeviceResourceData() noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

        void UpdateEffects();

        void CreateDeviceDependentResources(bool msaaEnabled);
        void CreateRenderTargetDependentResources(DirectX::ResourceUploadBatch& resourceUploadBatch, bool msaaEnabled);
        void CreateWindowSizeDependentResources();

    private:
        void CreateImGuiResources();

    public:
        Scene& GetScene() const noexcept;

        std::uint8_t GetNumDataBatches() const noexcept;
        std::uint8_t GetNumStaticBatches() const noexcept;
        std::uint8_t GetNumDynamicBatches() const noexcept;
        std::uint8_t GetFirstDynamicBatchIndex() const noexcept;

        const std::vector<std::unique_ptr<DeviceDataBatch>>& GetDataBatches() const noexcept;
        DirectX::DescriptorHeap* GetImGuiDescriptorHeap() noexcept;
        DirectX::CommonStates* GetImGuiStates() noexcept;

    private:
        Scene& m_scene;
        DeviceResources& m_deviceResources;

        std::vector<std::unique_ptr<DeviceDataBatch>> m_dataBatches;

        std::unique_ptr<DirectX::DescriptorHeap> m_pImGuiDescriptorHeap;
        std::unique_ptr<DirectX::CommonStates> m_pImGuiStates;
};
