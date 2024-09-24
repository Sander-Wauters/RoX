#pragma once

#include <functional>

#include "VertexTypes.h"
#include "Material.h"
#include "MeshFactory.h"
#include "Model.h"
#include "Scene.h"
#include "Outline.h"
#include "AssetBatch.h"

using ImGuiWindowFlags = int;

// Contains helper function for the UI.
// Is not indended for use in production code.
namespace DebugUI {
    // All updates made by **DebugUI** are queued.
    // This function will apply all updates in the order they were called.
    void Update();

    // Converting XMFLOAT to/from float*.
    void StoreFloat2(DirectX::XMFLOAT2& in, float* out);
    void StoreFloat3(DirectX::XMFLOAT3& in, float* out);
    void StoreFloat4(DirectX::XMFLOAT4& in, float* out);

    void LoadFloat2(float* in, DirectX::XMFLOAT2& out);
    void LoadFloat3(float* in, DirectX::XMFLOAT3& out);
    void LoadFloat4(float* in, DirectX::XMFLOAT4& out);

    // General utility.
    bool InputTextMultiline(const char* label, std::string* input);
    bool InputFilePath(const char* label, char* input, std::uint16_t inputSize);

    void HelpMarker(const char* desc);

    std::string GUIDLabel(std::string label, std::uint64_t GUID);
    std::string GUIDLabel(std::string label, std::string GUID);

    void ArrayControls(const char* label, std::uint32_t* pIndex, const std::function<void()>& onAdd, const std::function<void()>& onRemove);

    void Error(bool show, const char* label);
    void SameLineError(bool show, const char* label);

    // Vertices and indices.
    void Vertex(VertexPositionNormalTexture& vertex);
    void Vertex(VertexPositionNormalTextureSkinning& vertex);

    void Vertices(std::vector<VertexPositionNormalTexture>& vertices);
    void Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices);

    void Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices);

    // Matrices.
    void Matrix(DirectX::XMMATRIX& matrix);
    void Matrix(DirectX::XMFLOAT4X4& matrix);
    void Matrix(DirectX::XMFLOAT3X4& matrix);

    bool AffineTransformation(DirectX::XMMATRIX& matrix);
    bool AffineTransformation(DirectX::XMFLOAT4X4& matrix);
    bool AffineTransformation(DirectX::XMFLOAT3X4& matrix);

    // Assets.
    void AssetRemover(AssetBatch::AssetType type, AssetBatch& batch);

    void AssetMenu(Asset& asset);

    void AssetRemoverPopupMenu(AssetBatch::AssetType type, AssetBatch& batch);

    // Materials.
    void RenderFlagsPresets(std::uint32_t& renderFlags);
    void RenderFlagsBlendState(std::uint32_t& renderFlags);
    void RenderFlagsDepthStencilState(std::uint32_t& renderFlags);
    void RenderFlagsRasterizerState(std::uint32_t& renderFlags);
    void RenderFlagsSamplerState(std::uint32_t& renderFlags);
    void RenderFlagsEffects(std::uint32_t& renderFlags);
    void RenderFlags(std::uint32_t& renderFlags);

    void MaterialHeader(Material& material);
    bool SelectableMaterialHeader(bool state, Material& material);
    bool TreeNodeMaterialHeader(Material& material);

    void MaterialSelector(std::uint32_t& index, const std::vector<std::shared_ptr<Material>>& materials);
    void MaterialSelector(std::uint64_t& GUID, const Materials& materials);

    void MaterialTextures(Material& material);
    void MaterialColors(Material& material);

    void MaterialCreator(AssetBatch& batch);
    void MaterialRemover(Model& model);

    void MaterialMenu(Material& material);
    void MaterialMenu(std::vector<std::shared_ptr<Material>>& materials);
    void MaterialMenu(const std::unordered_map<std::uint64_t, std::shared_ptr<Material>>& materials);

    void MaterialCreatorPopupMenu(AssetBatch& batch);
    void MaterialAdderPopupMenu(Model& model, const Materials& availableMaterials);
    void MaterialRemoverPopupMenu(Model& model);

    // Sprites.
    void SpritePosition(Sprite& sprite);

    void SpriteCreator(AssetBatch& batch);

    void SpriteMenu(Sprite& sprite);
    void SpriteMenu(const Sprites& sprites); 

    void SpriteCreatorPopupMenu(AssetBatch& batch);

    // Texts.
    void TextCreator(AssetBatch& batch);

    void TextMenu(Text& text);
    void TextMenu(const Texts& texts);

    void TextCreatorPopupMenu(AssetBatch& batch);

    // Bones.
    void BoneSelector(std::uint32_t& index, std::vector<Bone>& bones);

    void BoneMenu(Model& model, std::uint32_t boneIndex);

    // Submeshes.
    void SubmeshInstances(Submesh& submesh);
    void SubmeshVertexIndexing(Submesh& submesh);

    void SubmeshSelector(std::uint32_t& index, const std::vector<std::unique_ptr<Submesh>>& submeshes);

    void SubmeshCreator(IMesh& iMesh, std::vector<std::shared_ptr<Material>>& availableMaterials);
    void SubmeshRemover(IMesh& iMesh);

    void SubmeshMenu(Submesh& submesh, std::vector<std::shared_ptr<Material>>& availableMaterials);

    void SubmeshRemoverPopupMenu(IMesh& iMesh);

    // Meshes.
    void IMeshSelector(std::uint32_t& index, std::vector<std::shared_ptr<IMesh>>& meshes);

    void IMeshCreator(Model& model);
    void IMeshRemover(Model& model);

    void IMeshMenu(IMesh& iMesh);

    void IMeshCreatorPopupMenu(Model& model);
    void IMeshAddGeoOrSubmeshPopupMenu(AssetBatch& batch, Model& model, IMesh& iMesh);
    void IMeshRemoverPopupMenu(Model& model);

    // Mesh factory.
    void GeoSelector(MeshFactory::Geometry& geo);

    void AddCubeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddBoxToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddGeoSphereToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddCylinderToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddConeToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTorusToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTetrahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddOctahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddDodecahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddIcosahedronToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);
    void AddTeapotToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);

    void AddGeoToIMeshCreator(AssetBatch& batch, Model& model, IMesh& iMesh);

    // Models.
    void ModelSelector(AssetBatch& batch, Model** ppSelectedModel, IMesh** ppSelectedIMesh, Submesh** ppSelectedSubmesh);

    void ModelCreator(AssetBatch& batch);

    void ModelMenu(Model& model, const Materials& availableMaterials);

    void ModelCreatorPopupMenu(AssetBatch& batch);

    // Outlines.
    Outline::Type OutlineTypeSelector();

    void OutlineCreator(AssetBatch& batch);

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
    void OutlineMenu(const Outlines& outlines);

    void OutlineCreatorPopupMenu(AssetBatch& batch);

    // Camera.
    void CameraMenu(Camera& camera);

    // Asset batch.
    void AssetBatchSelector(std::uint8_t& index, std::vector<std::shared_ptr<AssetBatch>>& batches);

    void AssetBatchStats(AssetBatch& batch);

    void AssetBatchMenu(AssetBatch& batch);

    // Scene.
    void SceneStats(Scene& scene);

    void SceneMenu(Scene& scene);

    void SceneWindow(Scene& scene, ImGuiWindowFlags windowFlags);
};

