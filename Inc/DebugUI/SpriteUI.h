#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Sprite.h>

namespace SpriteUI {
    void Position(Sprite& sprite);

    void Creator(AssetBatch& batch);

    void Menu(Sprite& sprite);
    void Menu(const Sprites& sprites); 

    void CreatorPopupMenu(AssetBatch& batch);
}

