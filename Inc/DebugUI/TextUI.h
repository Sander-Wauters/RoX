#pragma once

#include <RoX/AssetBatch.h>
#include <RoX/Sprite.h>

namespace TextUI {
    void Creator(AssetBatch& batch);

    void Menu(Text& text);
    void Menu(const Texts& texts);

    void CreatorPopupMenu(AssetBatch& batch);
}

