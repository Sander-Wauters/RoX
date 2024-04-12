#pragma once

#include "Mesh.h"

namespace AssetIO {
    void ImportMeshFromVBO(Mesh& mesh, std::string filePath);
    void ExportMeshToVBO(Mesh& mesh, std::string filePath);

    void ImportMesh(Mesh& mesh, std::string filePath);
    void ExportMesh(Mesh& mesh, std::string filePath);
};

