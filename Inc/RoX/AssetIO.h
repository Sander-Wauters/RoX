#pragma once

#include "Mesh.h"

namespace AssetIO {
    void ImportMeshFromVBO(Submesh& mesh, std::string filePath);
    void ExportMeshToVBO(Submesh& mesh, std::string filePath);

    void ImportMesh(Submesh& mesh, std::string filePath);
    void ExportMesh(Submesh& mesh, std::string filePath);
};

