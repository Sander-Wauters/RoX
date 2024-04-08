#pragma once

#include <unordered_map>

#include "../../Lib/DirectXTK12/Inc/VertexTypes.h"

#include "../../Src/Util/pch.h"

#include "Material.h"

class Mesh {
    public:
        enum class Type {
            CUBE,
            BOX,
            SPHERE,
            GEO_SPHERE,
            CYLINDER,
            CONE,
            TORUS,
            TETRAHEDRON,
            OCTAHEDRON,
            DODECAHEDRON,
            ICOSAHEDRON,
            CUSTOM
        };

    public:
        Mesh(
                const std::string name,
                const Material& material,
                const Type type = Type::CUSTOM,
                bool visible = true
            ) noexcept;
        ~Mesh() noexcept;

    public:
        std::string GetName() const noexcept;
        const Material& GetMaterial() const noexcept;
        Type GetType() const noexcept;

        std::vector<DirectX::XMFLOAT3X4>& GetInstances() noexcept;
        std::vector<DirectX::VertexPositionNormalTexture>& GetVertices() noexcept; 
        std::vector<std::uint16_t>& GetIndices() noexcept;

        bool IsVisible() const noexcept;

        void SetVisible(bool visible) noexcept;
    
    private:
        const std::string m_name;
        const Material& m_material;
        const Type m_type;

        std::vector<DirectX::XMFLOAT3X4> m_instances;
        std::vector<DirectX::VertexPositionNormalTexture> m_vertices; 
        std::vector<std::uint16_t> m_indices;

        bool m_visible;
};
