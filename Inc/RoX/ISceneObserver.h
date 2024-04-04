#pragma once

#include "Mesh.h"
#include "Sprite.h"
#include "Text.h"
#include "Outline.h"

class ISceneObserver {
    public:
        virtual void OnAdd(Mesh* pMesh) = 0;
        virtual void OnAdd(Sprite* pSprite) = 0;
        virtual void OnAdd(Text* pText) = 0;
        virtual void OnAdd(Outline::Base* pOutline) = 0;

        virtual void OnRemove(Mesh* pMesh) = 0;
        virtual void OnRemove(Sprite* pSprite) = 0;
        virtual void OnRemove(Text* pText) = 0;
        virtual void OnRemove(Outline::Base* pOutline) = 0;
};
