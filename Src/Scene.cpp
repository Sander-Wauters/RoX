#include "RoX/Scene.h"

Scene::Scene(Camera& camera) 
    noexcept : m_camera(camera),
    m_meshes({}),
    m_sprites({}),
    m_text({}),
    m_outlines({}),
    m_sceneObservers({})
{}

Scene::~Scene() noexcept {
}

void Scene::Add(std::shared_ptr<Mesh> pMesh) {
    std::shared_ptr<Mesh>& entry = m_meshes[pMesh->GetName()];
    if (!entry) {
        entry = pMesh;
        NotifyAdd<Mesh>(pMesh.get());
    }
    else
        throw std::invalid_argument("Scene already contains this mesh.");
}

void Scene::Add(std::shared_ptr<Sprite> pSprite) {
    std::shared_ptr<Sprite>& entry = m_sprites[pSprite->GetName()];
    if (!entry) {
        entry = pSprite;
        NotifyAdd<Sprite>(pSprite.get());
    }
    else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void Scene::Add(std::shared_ptr<Text> pText) {
    std::shared_ptr<Text>& entry = m_text[pText->GetName()];
    if (!entry) {
        entry = pText;
        NotifyAdd<Text>(pText.get());
    }
    else
        throw std::invalid_argument("Scene already contains this text.");
}

void Scene::Add(std::shared_ptr<Outline::Base> pOutline) {
    std::shared_ptr<Outline::Base>& entry = m_outlines[pOutline->GetName()];
    if (!entry) {
        entry = pOutline;
        NotifyAdd<Outline::Base>(pOutline.get());
    }
    else
        throw std::invalid_argument("Scene already contains this outline.");
}

void Scene::RemoveMesh(std::string name) {
    Mesh* ptr = m_meshes.at(name).get();
    if (m_meshes.erase(name))
        NotifyRemove<Mesh>(ptr);
}

void Scene::RemoveSprite(std::string name) {
    Sprite* ptr = m_sprites.at(name).get();
    if (m_sprites.erase(name))
        NotifyRemove<Sprite>(ptr);
}

void Scene::RemoveText(std::string name) {
    Text* ptr = m_text.at(name).get();
    if (m_text.erase(name))
        NotifyRemove<Text>(ptr);
}

void Scene::RemoveOutline(std::string name) {
    Outline::Base* ptr = m_outlines.at(name).get();
    if (m_outlines.erase(name))
        NotifyRemove<Outline::Base>(ptr);
}

void Scene::RegisterSceneObserver(ISceneObserver* pSceneObserver) noexcept {
    m_sceneObservers.insert(pSceneObserver);
}

Camera& Scene::GetCamera() const noexcept {
    return m_camera;
}

const std::shared_ptr<Mesh>& Scene::GetMesh(std::string name) const {
    return m_meshes.at(name);
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

const std::unordered_map<std::string, std::shared_ptr<Mesh>>& Scene::GetMeshes() const noexcept {
    return m_meshes;
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
    for (const std::pair<std::string const, std::shared_ptr<Mesh>>& mesh : m_meshes) {
        count += mesh.second->GetInstances().size();
    } 
    return count;
}

std::uint64_t Scene::GetTotalVerticesLoaded() const noexcept {
    std::uint64_t count = 0;
    for (const std::pair<std::string const, std::shared_ptr<Mesh>>& mesh : m_meshes) {
        count += mesh.second->GetVertices().size();
    }
    return count;
}

std::uint64_t Scene::GetTotalVerticesRendered() const noexcept {
    std::uint64_t count = 0;
    for (const std::pair<std::string const, std::shared_ptr<Mesh>>& mesh : m_meshes) {
        count += mesh.second->GetVertices().size() * mesh.second->GetInstances().size();
    }
    return count;
}
