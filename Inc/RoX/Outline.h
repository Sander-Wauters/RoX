#pragma once

#include <string>
#include <type_traits>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>


// Collection of structs that hold data to renderer the outline of shape.

// Abstract struct representing the outline of a shape.
struct Outline {
    protected:
        Outline(const std::string iname, DirectX::XMVECTOR icolor = DirectX::Colors::White, bool ivisible = true) noexcept;

    public:
        virtual ~Outline() = default;

        const std::string name;
        DirectX::XMFLOAT4 color;
        bool visible;
};

// Outlines a boundind body in **DirectXCollision**.
template<typename Bounds> struct BoundingBodyOutline : public Outline {
    static_assert(
            std::is_base_of<DirectX::BoundingBox,         Bounds>::value ||
            std::is_base_of<DirectX::BoundingFrustum,     Bounds>::value ||
            std::is_base_of<DirectX::BoundingOrientedBox, Bounds>::value ||
            std::is_base_of<DirectX::BoundingSphere,      Bounds>::value,
            "Bounds must either be a DirectX::BoundingBox, DirectX::BoundingFrustum, DirectX::BoundingOrientedBox or DirectX::BoundingSphere"
            );

    BoundingBodyOutline(const std::string iname, Bounds& iboundingBody, DirectX::XMVECTOR icolor = DirectX::Colors::White, bool ivisible = true) noexcept
        : Outline(iname, icolor, ivisible), boundingBody(iboundingBody)
        {}


    Bounds& boundingBody;
};

struct GridOutline : public Outline {
    GridOutline(const std::string iname,
            std::uint16_t ixDivisions,
            std::uint16_t iyDivisions,
            DirectX::XMFLOAT3 ixAxis, 
            DirectX::XMFLOAT3 iyAxis, 
            DirectX::XMFLOAT3 iorigin = { 0.f, 0.f, 0.f },
            DirectX::XMVECTOR icolor = DirectX::Colors::White,
            bool ivisible = true) noexcept;

    std::uint16_t xDivisions;
    std::uint16_t yDivisions;

    DirectX::XMFLOAT3 xAxis;
    DirectX::XMFLOAT3 yAxis;
    DirectX::XMFLOAT3 origin;
};

struct RingOutline : public Outline {
    RingOutline(const std::string iname,
            DirectX::XMFLOAT3 imajorAxis,
            DirectX::XMFLOAT3 iminorAxis,
            DirectX::XMFLOAT3 iorigin = { 0.f, 0.f, 0.f },
            DirectX::XMVECTOR icolor = DirectX::Colors::White,
            bool ivisible = true) noexcept;


    DirectX::XMFLOAT3 majorAxis;
    DirectX::XMFLOAT3 minorAxis;
    DirectX::XMFLOAT3 origin;
};

struct RayOutline : public Outline {
    RayOutline(const std::string iname,
            DirectX::XMFLOAT3 idirection,
            DirectX::XMFLOAT3 iorigin = { 0.f, 0.f, 0.f },
            bool inormalized = false,
            DirectX::XMVECTOR icolor = DirectX::Colors::White,
            bool ivisible = true) noexcept;

    DirectX::XMFLOAT3 direction;
    DirectX::XMFLOAT3 origin;
    bool normalized;
};

struct TriangleOutline : public Outline {
    TriangleOutline(const std::string iname,
            DirectX::XMFLOAT3 ipointA,
            DirectX::XMFLOAT3 ipointB,
            DirectX::XMFLOAT3 ipointC,
            DirectX::XMVECTOR icolor = DirectX::Colors::White,
            bool ivisible = true) noexcept;

    DirectX::XMFLOAT3 pointA;
    DirectX::XMFLOAT3 pointB;
    DirectX::XMFLOAT3 pointC;
};

struct QuadOutline : public Outline {
    QuadOutline(const std::string iname,
            DirectX::XMFLOAT3 ipointA,
            DirectX::XMFLOAT3 ipointB,
            DirectX::XMFLOAT3 ipointC,
            DirectX::XMFLOAT3 ipointD,
            DirectX::XMVECTOR icolor = DirectX::Colors::White,
            bool ivisible = true) noexcept;

    DirectX::XMFLOAT3 pointA;
    DirectX::XMFLOAT3 pointB;
    DirectX::XMFLOAT3 pointC;
    DirectX::XMFLOAT3 pointD;
};


/*
// Collection of classes that hold data to renderer the outline of shape.

// Abstract interface representing the outline of a shape.
class IOutline {
protected:
IOutline() = default;
IOutline(IOutline&) = default;
IOutline(IOutline&&) = default;
IOutline& operator= (IOutline&&) = default;

public:
virtual ~IOutline() = default;

virtual std::string GetName() const noexcept = 0;
virtual DirectX::XMFLOAT4& GetColor() noexcept = 0;
virtual bool IsVisible() const noexcept = 0;

virtual void SetVisible(bool visible) noexcept = 0;
};

// Outlines a boundind body in **DirectXCollision**.
template<typename Bounds> class BoundingBodyOutline : public IOutline {
private:
static_assert(
std::is_base_of<DirectX::BoundingBox,         Bounds>::value ||
std::is_base_of<DirectX::BoundingFrustum,     Bounds>::value ||
std::is_base_of<DirectX::BoundingOrientedBox, Bounds>::value ||
std::is_base_of<DirectX::BoundingSphere,      Bounds>::value,
"T must either be a DirectX::BoundingBox, DirectX::BoundingFrustum, DirectX::BoundingOrientedBox or DirectX::BoundingSphere"
);

public:
BoundingBodyOutline(
const std::string name,
Bounds& bounds,
DirectX::XMVECTOR color = DirectX::Colors::White,
bool visible = true) noexcept : 
m_name(name),
m_visible(visible),
m_bounds(bounds) 
{
DirectX::XMStoreFloat4(&m_color, color);
}

public:
std::string GetName() const noexcept override { return m_name; }
DirectX::XMFLOAT4& GetColor() noexcept override { return m_color; }
bool IsVisible() const noexcept override { return m_visible; }

Bounds& GetBounds() noexcept { return m_bounds; }

void SetVisible(bool visible) noexcept override { m_visible = visible; }

private:
const std::string m_name;
DirectX::XMFLOAT4 m_color;
bool m_visible;

Bounds& m_bounds;
};

class GridOutline : public IOutline {
public:
GridOutline(
const std::string name,
std::uint16_t xDivsions,
std::uint16_t yDivsions,
DirectX::XMFLOAT3 xAxis, 
DirectX::XMFLOAT3 yAxis, 
DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
DirectX::XMVECTOR color = DirectX::Colors::White,
    bool visible = true
    ) noexcept;
    public:
    std::string GetName() const noexcept override;
    DirectX::XMFLOAT4& GetColor() noexcept override;
    bool IsVisible() const noexcept override;

    std::uint16_t GetXDivsions() const noexcept;
    std::uint16_t GetYDivsions() const noexcept;

    DirectX::XMFLOAT3& GetXAxis() noexcept;
    DirectX::XMFLOAT3& GetYAxis() noexcept;
    DirectX::XMFLOAT3& GetOrigin() noexcept;

    void SetVisible(bool visible) noexcept override;

    void SetXDivisions(std::uint16_t xDivsions) noexcept;
    void SetYDivisions(std::uint16_t yDivsions) noexcept;

    private:
    const std::string m_name;
    DirectX::XMFLOAT4 m_color;
    bool m_visible;

    std::uint16_t m_xDivsions;
    std::uint16_t m_yDivsions;

    DirectX::XMFLOAT3 m_xAxis;
    DirectX::XMFLOAT3 m_yAxis;
    DirectX::XMFLOAT3 m_origin;
    };

class RingOutline : public IOutline {
    public:
        RingOutline(
                const std::string name,
                DirectX::XMFLOAT3 majorAxis,
                DirectX::XMFLOAT3 minorAxis,
                DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMFLOAT4& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMFLOAT3& GetMajorAxis() noexcept;
        DirectX::XMFLOAT3& GetMinorAxis() noexcept;
        DirectX::XMFLOAT3& GetOrigin() noexcept;           

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMFLOAT4 m_color;
        bool m_visible;

        DirectX::XMFLOAT3 m_majorAxis;
        DirectX::XMFLOAT3 m_minorAxis;
        DirectX::XMFLOAT3 m_origin;
};

class RayOutline : public IOutline {
    public:
        RayOutline(
                const std::string name,
                DirectX::XMFLOAT3 direction,
                DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
                bool normalized = false,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMFLOAT4& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMFLOAT3& GetDirection() noexcept;
        DirectX::XMFLOAT3& GetOrigin() noexcept;
        bool IsNormalized() const noexcept;

        void SetVisible(bool visible) noexcept override;
        void SetNormalized(bool normalized) noexcept;

    private:
        const std::string m_name;
        DirectX::XMFLOAT4 m_color;
        bool m_visible;

        DirectX::XMFLOAT3 m_direction;
        DirectX::XMFLOAT3 m_origin;
        bool m_normalized;
};

class TriangleOutline : public IOutline {
    public:
        TriangleOutline(
                const std::string name,
                DirectX::XMFLOAT3 pointA,
                DirectX::XMFLOAT3 pointB,
                DirectX::XMFLOAT3 pointC,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMFLOAT4& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMFLOAT3& GetPointA() noexcept;
        DirectX::XMFLOAT3& GetPointB() noexcept;
        DirectX::XMFLOAT3& GetPointC() noexcept;

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMFLOAT4 m_color;
        bool m_visible;

        DirectX::XMFLOAT3 m_pointA;
        DirectX::XMFLOAT3 m_pointB;
        DirectX::XMFLOAT3 m_pointC;
};

class QuadOutline : public IOutline {
    public:
        QuadOutline(
                const std::string name,
                DirectX::XMFLOAT3 pointA,
                DirectX::XMFLOAT3 pointB,
                DirectX::XMFLOAT3 pointC,
                DirectX::XMFLOAT3 pointD,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMFLOAT4& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMFLOAT3& GetPointA() noexcept;
        DirectX::XMFLOAT3& GetPointB() noexcept;
        DirectX::XMFLOAT3& GetPointC() noexcept;
        DirectX::XMFLOAT3& GetPointD() noexcept;

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMFLOAT4 m_color;
        bool m_visible;

        DirectX::XMFLOAT3 m_pointA;
        DirectX::XMFLOAT3 m_pointB;
        DirectX::XMFLOAT3 m_pointC;
        DirectX::XMFLOAT3 m_pointD;
};

*/
