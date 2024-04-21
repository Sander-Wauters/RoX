#pragma once

#include "Mesh.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"

class ISceneObserver {
    public:
        virtual void OnAdd(std::shared_ptr<Mesh> pMesh) = 0;
        virtual void OnAdd(std::shared_ptr<Sprite> pSprite) = 0;
        virtual void OnAdd(std::shared_ptr<Text> pText) = 0;
        virtual void OnAdd(std::shared_ptr<Outline::Base> pOutline) = 0;

        virtual void OnRemove(std::shared_ptr<Mesh> pMesh) = 0;
        virtual void OnRemove(std::shared_ptr<Sprite> pSprite) = 0;
        virtual void OnRemove(std::shared_ptr<Text> pText) = 0;
        virtual void OnRemove(std::shared_ptr<Outline::Base> pOutline) = 0;
};
