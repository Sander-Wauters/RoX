#pragma once

#include <functional>

#include "../Lib/ImGui/Inc/ImGui/imgui.h"

#include "VertexTypes.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"

using ImGuiWindowFlags = int;

// Contains helper function for the UI.
// Is not indended for use in production code.
namespace DebugUI {
    void HelpMarker(const char* desc);

    void ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove);

    void Vertex(VertexPositionNormalTexture& vertex);
    void Vertex(VertexPositionNormalTextureSkinning& vertex);

    void Vertices(std::vector<VertexPositionNormalTexture>& vertices);
    void Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices);

    void Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices);

    void Matrix(DirectX::XMMATRIX& matrix);
    void Matrix(DirectX::XMFLOAT4X4& matrix);
    void Matrix(DirectX::XMFLOAT3X4& matrix);

    void RenderFlags(std::uint32_t renderFlags);

    void MaterialTextures(Material& material);
    void MaterialColors(Material& material);
    void MaterialEditor(Material& material);
    void MaterialSelector(std::uint32_t& index, std::vector<std::shared_ptr<Material>>& materials);

    void SubmeshInstances(Submesh& submesh);
    void SubmeshVertexIndexing(Submesh& submesh);

    void ModelHierarchy(Scene& scene, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh);

    void CameraMenu(Camera& camera);
    void SubmeshMenu(Submesh& submesh, Model& grandParent);
    void IMeshMenu(IMesh& iMesh);
    void ModelMenu(Model& model);

    void SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags);
};

