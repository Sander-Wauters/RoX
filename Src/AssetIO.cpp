#include "RoX/AssetIO.h"

#include <fstream>

#include "Util/Logger.h"

void AssetIO::ImportMeshFromVBO(Mesh& mesh, std::wstring filePath) {
    std::wstring extension = filePath.substr(filePath.size() - 4);
    if (extension != L".vbo")
        throw std::invalid_argument("Must be a .vbo file");

    std::ifstream file(filePath, std::ifstream::binary); 
    if (!file.is_open())
        throw std::invalid_argument("Failed to open .vbo file");

    std::uint32_t numVertices = 0;
    std::uint32_t numIndices = 0;
    
    file.read(reinterpret_cast<char*>(&numVertices), sizeof(std::uint32_t));
    if (!numVertices)
        throw std::runtime_error("Failed reading numVertices from .vbo file");

    file.read(reinterpret_cast<char*>(&numIndices), sizeof(std::uint32_t));
    if (!numIndices)
        throw std::runtime_error("Failed reading numIndices from .vbo file");

    mesh.GetVertices().resize(numVertices);
    file.read(reinterpret_cast<char*>(mesh.GetVertices().data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices);

    mesh.GetIndices().resize(numIndices);
    file.read(reinterpret_cast<char*>(mesh.GetIndices().data()), sizeof(std::uint16_t) * numIndices);

    file.close();
}

void AssetIO::ExportMeshToVBO(Mesh& mesh, std::wstring filePath) {
    std::wstring extension = filePath.substr(filePath.size() - 4);
    if (extension != L".vbo")
        filePath += L".vbo";

    std::uint32_t numVertices = mesh.GetVertices().size();
    std::uint32_t numIndices = mesh.GetIndices().size();
   
    std::ofstream file(filePath, std::ofstream::binary);
    file.write(reinterpret_cast<char*>(&numVertices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(&numIndices), sizeof(std::uint32_t))
        .write(reinterpret_cast<char*>(mesh.GetVertices().data()), sizeof(DirectX::VertexPositionNormalTexture) * numVertices)
        .write(reinterpret_cast<char*>(mesh.GetIndices().data()), sizeof(std::uint16_t) * numIndices);

    file.close();
}
