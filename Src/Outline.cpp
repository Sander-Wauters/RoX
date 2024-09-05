#include "RoX/Outline.h"


Outline::Outline(const std::string iname, DirectX::XMVECTOR icolor, bool ivisible) 
    noexcept : name(iname), 
    visible(ivisible)
{
    DirectX::XMStoreFloat4(&color, icolor);
}

GridOutline::GridOutline(
        const std::string iname,
        std::uint16_t ixDivisions,
        std::uint16_t iyDivisions,
        DirectX::XMFLOAT3 ixAxis, 
        DirectX::XMFLOAT3 iyAxis, 
        DirectX::XMFLOAT3 iorigin,
        DirectX::XMVECTOR icolor,
        bool ivisible) 
    noexcept : Outline(iname, icolor, ivisible),
    xDivisions(ixDivisions),
    yDivisions(iyDivisions),
    xAxis(ixAxis),
    yAxis(iyAxis),
    origin(iorigin)
{}

RingOutline::RingOutline(
        const std::string iname,
        DirectX::XMFLOAT3 imajorAxis,
        DirectX::XMFLOAT3 iminorAxis,
        DirectX::XMFLOAT3 iorigin,
        DirectX::XMVECTOR icolor,
        bool ivisible) 
    noexcept : Outline(iname, icolor, ivisible),
    majorAxis(imajorAxis),
    minorAxis(iminorAxis),
    origin(iorigin)
{}

RayOutline::RayOutline(const std::string iname,
        DirectX::XMFLOAT3 idirection,
        DirectX::XMFLOAT3 iorigin,
        bool inormalized,
        DirectX::XMVECTOR icolor,
        bool ivisible) 
    noexcept : Outline(iname, icolor, ivisible),
    direction(idirection),
    origin(iorigin),
    normalized(inormalized)
{}

TriangleOutline::TriangleOutline(const std::string iname,
        DirectX::XMFLOAT3 ipointA,
        DirectX::XMFLOAT3 ipointB,
        DirectX::XMFLOAT3 ipointC,
        DirectX::XMVECTOR icolor,
        bool ivisible)
    noexcept : Outline(iname, icolor, ivisible),
    pointA(ipointA),
    pointB(ipointB),
    pointC(ipointC)
{}

QuadOutline::QuadOutline(const std::string iname,
        DirectX::XMFLOAT3 ipointA,
        DirectX::XMFLOAT3 ipointB,
        DirectX::XMFLOAT3 ipointC,
        DirectX::XMFLOAT3 ipointD,
        DirectX::XMVECTOR icolor,
        bool ivisible)
    noexcept : Outline(iname, icolor, ivisible),
    pointA(ipointA),
    pointB(ipointB),
    pointC(ipointC),
    pointD(ipointD)
{}

/*
// ---------------------------------------------------------------- //
//                          GridOutline
// ---------------------------------------------------------------- //

GridOutline::GridOutline(
        const std::string name,
        std::uint16_t xDivsions,
        std::uint16_t yDivsions,
        DirectX::XMFLOAT3 xAxis, 
        DirectX::XMFLOAT3 yAxis, 
        DirectX::XMFLOAT3 origin,
        DirectX::XMVECTOR color,
        bool visible) 
    noexcept : m_name(name),
    m_visible(visible),
    m_xDivsions(xDivsions),
    m_yDivsions(yDivsions),
    m_xAxis(xAxis),
    m_yAxis(yAxis),
    m_origin(origin)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

std::string GridOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMFLOAT4& GridOutline::GetColor() noexcept {
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

DirectX::XMFLOAT3& GridOutline::GetXAxis() noexcept {
    return m_xAxis;
}

DirectX::XMFLOAT3& GridOutline::GetYAxis() noexcept {
    return m_yAxis;
}

DirectX::XMFLOAT3& GridOutline::GetOrigin() noexcept {
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
    m_visible(visible),
    m_majorAxis(majorAxis),
    m_minorAxis(minorAxis),
    m_origin(origin)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

std::string RingOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMFLOAT4& RingOutline::GetColor() noexcept {
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
    m_visible(visible),
    m_direction(direction),
    m_origin(origin),
    m_normalized(normalized)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

std::string RayOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMFLOAT4& RayOutline::GetColor() noexcept {
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
    m_visible(visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

std::string TriangleOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMFLOAT4& TriangleOutline::GetColor() noexcept {
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
    m_visible(visible),
    m_pointA(pointA),
    m_pointB(pointB),
    m_pointC(pointC),
    m_pointD(pointD)
{
    DirectX::XMStoreFloat4(&m_color, color);
}

std::string QuadOutline::GetName() const noexcept {
    return m_name;
}

DirectX::XMFLOAT4& QuadOutline::GetColor() noexcept {
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
*/
