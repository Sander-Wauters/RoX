#include "RoX/SubmeshFactory.h"

#include <GeometricPrimitive.h>

std::unique_ptr<Submesh> SubmeshFactory::CreateCube(float size) {
    auto pMesh = std::make_unique<Submesh>("cube", 0);
    DirectX::GeometricPrimitive::CreateCube(*pMesh->GetVertices(), *pMesh->GetIndices(), size, false); 
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateBox(const DirectX::XMFLOAT3& size, bool invertn) {
    auto pMesh = std::make_unique<Submesh>("box", 0);
    DirectX::GeometricPrimitive::CreateBox(*pMesh->GetVertices(), *pMesh->GetIndices(), size, invertn, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateSphere(float diameter, size_t tessellation, bool invertn) {
    auto pMesh = std::make_unique<Submesh>("sphere", 0);
    DirectX::GeometricPrimitive::CreateSphere(*pMesh->GetVertices(), *pMesh->GetIndices(), diameter, tessellation, invertn, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateGeoSphere(float diameter, size_t tessellation) {
    auto pMesh = std::make_unique<Submesh>("geosphere", 0);
    DirectX::GeometricPrimitive::CreateGeoSphere(*pMesh->GetVertices(), *pMesh->GetIndices(), diameter, tessellation, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateCylinder(float height, float diameter, size_t tessellation) {
    auto pMesh = std::make_unique<Submesh>("cylinder", 0);
    DirectX::GeometricPrimitive::CreateCylinder(*pMesh->GetVertices(), *pMesh->GetIndices(), height, diameter, tessellation, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateCone(float diameter, float height, size_t tessellation) {
    auto pMesh = std::make_unique<Submesh>("cone", 0);
    DirectX::GeometricPrimitive::CreateCone(*pMesh->GetVertices(), *pMesh->GetIndices(), diameter, height, tessellation, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateTorus(float diameter, float thickness, size_t tessellation) {
    auto pMesh = std::make_unique<Submesh>("torus", 0);
    DirectX::GeometricPrimitive::CreateTorus(*pMesh->GetVertices(), *pMesh->GetIndices(), diameter, thickness, tessellation, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateTetrahedron(float size) {
    auto pMesh = std::make_unique<Submesh>("tetrahedron", 0);
    DirectX::GeometricPrimitive::CreateTetrahedron(*pMesh->GetVertices(), *pMesh->GetIndices(), size, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateOctahedron(float size) {
    auto pMesh = std::make_unique<Submesh>("octahedron", 0);
    DirectX::GeometricPrimitive::CreateOctahedron(*pMesh->GetVertices(), *pMesh->GetIndices(), size, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateDodecahedron(float size) {
    auto pMesh = std::make_unique<Submesh>("dodecahedron", 0);
    DirectX::GeometricPrimitive::CreateDodecahedron(*pMesh->GetVertices(), *pMesh->GetIndices(), size, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateIcosahedron(float size) {
    auto pMesh = std::make_unique<Submesh>("icosahedron", 0);
    DirectX::GeometricPrimitive::CreateIcosahedron(*pMesh->GetVertices(), *pMesh->GetIndices(), size, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

std::unique_ptr<Submesh> SubmeshFactory::CreateTeapot(float size, size_t tessellation) {
    auto pMesh = std::make_unique<Submesh>("teapot", 0);
    DirectX::GeometricPrimitive::CreateTeapot(*pMesh->GetVertices(), *pMesh->GetIndices(), size, tessellation, false);
    pMesh->SetIndexCount(pMesh->GetIndices()->size());
    return pMesh;
}

