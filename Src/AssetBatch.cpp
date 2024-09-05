#include "RoX/AssetBatch.h"

#include "Util/pch.h"

AssetBatch::AssetBatch() 
    noexcept : m_models({}),
    m_sprites({}),
    m_texts({}),
    m_outlines({})
{}

AssetBatch::~AssetBatch() noexcept 
{}

void AssetBatch::Add(std::shared_ptr<Model> pMesh) {
    std::shared_ptr<Model>& entry = m_models[pMesh->GetName()];
    if (!entry) 
        entry = pMesh;
    else
        throw std::invalid_argument("Scene already contains this mesh.");
}

void AssetBatch::Add(std::shared_ptr<Sprite> pSprite) {
    std::shared_ptr<Sprite>& entry = m_sprites[pSprite->GetName()];
    if (!entry)
        entry = pSprite;
    else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void AssetBatch::Add(std::shared_ptr<Text> pText) {
    std::shared_ptr<Text>& entry = m_texts[pText->GetName()];
    if (!entry) 
        entry = pText;
    else
        throw std::invalid_argument("Scene already contains this text.");
}

void AssetBatch::Add(std::shared_ptr<Outline> pOutline) {
    std::shared_ptr<Outline>& entry = m_outlines[pOutline->name];
    if (!entry) 
        entry = pOutline;
    else
        throw std::invalid_argument("Scene already contains this outline.");
}

void AssetBatch::RemoveMesh(std::string name) {
    m_models.erase(name);
}

void AssetBatch::RemoveSprite(std::string name) {
    m_sprites.erase(name);
}

void AssetBatch::RemoveText(std::string name) {
    m_texts.erase(name);
}

void AssetBatch::RemoveOutline(std::string name) {
    m_outlines.erase(name);
}

std::shared_ptr<Model>& AssetBatch::GetModel(std::string name) {
    return m_models.at(name);
}

std::shared_ptr<Sprite>& AssetBatch::GetSprite(std::string name) {
    return m_sprites.at(name);
}

std::shared_ptr<Text>& AssetBatch::GetText(std::string name) {
    return m_texts.at(name);
}

std::shared_ptr<Outline>& AssetBatch::GetOutline(std::string name) {
    return m_outlines.at(name); 
}

const std::unordered_map<std::string, std::shared_ptr<Model>>& AssetBatch::GetModels() const noexcept {
    return m_models;
}

const std::unordered_map<std::string, std::shared_ptr<Sprite>>& AssetBatch::GetSprites() const noexcept {
    return m_sprites;
}

const std::unordered_map<std::string, std::shared_ptr<Text>>& AssetBatch::GetTexts() const noexcept {
    return m_texts;
}

const std::unordered_map<std::string, std::shared_ptr<Outline>>& AssetBatch::GetOutlines() const noexcept {
    return m_outlines;
}

std::uint64_t AssetBatch::GetNumModels() const noexcept {
    return m_models.size();
}

std::uint64_t AssetBatch::GetNumMeshes() const noexcept {
    std::unordered_set<std::shared_ptr<IMesh>> uniqueMeshes;
    for (auto& modelPair : m_models) {
        for (std::shared_ptr<IMesh>& iMesh : modelPair.second->GetMeshes()) {
            uniqueMeshes.insert(iMesh);
        } 
    }
    return uniqueMeshes.size();
}

std::uint64_t AssetBatch::GetNumSubmeshes() const noexcept {
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

std::uint64_t AssetBatch::GetNumMaterials() const noexcept {
    std::unordered_set<std::string> materialNames;
    for (auto& modelPair : m_models) {
        for (std::shared_ptr<Material>& material : modelPair.second->GetMaterials()) {
            materialNames.insert(material->GetName());
        }
    }
    return materialNames.size();
}

std::uint64_t AssetBatch::GetNumSprites() const noexcept {
    return m_sprites.size();
}

std::uint64_t AssetBatch::GetNumTexts() const noexcept {
    return m_texts.size();
}

std::uint64_t AssetBatch::GetNumOutlines() const noexcept {
    return m_outlines.size();
}

std::uint64_t AssetBatch::GetNumSubmeshInstances() const noexcept {
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

std::uint64_t AssetBatch::GetNumRenderedSubmeshInstances() const noexcept {
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

std::uint64_t AssetBatch::GetNumLoadedVertices() const noexcept {
    std::uint64_t count = 0;
    for (auto& modelPair : m_models) {
        for (auto& pMesh : modelPair.second->GetMeshes()) {
            count += pMesh->GetNumVertices();
        }
    }
    return count;
}

std::uint64_t AssetBatch::GetNumRenderedVertices() const noexcept {
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
