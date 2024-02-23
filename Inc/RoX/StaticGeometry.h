#pragma once

#include <vector>

#include "../../Lib/DirectXTK12/Inc/SimpleMath.h"
#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

namespace StaticGeometry {
    struct Base {
        virtual ~Base() {}

        DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
    };

    struct Cube : Base {
        float Size = 2; 
    };

    struct Box : Base {
        DirectX::XMFLOAT3 Size = { 1.0f, 1.0f, 1.0f };
        bool InvertNormal = false;
    };

    struct Sphere : Base {
        float Diameter = 1;
        size_t Tessellation = 3;
        bool InvertNormal = false;
    };

    struct GeoSphere : Base {
        float Diameter = 1;
        size_t Tessellation = 3;
    };

    struct Cylinder : Base {
        float Height = 1;
        float Diameter = 1;
        size_t Tessellation = 3;
    };

    struct Cone : Base {
        float Diameter = 1;
        float Height = 1;
        size_t Tessellation = 3;
    };

    struct Torus : Base {
        float Diameter = 1;
        float Thickness = 1;
        size_t Tessellation = 3;
    };

    struct Tetrahedron : Base {
        float Size = 1;
    };

    struct Octahedron : Base {
        float Size = 1;
    };

    struct Dodecahedron : Base {
        float Size = 1;
    };

    struct Icosahedron : Base {
        float Size = 1;
    };

    struct Custom : Base {
        std::vector<DirectX::VertexPositionNormalTexture> Vertices = {}; 
        std::vector<std::uint16_t> Indices = {};
    };
}
