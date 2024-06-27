#include "RoX/Scene.h"

Scene::Scene(Camera& camera) 
    noexcept : m_camera(camera),
    m_models({}),
    m_sprites({}),
    m_text({}),
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
    std::shared_ptr<Text>& entry = m_text[pText->GetName()];
    if (!entry) 
        entry = pText;
    else
        throw std::invalid_argument("Scene already contains this text.");
}

void Scene::Add(std::shared_ptr<Outline::Base> pOutline) {
    std::shared_ptr<Outline::Base>& entry = m_outlines[pOutline->GetName()];
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
    m_text.erase(name);
}

void Scene::RemoveOutline(std::string name) {
    m_outlines.erase(name);
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
    return m_text.at(name);
}

const std::shared_ptr<Outline::Base>& Scene::GetOutline(std::string name) const {
    return m_outlines.at(name); 
}

const std::unordered_map<std::string, std::shared_ptr<Model>>& Scene::GetModels() const noexcept {
    return m_models;
}

const std::unordered_map<std::string, std::shared_ptr<Sprite>>& Scene::GetSprites() const noexcept {
    return m_sprites;
}

const std::unordered_map<std::string, std::shared_ptr<Text>>& Scene::GetText() const noexcept {
    return m_text;
}

const std::unordered_map<std::string, std::shared_ptr<Outline::Base>>& Scene::GetOutlines() const noexcept {
    return m_outlines;
}


std::uint64_t Scene::GetTotalInstanceCount() const noexcept {
    std::uint64_t count = 0;
    for (auto& model : m_models) {
        for (auto& mesh : model.second->GetMeshes()) {
            for (auto& submesh : mesh->GetSubmeshes()) {
                count += submesh->GetNumVisibleInstances();
            }
        }
    } 
    return count;
}

std::uint64_t Scene::GetTotalVerticesLoaded() const noexcept {
    std::uint64_t count = 0;
    for (auto& model : m_models) {
        for (auto& mesh : model.second->GetMeshes()) {
            for (auto& submesh : mesh->GetSubmeshes()) {
                count += submesh->GetNumVertices();
            }
        }
    }
    return count;
}

std::uint64_t Scene::GetTotalVerticesRendered() const noexcept {
    std::uint64_t count = 0;
    for (auto& model : m_models) {
        for (auto& mesh : model.second->GetMeshes()) {
            for (auto& submesh : mesh->GetSubmeshes()) {
                count += submesh->GetNumVertices() * submesh->GetNumVisibleInstances();
            }
        }
    }
    return count;
}
