#pragma once

#include "Model.h"
#include "Sprite.h"
#include "Outline.h"

// Mainly used internally but can be used by the client to execute code when an asset gets added and/or removed.
class IAssetBatchObserver {
    public:
        virtual ~IAssetBatchObserver() = default;

        virtual void OnAdd(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual void OnAdd(const std::shared_ptr<Model>& pModel) = 0;
        virtual void OnAdd(const std::shared_ptr<Sprite>& pSprite) = 0;
        virtual void OnAdd(const std::shared_ptr<Text>& pText) = 0;
        virtual void OnAdd(const std::shared_ptr<Outline>& pOutline) = 0;

        virtual void OnRemove(const std::shared_ptr<Material>& pMaterial) = 0;
        virtual void OnRemove(const std::shared_ptr<Model>& pModel) = 0;
        virtual void OnRemove(const std::shared_ptr<Sprite>& pSprite) = 0;
        virtual void OnRemove(const std::shared_ptr<Text>& pText) = 0;
        virtual void OnRemove(const std::shared_ptr<Outline>& pOutline) = 0;
};
