#pragma once

#include "../Util/pch.h"

// Describes how an .roxmodl is constructed.
// All arrays are stored in order.
//
//  ROXMODL::HEADER
//      char[ROXMODL::HEADER.NameSizeInbytes] modelName
//  ROXMODL::BONE[ROXMODL::HEADER.NumBones]
//      char[ROXMODL::BONE.NameSizeInbytes] boneName
//  DirectX::XMFLOAT4X4[ROXMODL::HEADER.NumBones] boneMatrices
//  DirectX::XMFLOAT4X4[ROXMODL::HEADER.NumBones] inverseBoneMatrices
//  ROXMODL::MESH_HEADER[ROXMODL::HEADER::NumMeshes]
//      char[ROXMODL::MESH_HEADER.NameSizeInbytes] meshName
//      std::uint32_t[ROXMODL::MESH_HEADER.NumBoneInfluences] boneInfluences
//      ROXMODL::SUBMESH[ROXMODL::MESH_HEADER.NumSubmeshes] submeshes
//          char[ROXMODL::SUBMESH.NameSizeInBytes] submeshName
//      ROXMODL::INDEX_BUFFER_HEADER
//          void[ROXMODL::INDEX_BUFFER_HEADER.IndexSizeInBytes * ROXMODL::INDEX_BUFFER_HEADER.NumIndices] indices
//      ROXMODL::VERTEX_BUFFER_HEADER
//          void[ROXMODL::VERTEX_BUFFER_HEADER.VertexSizeInBytes * ROXMODL::VERTEX_BUFFER_HEADER.NumVertices] vertices
//

namespace ROXMODL {
#pragma pack(push, header, 2)
    struct HEADER {
        std::uint16_t Version; 

        std::uint32_t NameSizeInBytes;

        std::uint32_t NumBones;
        std::uint32_t NumMeshes;
        std::uint32_t NumMaterials;
    };
#pragma pack(pop, header)

    static_assert(sizeof(HEADER) == 18, "ROXMODL::HEADER size mismatch");

#pragma pack(push, boneHeader, 8)
    struct BONE_HEADER {
        std::uint32_t NameSizeInBytes;
        std::uint32_t ParentIndex;
    };
#pragma pack(pop, boneHeader)

    static_assert(sizeof(BONE_HEADER) == 8, "ROXMODL::BONE_HEADER size mismatch");

#pragma pack(push, meshHeader, 1)
    struct MESH_HEADER {
        bool IsSkinned;
        std::uint32_t NameSizeInBytes;

        std::uint32_t NumBoneInfluences;
        std::uint32_t NumSubmeshes;
    };
#pragma pack(pop, meshHeader)

    static_assert(sizeof(MESH_HEADER) == 13, "ROXMODL::MESH_HEADER size mismatch");

#pragma pack(push, submeshHeader, 4)
    struct SUBMESH_HEADER {
        std::uint32_t NameSizeInBytes;
        std::uint32_t MaterialIndex; 
        std::uint32_t IndexCount;
        std::uint32_t StartIndex;
        std::uint32_t VertexOffset;
    };
#pragma pack(pop, submeshHeader)

    static_assert(sizeof(SUBMESH_HEADER) == 20, "ROXMODL::SUBMESH_HEADER size mismatch");

#pragma pack(push, indexBufferHeader, 4)
    struct INDEX_BUFFER_HEADER {
        std::uint32_t IndexSizeInBytes;

        std::uint64_t NumIndices;
    };
#pragma pack(pop, indexBufferHeader)

    static_assert(sizeof(INDEX_BUFFER_HEADER) == 12, "ROXMODL::INDEX_BUFFER_HEADER size mismatch");

#pragma pack(push, vertexBufferHeader, 4)
    struct VERTEX_BUFFER_HEADER {
        std::uint32_t VertexSizeInBytes;

        std::uint64_t NumVertices;
    };
#pragma pack(pop, vertexBufferHeader)

    static_assert(sizeof(VERTEX_BUFFER_HEADER) == 12, "ROXMODL::VERTEX_BUFFER_HEADER size mismatch");
}

