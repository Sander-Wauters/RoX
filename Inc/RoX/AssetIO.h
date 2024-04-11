#pragma once

#include "Mesh.h"

class AssetIO {
    public: 
        AssetIO() = delete;
        ~AssetIO() = delete;

        AssetIO(AssetIO&) = delete;
        AssetIO& operator= (AssetIO&) = delete;

        AssetIO(AssetIO&&) = delete;
        AssetIO& operator= (AssetIO&&) = delete;

        static void ImportMeshFromVBO(Mesh& mesh, std::wstring filePath);
        static void ExportMeshToVBO(Mesh& mesh, std::wstring filePath);
};

