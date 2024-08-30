#include "RoX/MeshFactory.h"

#include "Util/pch.h"
#include "Util/dxtk12pch.h"

void AppendToMesh(
        std::vector<DirectX::VertexPositionNormalTexture>& verticesIn,
        std::vector<std::uint16_t>& indicesIn,
        IMesh& mesh) 
{
    if (auto pMesh = dynamic_cast<Mesh*>(&mesh)) {
        pMesh->GetVertices().reserve(verticesIn.size());
        for (DirectX::VertexPositionNormalTexture vin : verticesIn) {
            pMesh->GetVertices().push_back({ vin.position, vin.normal, vin.textureCoordinate });
        }
    }
    if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(&mesh)) {
        pSkinnedMesh->GetVertices().reserve(verticesIn.size());
        for (DirectX::VertexPositionNormalTexture vin : verticesIn) {
            pSkinnedMesh->GetVertices().push_back({ vin.position, vin.normal, vin.textureCoordinate, { 0, 0, 0, 0 }, { 1.f, 1.f, 1.f, 1.f } });
        }
    }

    mesh.GetIndices().reserve(indicesIn.size());
    mesh.GetIndices().insert(mesh.GetIndices().end(), indicesIn.begin(), indicesIn.end());
}

void MeshFactory::CreateCube(IMesh& mesh, float size) {
    auto pSubmesh = std::make_unique<Submesh>("cube", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCube(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateBox(IMesh& mesh, const DirectX::XMFLOAT3& size, bool invertn) {
    auto pSubmesh = std::make_unique<Submesh>("box", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateBox(vertices, indices, size, invertn, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateSphere(IMesh& mesh, float diameter, size_t tessellation, bool invertn) {
    auto pSubmesh = std::make_unique<Submesh>("sphere", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateSphere(vertices, indices, diameter, tessellation, invertn, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateGeoSphere(IMesh& mesh, float diameter, size_t tessellation) {
    auto pSubmesh = std::make_unique<Submesh>("geosphere", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateGeoSphere(vertices, indices, diameter, tessellation, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateCylinder(IMesh& mesh, float height, float diameter, size_t tessellation) {
    auto pSubmesh = std::make_unique<Submesh>("cylinder", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCylinder(vertices, indices, height, diameter, tessellation, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateCone(IMesh& mesh, float diameter, float height, size_t tessellation) {
    auto pSubmesh = std::make_unique<Submesh>("cone", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCone(vertices, indices, diameter, height, tessellation, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateTorus(IMesh& mesh, float diameter, float thickness, size_t tessellation) {
    auto pSubmesh = std::make_unique<Submesh>("torus", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTorus(vertices, indices, diameter, thickness, tessellation, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateTetrahedron(IMesh& mesh, float size) {
    auto pSubmesh = std::make_unique<Submesh>("tetrahedron", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTetrahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateOctahedron(IMesh& mesh, float size) {
    auto pSubmesh = std::make_unique<Submesh>("octahedron", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateOctahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateDodecahedron(IMesh& mesh, float size) {
    auto pSubmesh = std::make_unique<Submesh>("dodecahedron", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateDodecahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateIcosahedron(IMesh& mesh, float size) {
    auto pSubmesh = std::make_unique<Submesh>("icosahedron", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateIcosahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}

void MeshFactory::CreateTeapot(IMesh& mesh, float size, size_t tessellation) {
    auto pSubmesh = std::make_unique<Submesh>("teapot", 0);
    pSubmesh->SetStartIndex(mesh.GetNumIndices());
    pSubmesh->SetVertexOffset(mesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTeapot(vertices, indices, size, tessellation, false); 
    AppendToMesh(vertices, indices, mesh);

    pSubmesh->SetIndexCount(indices.size());

    mesh.Add(std::move(pSubmesh));
}
