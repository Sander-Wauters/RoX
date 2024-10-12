#pragma once

#include <RoX/Animation.h>

class ValidAnimation {
    public:
        ValidAnimation() noexcept;

    public:
        Keyframe NewKeyframe() const noexcept;
        BoneAnimation NewBoneAnimation() const noexcept;
        Animation NewAnimation() const noexcept;

    public:
        std::shared_ptr<Animation> pAnimation;
};
