#include "RoX/MeshFactory.h"

#include "../Util/pch.h"
#include "../Util/dxtk12pch.h"

void AppendToMesh(
        std::vector<DirectX::VertexPositionNormalTexture>& verticesIn,
        std::vector<std::uint16_t>& indicesIn,
        IMesh& iMesh) 
{
    if (auto pMesh = dynamic_cast<Mesh*>(&iMesh)) {
        pMesh->GetVertices().reserve(verticesIn.size());
        for (DirectX::VertexPositionNormalTexture vin : verticesIn) {
            pMesh->GetVertices().push_back({ vin.position, vin.normal, vin.textureCoordinate });
        }
    }
    if (auto pSkinnedMesh = dynamic_cast<SkinnedMesh*>(&iMesh)) {
        pSkinnedMesh->GetVertices().reserve(verticesIn.size());
        for (DirectX::VertexPositionNormalTexture vin : verticesIn) {
            pSkinnedMesh->GetVertices().push_back({ vin.position, vin.normal, vin.textureCoordinate, { 0, 0, 0, 0 }, { 1.f, 1.f, 1.f, 1.f } });
        }
    }

    iMesh.GetIndices().reserve(indicesIn.size());
    iMesh.GetIndices().insert(iMesh.GetIndices().end(), indicesIn.begin(), indicesIn.end());
}

void ThrowIfUsingStaticBuffers(IMesh& iMesh) {
    if (iMesh.IsUsingStaticBuffers())
        throw std::invalid_argument("Cannot add geometry to mesh with static buffers.");
}

void MeshFactory::AddCube(IMesh& iMesh, float size) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("cube", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCube(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddBox(IMesh& iMesh, const DirectX::XMFLOAT3& size, bool invertn) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("box", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateBox(vertices, indices, size, invertn, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddSphere(IMesh& iMesh, float diameter, size_t tessellation, bool invertn) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("sphere", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateSphere(vertices, indices, diameter, tessellation, invertn, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddGeoSphere(IMesh& iMesh, float diameter, size_t tessellation) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("geosphere", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateGeoSphere(vertices, indices, diameter, tessellation, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddCylinder(IMesh& iMesh, float height, float diameter, size_t tessellation) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("cylinder", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCylinder(vertices, indices, height, diameter, tessellation, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddCone(IMesh& iMesh, float diameter, float height, size_t tessellation) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("cone", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateCone(vertices, indices, diameter, height, tessellation, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddTorus(IMesh& iMesh, float diameter, float thickness, size_t tessellation) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("torus", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTorus(vertices, indices, diameter, thickness, tessellation, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddTetrahedron(IMesh& iMesh, float size) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("tetrahedron", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTetrahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddOctahedron(IMesh& iMesh, float size) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("octahedron", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateOctahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddDodecahedron(IMesh& iMesh, float size) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("dodecahedron", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateDodecahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddIcosahedron(IMesh& iMesh, float size) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("icosahedron", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateIcosahedron(vertices, indices, size, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::AddTeapot(IMesh& iMesh, float size, size_t tessellation) {
    ThrowIfUsingStaticBuffers(iMesh);

    auto pSubmesh = std::make_unique<Submesh>("teapot", 0);
    pSubmesh->SetStartIndex(iMesh.GetNumIndices());
    pSubmesh->SetVertexOffset(iMesh.GetNumVertices());
    
    std::vector<DirectX::VertexPositionNormalTexture> vertices;
    std::vector<std::uint16_t> indices;
    DirectX::GeometricPrimitive::CreateTeapot(vertices, indices, size, tessellation, false); 
    AppendToMesh(vertices, indices, iMesh);

    pSubmesh->SetIndexCount(indices.size());

    iMesh.Add(std::move(pSubmesh));
    iMesh.UpdateBuffers();
}

void MeshFactory::Add(Geometry geo, IMesh& iMesh) {
    switch (geo) {
        case MeshFactory::Geometry::Cube:
            AddCube(iMesh); break;
        case MeshFactory::Geometry::Box:
            AddBox(iMesh); break;
        case MeshFactory::Geometry::Sphere:
            AddSphere(iMesh); break;
        case MeshFactory::Geometry::GeoSphere:
            AddGeoSphere(iMesh); break;
        case MeshFactory::Geometry::Cylinder:
            AddCylinder(iMesh); break;
        case MeshFactory::Geometry::Cone:
            AddCone(iMesh); break;
        case MeshFactory::Geometry::Torus:
            AddTorus(iMesh); break;
        case MeshFactory::Geometry::Tetrahedron:
            AddTetrahedron(iMesh); break;
        case MeshFactory::Geometry::Octahedron:
            AddOctahedron(iMesh); break;
        case MeshFactory::Geometry::Dodecahedron:
            AddDodecahedron(iMesh); break;
        case MeshFactory::Geometry::Icosahedron:
            AddIcosahedron(iMesh); break;
        case MeshFactory::Geometry::Teapot:
            AddTeapot(iMesh); break;
    }
}

