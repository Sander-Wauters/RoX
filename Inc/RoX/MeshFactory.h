#pragma once 

#include "Model.h"

// Contains helper functions for generating geometric primitives.
// Adds new geometry to a existing Mesh and adds a new submesh.
namespace MeshFactory {
    enum class Geometry {
        Cube,
        Box,
        Sphere,
        GeoSphere,
        Cylinder,
        Cone,
        Torus,
        Tetrahedron,
        Octahedron,
        Dodecahedron,
        Icosahedron,
        Teapot
    };

    void AddCube(IMesh& iMesh, float size = 1);
    void AddBox(IMesh& iMesh, const DirectX::XMFLOAT3& size = { 1.f, 1.f, 1.f }, bool invertn = false);
    void AddSphere(IMesh& iMesh, float diameter = 1, size_t tessellation = 16, bool invertn = false);
    void AddGeoSphere(IMesh& iMesh, float diameter = 1, size_t tessellation = 3);
    void AddCylinder(IMesh& iMesh, float height = 1, float diameter = 1, size_t tessellation = 32);
    void AddCone(IMesh& iMesh, float diameter = 1, float height = 1, size_t tessellation = 32);
    void AddTorus(IMesh& iMesh, float diameter = 1, float thickness = 0.333f, size_t tessellation = 32);
    void AddTetrahedron(IMesh& iMesh, float size = 1);
    void AddOctahedron(IMesh& iMesh, float size = 1);
    void AddDodecahedron(IMesh& iMesh, float size = 1);
    void AddIcosahedron(IMesh& iMesh, float size = 1);
    void AddTeapot(IMesh& iMesh, float size = 1, size_t tessellation = 8);

    void Add(Geometry geo, IMesh& iMesh);
}
