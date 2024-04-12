#include "RoX/Mesh.h"

Mesh::Mesh(
        const std::string name,
        const Material& material,
        const Type type,
        bool visible) 
    noexcept : m_name(name),
    m_material(material), m_type(type),
    m_culled(0), 
    m_instances({{
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            }}),
    m_vertices({}), m_indices({}),
    m_visible(visible)
{}

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

std::uint64_t Mesh::GetCulled() const noexcept {
    return m_culled;
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

void Mesh::SetCulled(std::uint64_t amount) {
    if (amount > m_instances.size()) {
        throw std::invalid_argument("Error culling '" + m_name + 
                "'; amount > instances; amount: " + std::to_string(amount) + 
                "; instances: " + std::to_string(m_instances.size()));
    }

    m_culled = amount;
}

void Mesh::SetVisible(bool visible) noexcept {
    m_visible = visible;
}
