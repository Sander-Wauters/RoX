#include "RoX/Animation.h"

Keyframe::Keyframe(float timePosition, DirectX::XMFLOAT3 translation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT4 rotationQuaternion) 
    noexcept : TimePosition(timePosition),
    Translation(translation),
    Scale(scale),
    RotationQuaternion(rotationQuaternion)
{}

bool Keyframe::operator< (const Keyframe& other) const noexcept {
    return TimePosition < other.TimePosition;
}

void BoneAnimation::Interpolate(float timePosition, DirectX::XMMATRIX& M) const {
    DirectX::XMVECTOR zero = DirectX::XMVectorSet(0.f, 0.f, 0.f, 1.f);

    if (timePosition <= GetStartTime()) {
        DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&Keyframes.front().Scale);
        DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&Keyframes.front().Translation);
        DirectX::XMVECTOR R = DirectX::XMLoadFloat4(&Keyframes.front().RotationQuaternion);

        //DirectX::XMMATRIX s = DirectX::XMMatrixScalingFromVector(S);
        //DirectX::XMMATRIX t = DirectX::XMMatrixTranslationFromVector(T);
        //DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(R);

        //M = r * s * t;

        M = DirectX::XMMatrixAffineTransformation(S, zero, R, T);
    } else if (timePosition >= GetEndTime()) {
        DirectX::XMVECTOR S = DirectX::XMLoadFloat3(&Keyframes.back().Scale);
        DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&Keyframes.back().Translation);
        DirectX::XMVECTOR R = DirectX::XMLoadFloat4(&Keyframes.back().RotationQuaternion);

        //DirectX::XMMATRIX s = DirectX::XMMatrixScalingFromVector(S);
        //DirectX::XMMATRIX t = DirectX::XMMatrixTranslationFromVector(T);
        //DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(R);

        //M = r * s * t;

        M = DirectX::XMMatrixAffineTransformation(S, zero, R, T);
    } else {
        for(std::uint32_t i = 0; i < Keyframes.size() - 1; ++i) {
            const Keyframe& currentFrame = Keyframes[i];
            const Keyframe& nextFrame = Keyframes[i + 1];

            if (timePosition >= currentFrame.TimePosition && timePosition <= nextFrame.TimePosition) {
                float lerpPercent = (timePosition - currentFrame.TimePosition) / (nextFrame.TimePosition - currentFrame.TimePosition);

                DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&currentFrame.Scale);
                DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&nextFrame.Scale);

                DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&currentFrame.Translation);
                DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&nextFrame.Translation);

                DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&currentFrame.RotationQuaternion);
                DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&nextFrame.RotationQuaternion);

                DirectX::XMVECTOR S =  DirectX::XMVectorLerp(s0, s1, lerpPercent);
                DirectX::XMVECTOR T =  DirectX::XMVectorLerp(p0, p1, lerpPercent);
                DirectX::XMVECTOR R =  DirectX::XMQuaternionSlerp(q0, q1, lerpPercent);

                //DirectX::XMMATRIX s = DirectX::XMMatrixScalingFromVector(S);
                //DirectX::XMMATRIX t = DirectX::XMMatrixTranslationFromVector(T);
                //DirectX::XMMATRIX r = DirectX::XMMatrixRotationQuaternion(R);

                //M = r * s * t;

                M = DirectX::XMMatrixAffineTransformation(S, zero, R, T);

                break;
            }
        }
    }
}

boolean BoneAnimation::HasKeyframes() const noexcept {
    return Keyframes.size() >= 2;
}

float BoneAnimation::GetStartTime() const {
    return Keyframes.front().TimePosition;
}

float BoneAnimation::GetEndTime() const {
    return Keyframes.back().TimePosition;
}

void Animation::Interpolate(float timePosition, Bone::TransformArray& boneTransforms) const {
    for (std::uint32_t i = 0; i < BoneAnimations.size(); ++i) {
        if (BoneAnimations[i].HasKeyframes())
            BoneAnimations[i].Interpolate(timePosition, boneTransforms[i]);
    }
}

void Animation::Apply(float timePosition, Model& model) const {
    Bone::TransformArray toParentTransforms = Bone::MakeArray(model.GetNumBones());
    Interpolate(timePosition, toParentTransforms);

    Bone::TransformArray toRootTransforms = Bone::MakeArray(model.GetNumBones());
    toRootTransforms[0] = toParentTransforms[0];

    for (std::uint32_t i = 1; i < model.GetNumBones(); ++i) {
        DirectX::XMMATRIX toParent = toParentTransforms[i];
        DirectX::XMMATRIX parentToRoot = toRootTransforms[model.GetBones()[i].GetParentIndex()];
        DirectX::XMMATRIX toRoot = toParent * parentToRoot;
        toRootTransforms[i] = toRoot;
    }

    for (std::uint32_t i = 0; i < model.GetNumBones(); ++i) {
        DirectX::XMMATRIX offset = model.GetInverseBindPoseMatrices()[i];
        DirectX::XMMATRIX toRoot = toRootTransforms[i];
        DirectX::XMMATRIX final = offset * toRoot;

        model.GetBoneMatrices()[i] = final;
    }
}

float Animation::GetStartTime() const {
    float startTime = 0.f;
    for (std::uint32_t i = 0; i < BoneAnimations.size(); ++i) {
        if (BoneAnimations[i].HasKeyframes()) {
            startTime = BoneAnimations[i].GetStartTime();
            break;
        }
    }
    return startTime;
}

float Animation::GetEndTime() const {
    float endTime = 0.f;
    for (std::int32_t i = BoneAnimations.size() - 1; i >= 0; --i) {
        if (BoneAnimations[i].HasKeyframes()) {
            endTime = BoneAnimations[i].GetEndTime();
            break;
        }
    }
    return endTime;
}
