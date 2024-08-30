#pragma once 

#include "Model.h"

// Contains helper functions for generating geometric primitives.
namespace MeshFactory {
    void CreateCube(IMesh& mesh, float size = 1);
    void CreateBox(IMesh& mesh, const DirectX::XMFLOAT3& size, bool invertn = false);
    void CreateSphere(IMesh& mesh, float diameter = 1, size_t tessellation = 16, bool invertn = false);
    void CreateGeoSphere(IMesh& mesh, float diameter = 1, size_t tessellation = 3);
    void CreateCylinder(IMesh& mesh, float height = 1, float diameter = 1, size_t tessellation = 32);
    void CreateCone(IMesh& mesh, float diameter = 1, float height = 1, size_t tessellation = 32);
    void CreateTorus(IMesh& mesh, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
    void CreateTetrahedron(IMesh& mesh, float size = 1);
    void CreateOctahedron(IMesh& mesh, float size = 1);
    void CreateDodecahedron(IMesh& mesh, float size = 1);
    void CreateIcosahedron(IMesh& mesh, float size = 1);
    void CreateTeapot(IMesh& mesh, float size = 1, size_t tessellation = 8);
}
