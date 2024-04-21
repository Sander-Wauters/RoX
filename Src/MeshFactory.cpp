#include "RoX/MeshFactory.h"

#include <GeometricPrimitive.h>

void MeshFactory::CreateCube(Submesh& mesh, float size) {
    DirectX::GeometricPrimitive::CreateCube(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, false); 
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateBox(Submesh& mesh, const DirectX::XMFLOAT3& size, bool invertn) {
    DirectX::GeometricPrimitive::CreateBox(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, invertn, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateSphere(Submesh& mesh, float diameter, size_t tessellation, bool invertn) {
    DirectX::GeometricPrimitive::CreateSphere(*mesh.GetVertices().get(), *mesh.GetIndices().get(), diameter, tessellation, invertn, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateGeoSphere(Submesh& mesh, float diameter, size_t tessellation) {
    DirectX::GeometricPrimitive::CreateGeoSphere(*mesh.GetVertices().get(), *mesh.GetIndices().get(), diameter, tessellation, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateCylinder(Submesh& mesh, float height, float diameter, size_t tessellation) {
    DirectX::GeometricPrimitive::CreateCylinder(*mesh.GetVertices().get(), *mesh.GetIndices().get(), height, diameter, tessellation, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateCone(Submesh& mesh, float diameter, float height, size_t tessellation) {
    DirectX::GeometricPrimitive::CreateCone(*mesh.GetVertices().get(), *mesh.GetIndices().get(), diameter, height, tessellation, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateTorus(Submesh& mesh, float diameter, float thickness, size_t tessellation) {
    DirectX::GeometricPrimitive::CreateTorus(*mesh.GetVertices().get(), *mesh.GetIndices().get(), diameter, thickness, tessellation, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateTetrahedron(Submesh& mesh, float size) {
    DirectX::GeometricPrimitive::CreateTetrahedron(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateOctahedron(Submesh& mesh, float size) {
    DirectX::GeometricPrimitive::CreateOctahedron(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateDodecahedron(Submesh& mesh, float size) {
    DirectX::GeometricPrimitive::CreateDodecahedron(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateIcosahedron(Submesh& mesh, float size) {
    DirectX::GeometricPrimitive::CreateIcosahedron(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

void MeshFactory::CreateTeapot(Submesh& mesh, float size, size_t tessellation) {
    DirectX::GeometricPrimitive::CreateTeapot(*mesh.GetVertices().get(), *mesh.GetIndices().get(), size, tessellation, false);
    mesh.SetIndexCount(mesh.GetIndices()->size());
}

