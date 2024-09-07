#include "RoX/Scene.h"

Scene::Scene(const std::string name, Camera& camera) 
    noexcept : m_name(name),
    m_camera(camera)
{}

Scene::~Scene() noexcept 
{}

void Scene::Add(std::shared_ptr<AssetBatch> batch) {
    m_assetBatches.push_back(batch);
}

void Scene::Add(std::uint8_t batch, std::shared_ptr<Model> pMesh) {
    m_assetBatches[batch]->Add(pMesh);
}

void Scene::Add(std::uint8_t batch, std::shared_ptr<Sprite> pSprite) {
    m_assetBatches[batch]->Add(pSprite);
}

void Scene::Add(std::uint8_t batch, std::shared_ptr<Text> pText) {
    m_assetBatches[batch]->Add(pText);
}

void Scene::Add(std::uint8_t batch, std::shared_ptr<Outline> pOutline) {
    m_assetBatches[batch]->Add(pOutline);
}

void Scene::RemoveModel(std::uint8_t batch, std::string name) {
    m_assetBatches[batch]->RemoveModel(name);
}

void Scene::RemoveSprite(std::uint8_t batch, std::string name) {
    m_assetBatches[batch]->RemoveSprite(name);
}

void Scene::RemoveText(std::uint8_t batch, std::string name) {
    m_assetBatches[batch]->RemoveText(name);
}

void Scene::RemoveOutline(std::uint8_t batch, std::string name) {
    m_assetBatches[batch]->RemoveOutline(name);
}

std::string Scene::GetName() const noexcept {
    return m_name;
}

Camera& Scene::GetCamera() const noexcept {
    return m_camera;
}

std::shared_ptr<AssetBatch>& Scene::GetAssetBatch(std::uint8_t batch) noexcept {
    return m_assetBatches[batch];
}

std::vector<std::shared_ptr<AssetBatch>>& Scene::GetAssetBatches() noexcept {
    return m_assetBatches;
}

std::shared_ptr<Model>& Scene::GetModel(std::uint8_t batch, std::string name) {
    return m_assetBatches[batch]->GetModel(name);
}

std::shared_ptr<Sprite>& Scene::GetSprite(std::uint8_t batch, std::string name) {
    return m_assetBatches[batch]->GetSprite(name);
}

std::shared_ptr<Text>& Scene::GetText(std::uint8_t batch, std::string name) {
    return m_assetBatches[batch]->GetText(name);
}

std::shared_ptr<Outline>& Scene::GetOutline(std::uint8_t batch, std::string name) {
    return m_assetBatches[batch]->GetOutline(name);
}

const std::unordered_map<std::string, std::shared_ptr<Model>>& Scene::GetModels(std::uint8_t batch) const noexcept {
    return m_assetBatches[batch]->GetModels();
}

const std::unordered_map<std::string, std::shared_ptr<Sprite>>& Scene::GetSprites(std::uint8_t batch) const noexcept {
    return m_assetBatches[batch]->GetSprites();
}

const std::unordered_map<std::string, std::shared_ptr<Text>>& Scene::GetTexts(std::uint8_t batch) const noexcept {
    return m_assetBatches[batch]->GetTexts();
}

const std::unordered_map<std::string, std::shared_ptr<Outline>>& Scene::GetOutlines(std::uint8_t batch) const noexcept {
    return m_assetBatches[batch]->GetOutlines();
}

std::uint8_t Scene::GetNumAssetBatches() const noexcept {
    return m_assetBatches.size();
}

std::uint64_t Scene::GetNumModels() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumModels();
    }
    return count;
}

std::uint64_t Scene::GetNumMeshes() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumMeshes();
    }
    return count;
}

std::uint64_t Scene::GetNumSubmeshes() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumSubmeshes();
    }
    return count;
}

std::uint64_t Scene::GetNumMaterials() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumMaterials();
    }
    return count;
}

std::uint64_t Scene::GetNumSprites() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumSprites();
    }
    return count;
}

std::uint64_t Scene::GetNumTexts() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumTexts();
    }
    return count;
}

std::uint64_t Scene::GetNumOutlines() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumOutlines();
    }
    return count;
}

std::uint64_t Scene::GetNumSubmeshInstances() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumSubmeshInstances();
    }
    return count;
}

std::uint64_t Scene::GetNumRenderedSubmeshInstances() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumRenderedSubmeshInstances();
    }
    return count;
}

std::uint64_t Scene::GetNumLoadedVertices() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumLoadedVertices();
    }
    return count;
}

std::uint64_t Scene::GetNumRenderedVertices() const noexcept {
    std::uint64_t count = 0;
    for (int i = 0; i < GetNumAssetBatches(); ++i) {
        count += m_assetBatches[i]->GetNumRenderedVertices();
    }
    return count;
}
