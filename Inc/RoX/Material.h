#pragma once

#include <string>
#include <DirectXMath.h>

// Collection of flags that **Renderer** uses to determine how a material (and by association a submesh) should be rendered.
namespace RenderFlags {
    constexpr std::uint32_t None =                     0;
    // Determines how a submesh should calculate opacity.
    // Only 1 may be set. If more than 1 is set that the lowest value will be used.
    namespace BlendState {
        constexpr std::uint32_t Opaque =               1 << 0;
        constexpr std::uint32_t AlphaBlend =           1 << 1;
        constexpr std::uint32_t Additive =             1 << 2;
        constexpr std::uint32_t NonPremultiplied =     1 << 3;
        constexpr std::uint32_t Reset =                !(Opaque | AlphaBlend | Additive | NonPremultiplied);
    }
    // Determines a texture texture is read.
    // Only 1 may be set. If more than 1 is set that the lowest value will be used.
    namespace DepthStencilState {
        constexpr std::uint32_t None =                 1 << 4;
        constexpr std::uint32_t Default =              1 << 5;
        constexpr std::uint32_t Read =                 1 << 6;
        constexpr std::uint32_t ReverseZ =             1 << 7;
        constexpr std::uint32_t ReadReverseZ =         1 << 8;
        constexpr std::uint32_t Reset =                !(None | Default | Read | ReverseZ | ReadReverseZ);
    }
    // Determines how the submesh should be rasterized.
    // Only 1 may be set. If more than 1 is set that the lowest value will be used.
    namespace RasterizerState {
        constexpr std::uint32_t CullNone =             1 << 9;
        constexpr std::uint32_t CullClockwise =        1 << 10;
        constexpr std::uint32_t CullCounterClockwise = 1 << 11;
        constexpr std::uint32_t Wireframe =            1 << 12;
        constexpr std::uint32_t Reset =                !(CullNone | CullClockwise | CullCounterClockwise | Wireframe);
    }
    // Determines how a texture is applied to a surface.
    // Only 1 may be set. If more than 1 is set that the lowest value will be used.
    namespace SamplerState {
        constexpr std::uint32_t PointWrap =            1 << 13;
        constexpr std::uint32_t PointClamp =           1 << 14;
        constexpr std::uint32_t LinearWrap =           1 << 15;
        constexpr std::uint32_t LinearClamp =          1 << 16;
        constexpr std::uint32_t AnisotropicWrap =      1 << 17;
        constexpr std::uint32_t AnisotropicClamp =     1 << 18;
        constexpr std::uint32_t Reset =                !(PointWrap | PointClamp | LinearWrap | LinearClamp | AnisotropicWrap | AnisotropicClamp);
    }
    // Determines which effect should be applied by the GPU.
    // Multiple values may be set.
    namespace Effect {
        constexpr std::uint32_t None =                 1 << 19;
        constexpr std::uint32_t Fog =                  1 << 20;
        constexpr std::uint32_t Lighting =             1 << 21;
        constexpr std::uint32_t PerPixelLighting =     1 << 22;
        constexpr std::uint32_t Texture =              1 << 23;
        constexpr std::uint32_t Instanced =            1 << 24;
        constexpr std::uint32_t Specular =             1 << 25;
        constexpr std::uint32_t Skinned =              1 << 26;
        constexpr std::uint32_t Reset =                !(None | Fog | Lighting | PerPixelLighting | Texture | Instanced | Specular | Skinned);
    }

    constexpr std::uint32_t Default = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::CullCounterClockwise | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture;

    constexpr std::uint32_t Skinned = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::CullCounterClockwise | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture | Effect::Skinned;

    constexpr std::uint32_t Wireframe = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::Wireframe | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture;

    constexpr std::uint32_t Instanced = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::CullCounterClockwise | SamplerState::AnisotropicWrap | Effect::Instanced | Effect::Lighting | Effect::Texture;

    constexpr std::uint32_t WireframeInstanced = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::Wireframe | SamplerState::AnisotropicWrap | Effect::Instanced | Effect::Lighting | Effect::Texture;

    constexpr std::uint32_t WireframeSkinned = BlendState::Opaque |  DepthStencilState::Default | 
        RasterizerState::Wireframe | SamplerState::AnisotropicWrap | Effect::Lighting | Effect::Texture | Effect::Skinned;
}

// Describes the texture, color and effects of a submesh.
// Name must be unique as it is used as a key internally.
// **Renderer** requires a material to have both a diffuse and normal map.
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

    private:
        const std::string m_name;
        const std::wstring m_diffuseMapFilePath;
        const std::wstring m_normalMapFilePath;

        std::uint32_t m_flags;

        DirectX::XMVECTOR m_diffuseColor;
        DirectX::XMVECTOR m_emissiveColor;
        DirectX::XMVECTOR m_specularColor;
};
