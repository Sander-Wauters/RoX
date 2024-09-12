#include "RoX/AssetBatch.h"

#include "Util/pch.h"

AssetBatch::AssetBatch(const std::string name, bool visible, std::uint8_t maxNumTextures) 
    noexcept : m_name(name),
    m_visible(visible),
    m_maxNumTextures(maxNumTextures)
{}

AssetBatch::~AssetBatch() noexcept 
{}

bool AssetBatch::operator== (const AssetBatch& other) const noexcept {
    return m_name == other.m_name;
}

std::uint64_t AssetBatch::FindGUID(std::string name, Materials& materials) {
    for (auto& pair : materials) {
        if (pair.second->GetName() == name)
            return pair.first;
    }
    return std::uint64_t(-1);
}

std::uint64_t AssetBatch::FindGUID(std::string name, Models& models) {
    for (auto& pair : models) {
        if (pair.second->GetName() == name)
            return pair.first;
    }
    return std::uint64_t(-1);
}

std::uint64_t AssetBatch::FindGUID(std::string name, Sprites& sprites) {
    for (auto& pair : sprites) {
        if (pair.second->GetName() == name)
            return pair.first;
    }
    return std::uint64_t(-1);
}

std::uint64_t AssetBatch::FindGUID(std::string name, Texts& texts) {
    for (auto& pair : texts) {
        if (pair.second->GetName() == name)
            return pair.first;
    }
    return std::uint64_t(-1);
}

std::uint64_t AssetBatch::FindGUID(std::string name, Outlines& outlines) {
    for (auto& pair : outlines) {
        if (pair.second->GetName() == name)
            return pair.first;
    }
    return std::uint64_t(-1);
}

void AssetBatch::Add(std::shared_ptr<Material> pMaterial) {
    std::shared_ptr<Material>& pMappedMaterial = m_materials[pMaterial->GetGUID()];
    if (!pMappedMaterial) {
        pMappedMaterial = pMaterial;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(pMaterial);
        }
    }
}

void AssetBatch::Add(std::shared_ptr<Model> pModel) {
    std::shared_ptr<Model>& entry = m_models[pModel->GetGUID()];
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
    std::shared_ptr<Sprite>& entry = m_sprites[pSprite->GetGUID()];
    if (!entry) {
        entry = pSprite;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this sprite.");
}

void AssetBatch::Add(std::shared_ptr<Text> pText) {
    std::shared_ptr<Text>& entry = m_texts[pText->GetGUID()];
    if (!entry) {
        entry = pText;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this text.");
}

void AssetBatch::Add(std::shared_ptr<Outline> pOutline) {
    std::shared_ptr<Outline>& entry = m_outlines[pOutline->GetGUID()];
    if (!entry) {
        entry = pOutline;
        for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
            pAssetBatchObserver->OnAdd(entry);
        }
    } else
        throw std::invalid_argument("Scene already contains this outline.");
}

void AssetBatch::RemoveMaterial(std::uint64_t GUID) {
    for (auto& modelPair : m_models) {
        std::vector<std::shared_ptr<Material>>& materials = modelPair.second->GetMaterials();
        for (std::shared_ptr<Material>& pMaterial : materials) {
            if (pMaterial->GetGUID() == GUID)
                throw std::runtime_error("Failed to remove material '" + pMaterial->GetName() + "' GUID:" + std::to_string(GUID) + " because it is still in use.");
        }
    }

    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_materials.at(GUID));
    }
    m_materials.erase(GUID);
}

void AssetBatch::RemoveModel(std::uint64_t GUID) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_models.at(GUID));
    }
    m_models.erase(GUID);
}

void AssetBatch::RemoveSprite(std::uint64_t GUID) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_sprites.at(GUID));
    }
    m_sprites.erase(GUID);
}

void AssetBatch::RemoveText(std::uint64_t GUID) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_texts.at(GUID));
    }
    m_texts.erase(GUID);
}

void AssetBatch::RemoveOutline(std::uint64_t GUID) {
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_outlines.at(GUID));
    }
    m_outlines.erase(GUID);
}

void AssetBatch::RemoveMaterial(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_materials);
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_materials.at(GUID));
    }
    m_materials.erase(GUID);
}

void AssetBatch::RemoveModel(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_models);
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_models.at(GUID));
    }
    m_models.erase(GUID);
}

void AssetBatch::RemoveSprite(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_sprites);
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_sprites.at(GUID));
    }
    m_sprites.erase(GUID);
}

void AssetBatch::RemoveText(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_texts);
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_texts.at(GUID));
    }
    m_texts.erase(GUID);
}

void AssetBatch::RemoveOutline(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_outlines);
    for (IAssetBatchObserver* pAssetBatchObserver : m_assetBatchObservers) {
        pAssetBatchObserver->OnRemove(m_outlines.at(GUID));
    }
    m_outlines.erase(GUID);
}

void AssetBatch::RegisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept {
    m_assetBatchObservers.insert(assetBatchObserver);
}

void AssetBatch::DeregisterAssetBatchObserver(IAssetBatchObserver* assetBatchObserver) noexcept {
    m_assetBatchObservers.erase(assetBatchObserver);
}

std::string AssetBatch::GetName() const noexcept {
    return m_name;
}

bool AssetBatch::IsVisible() const noexcept {
    return m_visible;
}

std::uint8_t AssetBatch::GetMaxNumTextures() const noexcept {
    return m_maxNumTextures;
}

std::shared_ptr<Material>& AssetBatch::GetMaterial(std::uint64_t GUID) {
    return m_materials.at(GUID);
}

std::shared_ptr<Model>& AssetBatch::GetModel(std::uint64_t GUID) {
    return m_models.at(GUID);
}

std::shared_ptr<Sprite>& AssetBatch::GetSprite(std::uint64_t GUID) {
    return m_sprites.at(GUID);
}

std::shared_ptr<Text>& AssetBatch::GetText(std::uint64_t GUID) {
    return m_texts.at(GUID);
}

std::shared_ptr<Outline>& AssetBatch::GetOutline(std::uint64_t GUID) {
    return m_outlines.at(GUID);
}

std::shared_ptr<Material>& AssetBatch::GetMaterial(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_materials);
    return m_materials.at(GUID);
}

std::shared_ptr<Model>& AssetBatch::GetModel(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_models);
    return m_models.at(GUID);
}

std::shared_ptr<Sprite>& AssetBatch::GetSprite(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_sprites);
    return m_sprites.at(GUID);
}

std::shared_ptr<Text>& AssetBatch::GetText(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_texts);
    return m_texts.at(GUID);
}

std::shared_ptr<Outline>& AssetBatch::GetOutline(std::string name) {
    std::uint64_t GUID = FindGUID(name, m_outlines);
    return m_outlines.at(GUID); 
}

const Materials& AssetBatch::GetMaterials() const noexcept {
    return m_materials;
}

const Models& AssetBatch::GetModels() const noexcept {
    return m_models;
}

const Sprites& AssetBatch::GetSprites() const noexcept {
    return m_sprites;
}

const Texts& AssetBatch::GetTexts() const noexcept {
    return m_texts;
}

const Outlines& AssetBatch::GetOutlines() const noexcept {
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

