#pragma once

#include <vector>
#include <string>

#include "../../Lib/DirectXTK12/Inc/SimpleMath.h"
#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

#include "Texture.h"

namespace StaticGeometry {
    struct Base {
        Base(const bool instanced = true) noexcept :
            Instanced(instanced) {}
        virtual ~Base() {}

        const bool Instanced = false;
        bool Visible = true;

        Texture* pTexture = nullptr;

        DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX View = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX Projection = DirectX::XMMatrixIdentity();
        std::vector<DirectX::XMFLOAT3X4> Instances = {};
    };

    struct Cube : public Base {
        using Base::Base;

        float Size = 1.f; 
    };

    struct Box : public Base {
        using Base::Base;

        DirectX::XMFLOAT3 Size = { 1.0f, 1.0f, 1.0f };
        bool InvertNormal = false;
    };

    struct Sphere : public Base {
        using Base::Base;

        float Diameter = 1.f;
        size_t Tessellation = 16;
        bool InvertNormal = false;
    };

    struct GeoSphere : public Base {
        using Base::Base;

        float Diameter = 1.f;
        size_t Tessellation = 3;
    };

    struct Cylinder : public Base {
        using Base::Base;

        float Height = 1.f;
        float Diameter = 1.f;
        size_t Tessellation = 32;
    };

    struct Cone : public Base {
        using Base::Base;

        float Diameter = 1.f;
        float Height = 1.f;
        size_t Tessellation = 32;
    };

    struct Torus : public Base {
        using Base::Base;

        float Diameter = 1.f;
        float Thickness = 0.333f;
        size_t Tessellation = 32;
    };

    struct Tetrahedron : public Base {
        using Base::Base;

        float Size = 1.f;
    };

    struct Octahedron : public Base {
        using Base::Base;

        float Size = 1.f;
    };

    struct Dodecahedron : public Base {
        using Base::Base;

        float Size = 1.f;
    };

    struct Icosahedron : public Base {
        using Base::Base;

        float Size = 1.f;
    };

    struct Custom : public Base {
        using Base::Base;

        std::vector<DirectX::VertexPositionNormalTexture> Vertices = {}; 
        std::vector<std::uint16_t> Indices = {};
    };
}
