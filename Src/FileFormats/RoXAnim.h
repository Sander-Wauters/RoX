#pragma once

#include "../Util/pch.h"

// Describes how an .roxanim is constructed.
// All arrays are stored in order.
//
//  ROXANIM::ANIM_HEADER
//      ROXANIM::BONE_ANIM_HEADER[ROXANIM::ANIM_HEADER.NumBoneAnimations] boneAnimation 
//          Keyframe[ROXANIM::BONE_ANIM_HEADER.NumKeyframes] keyframes
//

namespace ROXANIM {
#pragma pack(push, animHeader, 1)
    struct ANIM_HEADER {
        std::uint8_t NumBoneAnimations;
    };
#pragma pack(pop, animHeader)

    static_assert(sizeof(ANIM_HEADER) == 1, "ROXANIM::ANIM_HEADER size mismatch");

#pragma pack(push, boneAnimHeader, 4)
    struct BONE_ANIM_HEADER {
        std::uint32_t NumKeyframes;
        std::uint64_t KeyframeSizeInBytes;
    };
#pragma pack(pop, boneAnimHeader)

    static_assert(sizeof(BONE_ANIM_HEADER) == 12, "ROXANIM::BONE_ANIM_HEADER size mismatch");
}
