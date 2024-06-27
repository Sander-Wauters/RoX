#pragma once 

#include "Model.h"

namespace SubmeshFactory {
    std::unique_ptr<Submesh> CreateCube(float size = 1);
    std::unique_ptr<Submesh> CreateBox(const DirectX::XMFLOAT3& size, bool invertn = false);
    std::unique_ptr<Submesh> CreateSphere(float diameter = 1, size_t tessellation = 16, bool invertn = false);
    std::unique_ptr<Submesh> CreateGeoSphere(float diameter = 1, size_t tessellation = 3);
    std::unique_ptr<Submesh> CreateCylinder(float height = 1, float diameter = 1, size_t tessellation = 32);
    std::unique_ptr<Submesh> CreateCone(float diameter = 1, float height = 1, size_t tessellation = 32);
    std::unique_ptr<Submesh> CreateTorus(float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
    std::unique_ptr<Submesh> CreateTetrahedron(float size = 1);
    std::unique_ptr<Submesh> CreateOctahedron(float size = 1);
    std::unique_ptr<Submesh> CreateDodecahedron(float size = 1);
    std::unique_ptr<Submesh> CreateIcosahedron(float size = 1);
    std::unique_ptr<Submesh> CreateTeapot(float size = 1, size_t tessellation = 8);
}
