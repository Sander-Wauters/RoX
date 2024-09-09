#pragma once

#include <functional>

#include "VertexTypes.h"
#include "Material.h"
#include "Model.h"
#include "Scene.h"
#include "Outline.h"
#include "AssetBatch.h"

using ImGuiWindowFlags = int;

// Contains helper function for the UI.
// Is not indended for use in production code.
namespace DebugUI {
    bool InputTextMultiline(const char* label, std::string* str);
    void HelpMarker(const char* desc);

    void StoreFloat2(DirectX::XMFLOAT2& in, float* out);
    void StoreFloat3(DirectX::XMFLOAT3& in, float* out);
    void StoreFloat4(DirectX::XMFLOAT4& in, float* out);

    void LoadFloat2(float* in, DirectX::XMFLOAT2& out);
    void LoadFloat3(float* in, DirectX::XMFLOAT3& out);
    void LoadFloat4(float* in, DirectX::XMFLOAT4& out);

    void ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove);

    void Vertex(VertexPositionNormalTexture& vertex);
    void Vertex(VertexPositionNormalTextureSkinning& vertex);

    void Vertices(std::vector<VertexPositionNormalTexture>& vertices);
    void Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices);

    void Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices);

    void Matrix(DirectX::XMMATRIX& matrix);
    void Matrix(DirectX::XMFLOAT4X4& matrix);
    void Matrix(DirectX::XMFLOAT3X4& matrix);

    bool AffineTransformation(DirectX::XMMATRIX& matrix);
    bool AffineTransformation(DirectX::XMFLOAT4X4& matrix);
    bool AffineTransformation(DirectX::XMFLOAT3X4& matrix);

    void RenderFlags(std::uint32_t renderFlags);

    void MaterialTextures(Material& material);
    void MaterialColors(Material& material);
    void MaterialSelector(std::uint32_t& index, std::vector<std::shared_ptr<Material>>& materials);

    void SubmeshInstances(Submesh& submesh);
    void SubmeshVertexIndexing(Submesh& submesh);

    void ModelHierarchy(AssetBatch& batch, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh);
    void BoneHierarchy(Model& model, std::uint32_t& selectedBone);

    void AddSubmesh(IMesh& iMesh);
    void AddIMesh(Model& model);
    void AddModel(AssetBatch& batch);
    void AddMaterial(AssetBatch& batch);
    void AddSprite(AssetBatch& batch);
    void AddOutline(AssetBatch& batch);

    void CameraMenu(Camera& camera);
    void MaterialMenu(Material& material);
    void BoneMenu(Model& model, std::uint32_t boneIndex);
    void SubmeshMenu(Submesh& submesh, Model& grandParent);
    void IMeshMenu(IMesh& iMesh);
    void ModelMenu(Model& model);
    void SpriteMenu(Sprite& sprite);
    void BoundingBoxOutlineMenu(BoundingBodyOutline<DirectX::BoundingBox>& outline);
    void BoundingFrustumOutlineMenu(BoundingBodyOutline<DirectX::BoundingFrustum>& outline);
    void BoundingOrientedBoxOutlineMenu(BoundingBodyOutline<DirectX::BoundingOrientedBox>& outline);
    void BoundingSphereOutlineMenu(BoundingBodyOutline<DirectX::BoundingSphere>& outline);
    void GridOutlineMenu(GridOutline& outline);
    void RingOutlineMenu(RingOutline& outline);
    void RayOutlineMenu(RayOutline& outline);
    void TriangleOutlineMenu(TriangleOutline& outline);
    void QuadOutlineMenu(QuadOutline& outline);
    void OutlineMenu(Outline& outline);
    void AssetBatchMenu(AssetBatch& batch);

    void SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags);
};

