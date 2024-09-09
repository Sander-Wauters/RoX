#include "RoX/AssetBatch.h"

#include "Util/pch.h"

AssetBatch::AssetBatch(const std::string name, bool visible, std::uint8_t maxAssets) 
    noexcept : m_name(name),
    m_visible(visible),
    m_maxAssets(maxAssets)
{}

AssetBatch::~AssetBatch() noexcept 
{}

bool AssetBatch::operator== (const AssetBatch& other) const noexcept {
    return m_name == other.m_name;
}

void AssetBatch::Add(std::shared_ptr<Material> pMaterial) {
    std::shared_ptr<Material>& pMappedMaterial = m_materials[pMaterial->GetName()];
    if (!pMappedMaterial) {
        pMappedMaterial = pMaterial;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(pMaterial);
        }
    }
}

void AssetBatch::Add(std::shared_ptr<Model> pModel) {
    std::shared_ptr<Model>& entry = m_models[pModel->GetName()];
    if (!entry) {
        entry = pModel;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
        // Material can be shared so check if they aren't already loaded in.
        for (std::uint8_t i = 0; i < pModel->GetNumMaterials(); ++i) {
            Add(pModel->GetMaterials()[i]);
        }
    } else
        throw std::invalid_argument("Scene already contains this mesh.");
}

void AssetBatch::Add(std::shared_ptr<Sprite> pSprite) {
    std::shared_ptr<Sprite>& entry = m_sprites[pSprite->GetName()];
    if (!entry) {
        entry = pSprite;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void AssetBatch::Add(std::shared_ptr<Text> pText) {
    std::shared_ptr<Text>& entry = m_texts[pText->GetName()];
    if (!entry) {
        entry = pText;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this text.");
}

void AssetBatch::Add(std::shared_ptr<Outline> pOutline) {
    std::shared_ptr<Outline>& entry = m_outlines[pOutline->name];
    if (!entry) {
        entry = pOutline;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this outline.");
}

void AssetBatch::RemoveMaterial(std::string name) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_materials.at(name));
    }
    m_materials.erase(name);
}

void AssetBatch::RemoveModel(std::string name) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_models.at(name));
    }
    m_models.erase(name);
}

void AssetBatch::RemoveSprite(std::string name) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_sprites.at(name));
    }
    m_sprites.erase(name);
}

void AssetBatch::RemoveText(std::string name) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_texts.at(name));
    }
    m_texts.erase(name);
}

void AssetBatch::RemoveOutline(std::string name) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_outlines.at(name));
    }
    m_outlines.erase(name);
}

void AssetBatch::RegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept {
    m_assetBatchObservers.insert(assetBatchObserver);
}

void AssetBatch::DeRegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept {
    m_assetBatchObservers.erase(assetBatchObserver);
}

std::string AssetBatch::GetName() const noexcept {
    return m_name;
}

bool AssetBatch::IsVisible() const noexcept {
    return m_visible;
}

std::uint8_t AssetBatch::GetMaxAssets() const noexcept {
    return m_maxAssets;
}

std::shared_ptr<Material>& AssetBatch::GetMaterial(std::string name) {
    return m_materials.at(name);
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

const std::unordered_map<std::string, std::shared_ptr<Material>> AssetBatch::GetMaterials() const noexcept {
    return m_materials;
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


std::uint64_t AssetBatch::GetNumMaterials() const noexcept {
    return m_materials.size();
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

void AssetBatch::SetVisible(bool visible) {
    m_visible = visible;
}

