#include "RoX/Mesh.h"

Mesh::Mesh(
        const std::string name,
        const Material& material,
        const Type type,
        bool visible
    ) noexcept : m_name(name),
    m_material(material),
    m_type(type),
    m_visible(visible)
{
    m_instances = {{
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
    }};
}

Mesh::~Mesh() noexcept {
}

std::string Mesh::GetName() const noexcept {
    return m_name;
}

const Material& Mesh::GetMaterial() const noexcept {
    return m_material;
}

Mesh::Type Mesh::GetType() const noexcept {
    return m_type;
}

std::vector<DirectX::XMFLOAT3X4>& Mesh::GetInstances() noexcept {
    return m_instances;
}

std::vector<DirectX::VertexPositionNormalTexture>& Mesh::GetVertices() noexcept {
    return m_vertices;
}
 
std::vector<std::uint16_t>& Mesh::GetIndices() noexcept {
    return m_indices;
}

bool Mesh::IsVisible() const noexcept {
    return m_visible;
}

void Mesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}
