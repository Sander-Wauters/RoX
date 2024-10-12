#include "ValidAnimation.h"


ValidAnimation::ValidAnimation() noexcept {
    pAnimation = std::make_shared<Animation>(NewAnimation());
}

Keyframe ValidAnimation::NewKeyframe() const noexcept {
    Keyframe kf;
    kf.TimePosition = 0.f;
    kf.Translation = { 1.f, 2.f, 3.f };
    kf.Scale = { 4.f, 5.f, 6.f };
    kf.RotationQuaternion = { 7.f, 8.f, 9.f, 10.f };

    return kf;
}

BoneAnimation ValidAnimation::NewBoneAnimation() const noexcept {
    BoneAnimation ba;
    ba.Keyframes.push_back(NewKeyframe());
    ba.Keyframes.push_back(NewKeyframe());
    return ba;
}

Animation ValidAnimation::NewAnimation() const noexcept {
    Animation a;
    a.BoneAnimations.push_back(NewBoneAnimation());
    a.BoneAnimations.push_back(NewBoneAnimation());
    return a;
}

