#pragma once

#include <string>
#include <type_traits>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

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
        virtual DirectX::XMVECTOR& GetColor() noexcept = 0;
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
            m_color(color),
            m_visible(visible),
            m_bounds(bounds) {}

    public:
        std::string GetName() const noexcept override { return m_name; }
        DirectX::XMVECTOR& GetColor() noexcept override { return m_color; }
        bool IsVisible() const noexcept override { return m_visible; }

        Bounds& GetBounds() noexcept { return m_bounds; }

        void SetVisible(bool visible) noexcept override { m_visible = visible; }

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        Bounds& m_bounds;
};

class GridOutline : public IOutline {
    public:
        GridOutline(
                const std::string name,
                std::uint16_t xDivsions,
                std::uint16_t yDivsions,
                DirectX::XMVECTOR xAxis, 
                DirectX::XMVECTOR yAxis, 
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;
    public:
        std::string GetName() const noexcept override;
        DirectX::XMVECTOR& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        std::uint16_t GetXDivsions() const noexcept;
        std::uint16_t GetYDivsions() const noexcept;

        DirectX::XMVECTOR& GetXAxis() noexcept;
        DirectX::XMVECTOR& GetYAxis() noexcept;
        DirectX::XMVECTOR& GetOrigin() noexcept;

        void SetVisible(bool visible) noexcept override;

        void SetXDivisions(std::uint16_t xDivsions) noexcept;
        void SetYDivisions(std::uint16_t yDivsions) noexcept;

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        std::uint16_t m_xDivsions;
        std::uint16_t m_yDivsions;

        DirectX::XMVECTOR m_xAxis;
        DirectX::XMVECTOR m_yAxis;
        DirectX::XMVECTOR m_origin;
};

class RingOutline : public IOutline {
    public:
        RingOutline(
                const std::string name,
                DirectX::XMVECTOR majorAxis,
                DirectX::XMVECTOR minorAxis,
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMVECTOR& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMVECTOR& GetMajorAxis() noexcept;
        DirectX::XMVECTOR& GetMinorAxis() noexcept;
        DirectX::XMVECTOR& GetOrigin() noexcept;           

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        DirectX::XMVECTOR m_majorAxis;
        DirectX::XMVECTOR m_minorAxis;
        DirectX::XMVECTOR m_origin;
};

class RayOutline : public IOutline {
    public:
        RayOutline(
                const std::string name,
                DirectX::XMVECTOR direction,
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                bool normalized = false,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMVECTOR& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMVECTOR& GetDirection() noexcept;
        DirectX::XMVECTOR& GetOrigin() noexcept;
        bool IsNormalized() const noexcept;

        void SetVisible(bool visible) noexcept override;
        void SetNormalized(bool normalized) noexcept;

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        DirectX::XMVECTOR m_direction;
        DirectX::XMVECTOR m_origin;
        bool m_normalized;
};

class TriangleOutline : public IOutline {
    public:
        TriangleOutline(
                const std::string name,
                DirectX::XMVECTOR pointA,
                DirectX::XMVECTOR pointB,
                DirectX::XMVECTOR pointC,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMVECTOR& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMVECTOR& GetPointA() noexcept;
        DirectX::XMVECTOR& GetPointB() noexcept;
        DirectX::XMVECTOR& GetPointC() noexcept;

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        DirectX::XMVECTOR m_pointA;
        DirectX::XMVECTOR m_pointB;
        DirectX::XMVECTOR m_pointC;
};

class QuadOutline : public IOutline {
    public:
        QuadOutline(
                const std::string name,
                DirectX::XMVECTOR pointA,
                DirectX::XMVECTOR pointB,
                DirectX::XMVECTOR pointC,
                DirectX::XMVECTOR pointD,
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true
                ) noexcept;

    public:
        std::string GetName() const noexcept override;
        DirectX::XMVECTOR& GetColor() noexcept override;
        bool IsVisible() const noexcept override;

        DirectX::XMVECTOR& GetPointA() noexcept;
        DirectX::XMVECTOR& GetPointB() noexcept;
        DirectX::XMVECTOR& GetPointC() noexcept;
        DirectX::XMVECTOR& GetPointD() noexcept;

        void SetVisible(bool visible) noexcept override;

    private:
        const std::string m_name;
        DirectX::XMVECTOR m_color;
        bool m_visible;

        DirectX::XMVECTOR m_pointA;
        DirectX::XMVECTOR m_pointB;
        DirectX::XMVECTOR m_pointC;
        DirectX::XMVECTOR m_pointD;
};

