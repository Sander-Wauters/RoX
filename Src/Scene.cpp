#include "RoX/Scene.h"

Scene::Scene(const std::string name, Camera& camera) 
    noexcept : m_name(name),
    m_camera(camera),
    m_models({}),
    m_sprites({}),
    m_texts({}),
    m_outlines({})
{}

Scene::~Scene() noexcept {
}

void Scene::Add(std::shared_ptr<Model> pMesh) {
    std::shared_ptr<Model>& entry = m_models[pMesh->GetName()];
    if (!entry) 
        entry = pMesh;
    else
        throw std::invalid_argument("Scene already contains this mesh.");
}

void Scene::Add(std::shared_ptr<Sprite> pSprite) {
    std::shared_ptr<Sprite>& entry = m_sprites[pSprite->GetName()];
    if (!entry)
        entry = pSprite;
    else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void Scene::Add(std::shared_ptr<Text> pText) {
    std::shared_ptr<Text>& entry = m_texts[pText->GetName()];
    if (!entry) 
        entry = pText;
    else
        throw std::invalid_argument("Scene already contains this text.");
}

void Scene::Add(std::shared_ptr<IOutline> pOutline) {
    std::shared_ptr<IOutline>& entry = m_outlines[pOutline->GetName()];
    if (!entry) 
        entry = pOutline;
    else
        throw std::invalid_argument("Scene already contains this outline.");
}

void Scene::RemoveMesh(std::string name) {
    m_models.erase(name);
}

void Scene::RemoveSprite(std::string name) {
    m_sprites.erase(name);
}

void Scene::RemoveText(std::string name) {
    m_texts.erase(name);
}

void Scene::RemoveOutline(std::string name) {
    m_outlines.erase(name);
}

std::string Scene::GetName() const noexcept {
    return m_name;
}

Camera& Scene::GetCamera() const noexcept {
    return m_camera;
}

const std::shared_ptr<Model>& Scene::GetModel(std::string name) const {
    return m_models.at(name);
}

const std::shared_ptr<Sprite>& Scene::GetSprite(std::string name) const {
    return m_sprites.at(name);
}

const std::shared_ptr<Text>& Scene::GetText(std::string name) const {
    return m_texts.at(name);
}

const std::shared_ptr<IOutline>& Scene::GetOutline(std::string name) const {
    return m_outlines.at(name); 
}

const std::unordered_map<std::string, std::shared_ptr<Model>>& Scene::GetModels() const noexcept {
    return m_models;
}

const std::unordered_map<std::string, std::shared_ptr<Sprite>>& Scene::GetSprites() const noexcept {
    return m_sprites;
}

const std::unordered_map<std::string, std::shared_ptr<Text>>& Scene::GetTexts() const noexcept {
    return m_texts;
}

const std::unordered_map<std::string, std::shared_ptr<IOutline>>& Scene::GetOutlines() const noexcept {
    return m_outlines;
}

std::uint64_t Scene::GetNumModels() const noexcept {
    return m_models.size();
}

std::uint64_t Scene::GetNumMeshes() const noexcept {
    std::unordered_set<std::shared_ptr<IMesh>> uniqueMeshes;
    for (auto& modelPair : m_models) {
        for (std::shared_ptr<IMesh>& iMesh : modelPair.second->GetMeshes()) {
            uniqueMeshes.insert(iMesh);
        } 
    }
    return uniqueMeshes.size();
}

std::uint64_t Scene::GetNumSubmeshes() const noexcept {
    std::unordered_set<std::shared_ptr<IMesh>> uniqueMeshes;
    for (auto& modelPair : m_models) {
        for (std::shared_ptr<IMesh>& iMesh : modelPair.second->GetMeshes()) {
            uniqueMeshes.insert(iMesh);
        } 
    }

    std::uint64_t count = 0;
    for (std::shared_ptr<IMesh> iMesh : uniqueMeshes) {
        count += iMesh->GetNumSubmeshes();
    } 
    return count;
}

std::uint64_t Scene::GetNumMaterials() const noexcept {
    std::unordered_set<std::string> materialNames;
    for (auto& modelPair : m_models) {
        for (std::shared_ptr<Material>& material : modelPair.second->GetMaterials()) {
            materialNames.insert(material->GetName());
        }
    }
    return materialNames.size();
}

std::uint64_t Scene::GetNumSprites() const noexcept {
    return m_sprites.size();
}

std::uint64_t Scene::GetNumTexts() const noexcept {
    return m_texts.size();
}

std::uint64_t Scene::GetNumOutlines() const noexcept {
    return m_outlines.size();
}

std::uint64_t Scene::GetNumSubmeshInstances() const noexcept {
    std::uint64_t count = 0;
    for (auto& modelPair : m_models) {
        for (auto& pMesh : modelPair.second->GetMeshes()) {
            for (auto& pSubmesh : pMesh->GetSubmeshes()) {
                if (pSubmesh->GetMaterial(*modelPair.second.get())->GetFlags() & RenderFlags::Effect::Instanced)
                    count += pSubmesh->GetNumVisibleInstances();
                else
                    ++count;
            }
        }
    } 
    return count;
}

std::uint64_t Scene::GetNumRenderedSubmeshInstances() const noexcept {
    std::uint64_t count = 0;
    for (auto& modelPair : m_models) {
        if (!modelPair.second->IsVisible())
            continue;

        for (auto& pMesh : modelPair.second->GetMeshes()) {
            if (!pMesh->IsVisible())
                continue;

            for (auto& pSubmesh : pMesh->GetSubmeshes()) {
                if (!pSubmesh->IsVisible())
                    continue;

                if (pSubmesh->GetMaterial(*modelPair.second.get())->GetFlags() & RenderFlags::Effect::Instanced)
                    count += pSubmesh->GetNumVisibleInstances();
                else
                    ++count;
            }
        }
    } 
    return count;
}

std::uint64_t Scene::GetNumLoadedVertices() const noexcept {
    std::uint64_t count = 0;
    for (auto& modelPair : m_models) {
        for (auto& pMesh : modelPair.second->GetMeshes()) {
            count += pMesh->GetNumVertices();
        }
    }
    return count;
}

std::uint64_t Scene::GetNumRenderedVertices() const noexcept {
    std::uint64_t count = 0;
    for (auto& modelPair : m_models) {
        if (!modelPair.second->IsVisible())
            continue;

        for (auto& pMesh : modelPair.second->GetMeshes()) {
            if (!pMesh->IsVisible())
                continue;

            for (auto& pSubmesh : pMesh->GetSubmeshes()) {
                if (!pSubmesh->IsVisible())
                    continue;

                if (pSubmesh->GetMaterial(*modelPair.second.get())->GetFlags() & RenderFlags::Effect::Instanced)
                    count += pMesh->GetNumVertices() * pSubmesh->GetNumVisibleInstances();
                else
                    count += pMesh->GetNumVertices();
            }
        }
    }
    return count;
}
