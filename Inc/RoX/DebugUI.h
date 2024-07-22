#pragma once

#include <functional>

#include "VertexTypes.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"

using ImGuiWindowFlags = int;

class DebugUI {
    public:
        DebugUI() = delete;
        DebugUI operator=(DebugUI) = delete;

        DebugUI(const DebugUI& other) = delete;
        DebugUI& operator=(const DebugUI& other) = delete;

        DebugUI(const DebugUI&& other) = delete;
        DebugUI& operator=(const DebugUI&& other) = delete;

        static void HelpMarker(const char* desc);

        static void ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove);

        static void Vertex(VertexPositionNormalTexture& vertex);
        static void Vertex(VertexPositionNormalTextureSkinning& vertex);

        static void Vertices(std::vector<VertexPositionNormalTexture>& vertices);
        static void Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices);

        static void Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices);

        static void Matrix(DirectX::XMMATRIX& matrix);
        static void Matrix(DirectX::XMFLOAT4X4& matrix);
        static void Matrix(DirectX::XMFLOAT3X4& matrix);

        static void RenderFlags(std::uint32_t renderFlags);

        static void MaterialTextures(Material& material);
        static void MaterialColors(Material& material);
        static void MaterialEditor(Material& material);
        static void MaterialSelector(std::uint32_t& index, std::vector<std::shared_ptr<Material>>& materials);

        static void SubmeshInstances(Submesh& submesh);
        static void SubmeshVertexIndexing(Submesh& submesh);

        static void BoneHierarchy(std::vector<Bone>& bones, Bone** ppSelectedBone, std::uint32_t index = 0);
        static void ModelHierarchy(Scene& scene, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh);

        static void CameraMenu(Camera& camera);
        static void SubmeshMenu(Submesh& submesh, Model& grandParent);
        static void IMeshMenu(IMesh& iMesh);
        static void ModelMenu(Model& model);

        static void SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags);
};

