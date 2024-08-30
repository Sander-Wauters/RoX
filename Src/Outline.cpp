#include "RoX/Outline.h"

// ---------------------------------------------------------------- //
//                          GridOutline
// ---------------------------------------------------------------- //

GridOutline::GridOutline(
        const std::string name,
        std::uint16_t xDivsions,
        std::uint16_t yDivsions,
        DirectX::XMVECTOR xAxis, 
        DirectX::XMVECTOR yAxis, 
        DirectX::XMVECTOR origin,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : m_name(name),
    m_color(color),
    m_visible(visible),
    m_xDivsions(xDivsions),
    m_yDivsions(yDivsions),
    m_xAxis(xAxis),
    m_yAxis(yAxis),
    m_origin(origin)
{}

std::string GridOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMVECTOR& GridOutline::GetColor() noexcept {
    return m_color;
}

bool GridOutline::IsVisible() const noexcept {
    return m_visible;
}

std::uint16_t GridOutline::GetXDivsions() const noexcept {
    return m_xDivsions;
}

std::uint16_t GridOutline::GetYDivsions() const noexcept {
    return m_yDivsions;
}

DirectX::XMVECTOR& GridOutline::GetXAxis() noexcept {
    return m_xAxis;
}

DirectX::XMVECTOR& GridOutline::GetYAxis() noexcept {
    return m_yAxis;
}

DirectX::XMVECTOR& GridOutline::GetOrigin() noexcept {
    return m_origin;
}

void GridOutline::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

void GridOutline::SetXDivisions(std::uint16_t xDivsions) noexcept {
    m_xDivsions = xDivsions;
}

void GridOutline::SetYDivisions(std::uint16_t yDivsions) noexcept {
    m_yDivsions = yDivsions;
}

// ---------------------------------------------------------------- //
//                          RingOutline
// ---------------------------------------------------------------- //

RingOutline::RingOutline(
        const std::string name,
        DirectX::XMVECTOR majorAxis,
        DirectX::XMVECTOR minorAxis,
        DirectX::XMVECTOR origin,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept: m_name(name),
    m_color(color),
    m_visible(visible),
    m_majorAxis(majorAxis),
    m_minorAxis(minorAxis),
    m_origin(origin)
{}

std::string RingOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMVECTOR& RingOutline::GetColor() noexcept {
    return m_color;
}

bool RingOutline::IsVisible() const noexcept {
    return m_visible;
}

DirectX::XMVECTOR& RingOutline::GetMajorAxis() noexcept {
    return m_majorAxis;
}

DirectX::XMVECTOR& RingOutline::GetMinorAxis() noexcept {
    return m_minorAxis;
}

DirectX::XMVECTOR& RingOutline::GetOrigin() noexcept {
    return m_origin;
}

void RingOutline::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          RayOutline
// ---------------------------------------------------------------- //

RayOutline::RayOutline(
        const std::string name,
        DirectX::XMVECTOR direction,
        DirectX::XMVECTOR origin,
        bool normalized,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : m_name(name),
    m_color(color),
    m_visible(visible),
    m_direction(direction),
    m_origin(origin),
    m_normalized(normalized)
{}

std::string RayOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMVECTOR& RayOutline::GetColor() noexcept {
    return m_color;
}

bool RayOutline::IsVisible() const noexcept {
    return m_visible;
}

DirectX::XMVECTOR& RayOutline::GetDirection() noexcept {
    return m_direction;
}

DirectX::XMVECTOR& RayOutline::GetOrigin() noexcept {
    return m_origin;
}

bool RayOutline::IsNormalized() const noexcept {
    return m_normalized;
}

void RayOutline::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

void RayOutline::SetNormalized(bool normalized) noexcept {
    m_normalized = normalized;
}

// ---------------------------------------------------------------- //
//                          TriangleOutline
// ---------------------------------------------------------------- //

TriangleOutline::TriangleOutline(
        const std::string name,
        DirectX::XMVECTOR pointA,
        DirectX::XMVECTOR pointB,
        DirectX::XMVECTOR pointC,
        DirectX::XMVECTOR color,
        bool visible)
    noexcept : m_name(name),
    m_color(color),
    m_visible(visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC)
{}

std::string TriangleOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMVECTOR& TriangleOutline::GetColor() noexcept {
    return m_color;
}

bool TriangleOutline::IsVisible() const noexcept {
    return m_visible;
}

DirectX::XMVECTOR& TriangleOutline::GetPointA() noexcept {
    return m_pointA;
}

DirectX::XMVECTOR& TriangleOutline::GetPointB() noexcept {
    return m_pointB;
}

DirectX::XMVECTOR& TriangleOutline::GetPointC() noexcept {
    return m_pointC;
}

void TriangleOutline::SetVisible(bool visible) noexcept {
    m_visible = visible;
}

// ---------------------------------------------------------------- //
//                          QuadOutline
// ---------------------------------------------------------------- //

QuadOutline::QuadOutline(
        const std::string name,
        DirectX::XMVECTOR pointA,
        DirectX::XMVECTOR pointB,
        DirectX::XMVECTOR pointC,
        DirectX::XMVECTOR pointD,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : m_name(name),
    m_color(color),
    m_visible(visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC),
    m_pointD(pointD)
{}

std::string QuadOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMVECTOR& QuadOutline::GetColor() noexcept {
    return m_color;
}

bool QuadOutline::IsVisible() const noexcept {
    return m_visible;
}

DirectX::XMVECTOR& QuadOutline::GetPointA() noexcept {
    return m_pointA;
}

DirectX::XMVECTOR& QuadOutline::GetPointB() noexcept {
    return m_pointB;
}

DirectX::XMVECTOR& QuadOutline::GetPointC() noexcept {
    return m_pointC;
}

DirectX::XMVECTOR& QuadOutline::GetPointD() noexcept {
    return m_pointD;
}

void QuadOutline::SetVisible(bool visible) noexcept  {
    m_visible = visible;
}

