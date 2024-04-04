#include "RoX/Scene.h"

Scene::Scene(Camera& camera) 
    noexcept : m_camera(camera),
    m_meshes({}),
    m_materials({}),
    m_sprites({}),
    m_text({}),
    m_outlines({}),
    m_sceneObservers({})
{}

Scene::~Scene() noexcept {
}

void Scene::Add(std::shared_ptr<Mesh> pMesh) {
    if (m_meshes.find(pMesh->GetName()) == m_meshes.end()) {
        m_meshes[pMesh->GetName()] = pMesh;
        NotifyAdd<Mesh>(pMesh.get());
    }
    else
        throw std::invalid_argument("Scene already contains this mesh.");
}

void Scene::Add(std::shared_ptr<Sprite> pSprite) {
    if (m_sprites.find(pSprite->GetName()) == m_sprites.end()) {
        m_sprites[pSprite->GetName()] = pSprite;
        NotifyAdd<Sprite>(pSprite.get());
    }
    else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void Scene::Add(std::shared_ptr<Text> pText) {
    if (m_text.find(pText->GetName()) == m_text.end()) {
        m_text[pText->GetName()] = pText;
        NotifyAdd<Text>(pText.get());
    }
    else
        throw std::invalid_argument("Scene already contains this text.");
}

void Scene::Add(std::shared_ptr<Outline::Base> pOutline) {
    if (m_outlines.find(pOutline->GetName()) == m_outlines.end()) {
        m_outlines[pOutline->GetName()] = pOutline;
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

