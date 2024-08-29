#pragma once
#include <vector>

#include "Model.h"

#include <DirectXMath.h>

struct Keyframe {
    Keyframe(
            float timePosition = 0.f, 
            DirectX::XMFLOAT3 translation = { 0.f, 0.f, 0.f }, 
            DirectX::XMFLOAT3 scale = { 1.f, 1.f, 1.f }, 
            DirectX::XMFLOAT4 rotationQuaternion = { 0.f, 0.f, 0.f, 0.f }
            ) noexcept;
    bool operator< (const Keyframe& other) const noexcept;;
    
    float TimePosition;
    DirectX::XMFLOAT3 Translation;
    DirectX::XMFLOAT3 Scale;
    DirectX::XMFLOAT4 RotationQuaternion;
};

struct BoneAnimation {
    void Interpolate(float timePosition, DirectX::XMMATRIX& M) const;

    boolean HasKeyframes() const noexcept;

    float GetStartTime() const;
    float GetEndTime() const;

    std::vector<Keyframe> Keyframes;
};

struct Animation {
    void Interpolate(float timePosition, Bone::TransformArray& boneTransforms) const;
    void Apply(float timePosition, Model& model) const;

    float GetStartTime() const;
    float GetEndTime() const;

    std::vector<BoneAnimation> BoneAnimations;
};
