#include "RoX/Outline.h"

// ---------------------------------------------------------------- //
//                          Outline
// ---------------------------------------------------------------- //

Outline::Outline(Type type, std::string name, DirectX::XMVECTOR color, bool visible) 
    noexcept : Identifiable("outline", name), 
    m_type(type),
    m_visible(visible)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

DirectX::XMFLOAT4& Outline::GetColor() noexcept {
    return m_color;
}

bool Outline::IsVisible() const noexcept {
    return m_visible;
}

Outline::Type Outline::GetType() const noexcept {
    return m_type;
}

void Outline::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          GridOutline
// ---------------------------------------------------------------- //

GridOutline::GridOutline(
        std::string name,
        std::uint16_t xDivisions,
        std::uint16_t yDivisions,
        DirectX::XMFLOAT3 xAxis, 
        DirectX::XMFLOAT3 yAxis, 
        DirectX::XMFLOAT3 origin,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Outline(Type::Grid, name, color, visible),
    m_xDivisions(xDivisions),
    m_yDivisions(yDivisions),
    m_xAxis(xAxis),
    m_yAxis(yAxis),
    m_origin(origin)
{}

std::uint16_t GridOutline::GetXDivisions() const noexcept {
    return m_xDivisions;
}

std::uint16_t GridOutline::GetYDivisions() const noexcept {
    return m_yDivisions;
}

DirectX::XMFLOAT3& GridOutline::GetXAxis() noexcept {
    return m_xAxis;
}

DirectX::XMFLOAT3& GridOutline::GetYAxis() noexcept {
    return m_yAxis;
}

DirectX::XMFLOAT3& GridOutline::GetOrigin() noexcept {
    return m_origin;
}

void GridOutline::SetXDivisions(std::uint16_t xDivsions) noexcept {
    m_xDivisions = xDivsions;
}

void GridOutline::SetYDivisions(std::uint16_t yDivsions) noexcept {
    m_yDivisions = yDivsions;
}

// ---------------------------------------------------------------- //
//                          RingOutline
// ---------------------------------------------------------------- //

RingOutline::RingOutline(
        std::string name,
        DirectX::XMFLOAT3 majorAxis,
        DirectX::XMFLOAT3 minorAxis,
        DirectX::XMFLOAT3 origin,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : Outline(Type::Ring, name, color, visible),
    m_majorAxis(majorAxis),
    m_minorAxis(minorAxis),
    m_origin(origin)
{}

DirectX::XMFLOAT3& RingOutline::GetMajorAxis() noexcept {
    return m_majorAxis;
}

DirectX::XMFLOAT3& RingOutline::GetMinorAxis() noexcept {
    return m_minorAxis;
}

DirectX::XMFLOAT3& RingOutline::GetOrigin() noexcept {
    return m_origin;
}

// ---------------------------------------------------------------- //
//                          RayOutline
// ---------------------------------------------------------------- //

RayOutline::RayOutline(std::string name,
        DirectX::XMFLOAT3 direction,
        DirectX::XMFLOAT3 origin,
        DirectX::XMVECTOR color,
        bool normalized,
        bool visible) 
    noexcept : Outline(Type::Ray, name, color, visible),
    m_direction(direction),
    m_origin(origin),
    m_normalized(normalized)
{}

DirectX::XMFLOAT3& RayOutline::GetDirection() noexcept {
    return m_direction;
}

DirectX::XMFLOAT3& RayOutline::GetOrigin() noexcept {
    return m_origin;
}

bool RayOutline::IsNormalized() const noexcept {
    return m_normalized;
}

void RayOutline::SetNormalized(bool normalized) noexcept {
    m_normalized = normalized;
}

// ---------------------------------------------------------------- //
//                          TriangleOutline
// ---------------------------------------------------------------- //

TriangleOutline::TriangleOutline(std::string name,
        DirectX::XMFLOAT3 pointA,
        DirectX::XMFLOAT3 pointB,
        DirectX::XMFLOAT3 pointC,
        DirectX::XMVECTOR color,
        bool visible)
    noexcept : Outline(Type::Triangle, name, color, visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC)
{}

DirectX::XMFLOAT3& TriangleOutline::GetPointA() noexcept {
    return m_pointA;
}

DirectX::XMFLOAT3& TriangleOutline::GetPointB() noexcept {
    return m_pointB;
}

DirectX::XMFLOAT3& TriangleOutline::GetPointC() noexcept {
    return m_pointC;
}

// ---------------------------------------------------------------- //
//                          QuadOutline
// ---------------------------------------------------------------- //

QuadOutline::QuadOutline(const std::string name,
        DirectX::XMFLOAT3 pointA,
        DirectX::XMFLOAT3 pointB,
        DirectX::XMFLOAT3 pointC,
        DirectX::XMFLOAT3 pointD,
        DirectX::XMVECTOR color,
        bool visible)
    noexcept : Outline(Type::Quad, name, color, visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC),
    m_pointD(pointD)
{}

DirectX::XMFLOAT3& QuadOutline::GetPointA() noexcept {
    return m_pointA;
}

DirectX::XMFLOAT3& QuadOutline::GetPointB() noexcept {
    return m_pointB;
}

DirectX::XMFLOAT3& QuadOutline::GetPointC() noexcept {
    return m_pointC;
}

DirectX::XMFLOAT3& QuadOutline::GetPointD() noexcept {
    return m_pointD;
}

