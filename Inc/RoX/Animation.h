#pragma once
#include <vector>

#include "Model.h"

#include <DirectXMath.h>

// Describes the position scale and rotation of a transformation at a point in time.
// Does not enforce a given time unit.
// When comparing keyframes, be mindfull that both are using the same time unit.
struct Keyframe {
    Keyframe(
            float timePosition = 0.f, 
            DirectX::XMFLOAT3 translation = { 0.f, 0.f, 0.f }, 
            DirectX::XMFLOAT3 scale = { 1.f, 1.f, 1.f }, 
            DirectX::XMFLOAT4 rotationQuaternion = { 0.f, 0.f, 0.f, 0.f }
            ) noexcept;

    bool operator< (const Keyframe& other) const noexcept;
    bool operator> (const Keyframe& other) const noexcept;
    
    float TimePosition;
    DirectX::XMFLOAT3 Translation;
    DirectX::XMFLOAT3 Scale;
    DirectX::XMFLOAT4 RotationQuaternion;
};

// Hold the keyframes for the animation of a bone.
// All keyframes must be sorted in ascending order.
// At least 2 keyframes are needed to calculate the transformation at a given point in time.
struct BoneAnimation {
    void Interpolate(float timePosition, DirectX::XMMATRIX& M) const;

    float GetStartTime() const;
    float GetEndTime() const;

    std::uint32_t GetNumKeyframes() const noexcept;

    std::vector<Keyframe> Keyframes;
};

// Hold the animation data for a collection of bones.
// Bone animations must be sorted in the same way as the bone hierarchy.
struct Animation {
    void Interpolate(float timePosition, Bone::TransformArray& boneTransforms) const;
    void Apply(float timePosition, Model& model) const;

    float GetStartTime() const;
    float GetEndTime() const;

    std::uint32_t GetNumBoneAnimations() const noexcept;

    std::vector<BoneAnimation> BoneAnimations;
};
