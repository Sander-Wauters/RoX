#pragma once

#include <vector>
#include <string>

#include "../../Lib/DirectXTK12/Inc/SimpleMath.h"
#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

#include "Texture.h"

namespace StaticGeometry {
    struct Base {
        Base(const bool instanced = false) noexcept :
            Instanced(instanced) {}

        virtual ~Base() {}

        DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

        Texture* pTexture = nullptr;

        const bool Instanced = false;
        std::vector<DirectX::XMFLOAT3X4> Instances = {};
    };

    struct Cube : Base {
        using Base::Base;

        float Size = 2; 
    };

    struct Box : Base {
        using Base::Base;

        DirectX::XMFLOAT3 Size = { 1.0f, 1.0f, 1.0f };
        bool InvertNormal = false;
    };

    struct Sphere : Base {
        using Base::Base;

        float Diameter = 1;
        size_t Tessellation = 3;
        bool InvertNormal = false;
    };

    struct GeoSphere : Base {
        using Base::Base;

        float Diameter = 1;
        size_t Tessellation = 3;
    };

    struct Cylinder : Base {
        using Base::Base;

        float Height = 1;
        float Diameter = 1;
        size_t Tessellation = 3;
    };

    struct Cone : Base {
        using Base::Base;

        float Diameter = 1;
        float Height = 1;
        size_t Tessellation = 3;
    };

    struct Torus : Base {
        using Base::Base;

        float Diameter = 1;
        float Thickness = 1;
        size_t Tessellation = 3;
    };

    struct Tetrahedron : Base {
        using Base::Base;

        float Size = 1;
    };

    struct Octahedron : Base {
        using Base::Base;

        float Size = 1;
    };

    struct Dodecahedron : Base {
        using Base::Base;

        float Size = 1;
    };

    struct Icosahedron : Base {
        using Base::Base;

        float Size = 1;
    };

    struct Custom : Base {
        using Base::Base;

        std::vector<DirectX::VertexPositionNormalTexture> Vertices = {}; 
        std::vector<std::uint16_t> Indices = {};
    };
}
