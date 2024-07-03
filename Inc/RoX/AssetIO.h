#pragma once

#include "Model.h"

namespace AssetIO {
    std::unique_ptr<Submesh> ImportSubmeshFromVBO(std::string filePath);
    void ExportSubmeshToVBO(Submesh* pSubmesh, std::string filePath);

    std::unique_ptr<Submesh> ImportSubmesh(std::string filePath);
    void ExportSubmesh(Submesh* pSubmesh, std::string filePath);

    std::shared_ptr<Mesh> ImportMesh(std::string filePath);
    void ExportMesh(Mesh* pMeshPart, std::string filePath);

    std::shared_ptr<Model> ImportModel(std::string filePath, std::shared_ptr<Material> pMaterial);
    void ExportModel(Model* pMesh, std::string filePath);
};

