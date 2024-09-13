#pragma once 

#include "Model.h"

// Contains helper functions for generating geometric primitives.
// Adds new geometry to a existing mesh and adds a new submesh.
namespace MeshFactory {
    void AddCube(IMesh& mesh, float size = 1);
    void AddBox(IMesh& mesh, const DirectX::XMFLOAT3& size, bool invertn = false);
    void AddSphere(IMesh& mesh, float diameter = 1, size_t tessellation = 16, bool invertn = false);
    void AddGeoSphere(IMesh& mesh, float diameter = 1, size_t tessellation = 3);
    void AddCylinder(IMesh& mesh, float height = 1, float diameter = 1, size_t tessellation = 32);
    void AddCone(IMesh& mesh, float diameter = 1, float height = 1, size_t tessellation = 32);
    void AddTorus(IMesh& mesh, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
    void AddTetrahedron(IMesh& mesh, float size = 1);
    void AddOctahedron(IMesh& mesh, float size = 1);
    void AddDodecahedron(IMesh& mesh, float size = 1);
    void AddIcosahedron(IMesh& mesh, float size = 1);
    void AddTeapot(IMesh& mesh, float size = 1, size_t tessellation = 8);
}
