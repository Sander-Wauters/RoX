#pragma once

#include <string>

#include "../../Src/Util/pch.h"

namespace RenderFlags {
   namespace BlendState {
       constexpr std::uint32_t Opaque =                0x00000001;
       constexpr std::uint32_t AlphaBlend =            0x00000002;
       constexpr std::uint32_t Additive =              0x00000004;
       constexpr std::uint32_t NonPremultiplied =      0x00000008;
       constexpr std::uint32_t Reset =                 !(Opaque + AlphaBlend + Additive + NonPremultiplied);
   }
   namespace DepthStencilState {
        constexpr std::uint32_t None =                 0x00000010;
        constexpr std::uint32_t Default =              0x00000020;
        constexpr std::uint32_t Read =                 0x00000040;
        constexpr std::uint32_t ReverseZ =             0x00000080;
        constexpr std::uint32_t ReadReverseZ =         0x00000100;
        constexpr std::uint32_t Reset =                !(None + Default + Read + ReverseZ + ReadReverseZ);
   }
   namespace RasterizerState {
        constexpr std::uint32_t CullNone =             0x00000200;
        constexpr std::uint32_t CullClockwise =        0x00000400;
        constexpr std::uint32_t CullCounterClockwise = 0x00000800;
        constexpr std::uint32_t Wireframe =            0x00001000;
        constexpr std::uint32_t Reset =                !(CullNone + CullClockwise + CullCounterClockwise + Wireframe);
   }
   namespace SamplerState {
        constexpr std::uint32_t PointWrap =            0x00002000;
        constexpr std::uint32_t PointClamp =           0x00004000;
        constexpr std::uint32_t LinearWrap =           0x00008000;
        constexpr std::uint32_t LinearClamp =          0x00010000;
        constexpr std::uint32_t AnisotropicWrap =      0x00020000;
        constexpr std::uint32_t AnisotropicClamp =     0x00040000;
        constexpr std::uint32_t Reset =                !(PointWrap + PointClamp + LinearWrap + LinearClamp + AnisotropicWrap + AnisotropicClamp);
   }
   namespace Effect {
       constexpr std::uint32_t None =                  0x00800000;
       constexpr std::uint32_t Fog =                   0x00100000;
       constexpr std::uint32_t Lighting =              0x00200000;
       constexpr std::uint32_t PerPixelLighting =      0x00400000;
       constexpr std::uint32_t Texture =               0x00800000;
       constexpr std::uint32_t Instancing =            0x01000000;
       constexpr std::uint32_t Specular =              0x02000000;
       constexpr std::uint32_t Reset =                 !(None + Fog + Lighting + PerPixelLighting + Texture + Instancing, + Specular);
   }

   constexpr std::uint32_t Default = BlendState::Opaque |  DepthStencilState::Default | 
       RasterizerState::CullCounterClockwise | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture;

   constexpr std::uint32_t DefaultWireframe = BlendState::Opaque |  DepthStencilState::Default | 
       RasterizerState::Wireframe | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture;

   constexpr std::uint32_t DefaultInstancing = BlendState::Opaque |  DepthStencilState::Default | 
       RasterizerState::CullCounterClockwise | SamplerState::AnisotropicWrap | Effect::Instancing | Effect::Lighting | Effect::Texture;

   constexpr std::uint32_t DefaultWireframeInstancing = BlendState::Opaque |  DepthStencilState::Default | 
       RasterizerState::Wireframe | SamplerState::AnisotropicWrap | Effect::Instancing | Effect::Lighting | Effect::Texture;
}

class Material {
    public:
        Material(
                const std::string name,
                const std::wstring diffuseMapFilePath,
                const std::wstring normalMapFilePath,
                std::uint32_t flags = RenderFlags::Default
                ) noexcept;
        ~Material() noexcept;

    public:
        std::string GetName() const noexcept;
        std::wstring GetDiffuseMapFilePath() const noexcept;
        std::wstring GetNormalMapFilePath() const noexcept;

        std::uint32_t GetFlags() const noexcept;

        DirectX::XMVECTOR& GetDiffuseColor() noexcept;
        DirectX::XMVECTOR& GetEmissiveColor() noexcept;
        DirectX::XMVECTOR& GetSpecularColor() noexcept;

        void SetDiffuseColor(DirectX::XMVECTOR color) noexcept;
        void SetEmissiveColor(DirectX::XMVECTOR color) noexcept;
        void SetSpecularColor(DirectX::XMVECTOR color) noexcept;

    private:
        const std::string m_name;
        const std::wstring m_diffuseMapFilePath;
        const std::wstring m_normalMapFilePath;

        std::uint32_t m_flags;

        DirectX::XMVECTOR m_diffuseColor;
        DirectX::XMVECTOR m_emissiveColor;
        DirectX::XMVECTOR m_specularColor;
};
