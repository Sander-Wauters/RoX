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
                DirectX::DescriptorHeap& descriptorHeap, 
                DirectX::CommonStates& commonStates,
                DirectX::RenderTargetState& rtState,
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

    private:
        DeviceResources& m_deviceResources;
        DirectX::DescriptorHeap& m_descriptorHeap;
        DirectX::CommonStates& m_commonStates;
        DirectX::RenderTargetState& m_rtState;

        std::uint32_t m_flags;

        std::unique_ptr<DirectX::IEffect> m_pIEffect;

        std::shared_ptr<TextureDeviceData> m_pDiffuseMap;
        std::shared_ptr<TextureDeviceData> m_pNormalMap;
};
