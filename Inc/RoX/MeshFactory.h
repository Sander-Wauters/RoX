#pragma once 

#include "Mesh.h"

namespace MeshFactory {
    void CreateCube(Submesh& mesh, float size = 1);
    void CreateBox(Submesh& mesh, const DirectX::XMFLOAT3& size, bool invertn = false);
    void CreateSphere(Submesh& mesh, float diameter = 1, size_t tessellation = 16, bool invertn = false);
    void CreateGeoSphere(Submesh& mesh, float diameter = 1, size_t tessellation = 3);
    void CreateCylinder(Submesh& mesh, float height = 1, float diameter = 1, size_t tessellation = 32);
    void CreateCone(Submesh& mesh, float diameter = 1, float height = 1, size_t tessellation = 32);
    void CreateTorus(Submesh& mesh, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
    void CreateTetrahedron(Submesh& mesh, float size = 1);
    void CreateOctahedron(Submesh& mesh, float size = 1);
    void CreateDodecahedron(Submesh& mesh, float size = 1);
    void CreateIcosahedron(Submesh& mesh, float size = 1);
    void CreateTeapot(Submesh& mesh, float size = 1, size_t tessellation = 8);
}
