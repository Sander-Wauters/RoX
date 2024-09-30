#pragma once

#include "RoX/Material.h"

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

#include "DeviceResources.h"
#include "TextureDeviceData.h"

class MaterialDeviceData : public IDeviceObserver {
    public:
        MaterialDeviceData(
                DeviceResources& deviceResources, 
                DirectX::DescriptorHeap* pDescriptorHeap, 
                DirectX::CommonStates* pCommonStates,
                DirectX::RenderTargetState* pRtState,
                std::shared_ptr<TextureDeviceData> pDiffuseMap,
                std::shared_ptr<TextureDeviceData> pNormalMap,
                Material& material);
        ~MaterialDeviceData() noexcept;

    public:
        void OnDeviceLost() override;
        void OnDeviceRestored() override;

    public:
        void UpdateIEffect(DirectX::XMMATRIX view, DirectX::XMMATRIX projection, Material& material) noexcept;

        void CreateRenderTargetDependentResources();

    private:
        void CreateIEffect();
        void BindTexturesToIEffect();

        D3D12_INPUT_LAYOUT_DESC InputLayoutDesc(std::uint32_t flags) const;
        D3D12_BLEND_DESC BlendDesc(std::uint32_t flags) const noexcept;
        D3D12_DEPTH_STENCIL_DESC DepthStencilDesc(std::uint32_t flags) const noexcept;
        D3D12_RASTERIZER_DESC RasterizerDesc(std::uint32_t flags) const noexcept;
        D3D12_GPU_DESCRIPTOR_HANDLE SamplerDesc(std::uint32_t flags) const noexcept;

    public:
        DirectX::IEffect* GetIEffect();

        void SetDescriptorHeap(DirectX::DescriptorHeap* pDescriptorHeap) noexcept;
        void SetCommonStates(DirectX::CommonStates* pCommonStates) noexcept;
        void SetRtState(DirectX::RenderTargetState* pRtState) noexcept;

    private:
        DeviceResources& m_deviceResources;
        DirectX::DescriptorHeap* m_pDescriptorHeap;
        DirectX::CommonStates* m_pCommonStates;
        DirectX::RenderTargetState* m_pRtState;

        std::uint32_t m_flags;

        std::unique_ptr<DirectX::IEffect> m_pIEffect;

        std::shared_ptr<TextureDeviceData> m_pDiffuseMap;
        std::shared_ptr<TextureDeviceData> m_pNormalMap;
};
