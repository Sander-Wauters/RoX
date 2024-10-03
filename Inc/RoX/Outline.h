#pragma once

#include <string>
#include <type_traits>

#include <DirectXMath.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include "Identifiable.h"


// Collection of structs that hold data to renderer the outline of shape.

// Abstract class that contains properties that every outline has in common.
class Outline : public Identifiable {
    public:
        enum class Type {
            BoundingBox,
            BoundingFrustum,
            BoundingOrientedBox,
            BoundingSphere,
            Grid,
            Ring,
            Ray,
            Triangle,
            Quad
        };

    protected:
        Outline(Type type, std::string name = "", DirectX::XMVECTOR color = DirectX::Colors::White, bool visible = true) noexcept;
        virtual ~Outline() = default;

    public:
        DirectX::XMFLOAT4& GetColor() noexcept;
        bool IsVisible() const noexcept;
        Type GetType() const noexcept;

        void SetVisible(bool visible) noexcept;

    public:
        const Type m_type;
        DirectX::XMFLOAT4 m_color;
        bool m_visible;
};

// Outlines a boundind body in **DirectXCollision**.
template<typename Bounds> class BoundingBodyOutline : public Outline {
    private:
        static_assert(
                std::is_base_of<DirectX::BoundingBox,         Bounds>::value ||
                std::is_base_of<DirectX::BoundingFrustum,     Bounds>::value ||
                std::is_base_of<DirectX::BoundingOrientedBox, Bounds>::value ||
                std::is_base_of<DirectX::BoundingSphere,      Bounds>::value,
                "Bounds must either be a DirectX::BoundingBox, DirectX::BoundingFrustum, DirectX::BoundingOrientedBox or DirectX::BoundingSphere"
                );

        constexpr Type GetType() {
            if (std::is_base_of<DirectX::BoundingBox, Bounds>::value == true)
                return Type::BoundingBox;
            if (std::is_base_of<DirectX::BoundingFrustum, Bounds>::value == true)
                return Type::BoundingFrustum;
            if (std::is_base_of<DirectX::BoundingOrientedBox, Bounds>::value == true)
                return Type::BoundingOrientedBox;
            if (std::is_base_of<DirectX::BoundingSphere, Bounds>:: value == true)
                return Type::BoundingSphere;
        }

    public:
        BoundingBodyOutline(Bounds& boundingBody, std::string name = "", DirectX::XMVECTOR color = DirectX::Colors::White, bool visible = true) noexcept
            : Outline(GetType(), name, color, visible), 
            m_boundingBody(boundingBody)
            {}

    public:
        Bounds& GetBoundingBody() noexcept {
            return m_boundingBody;
        }

    private:
        Bounds& m_boundingBody;
};

class GridOutline : public Outline {
    public:
        GridOutline(std::string name = "",
                std::uint16_t xDivisions = 2,
                std::uint16_t yDivisions = 2,
                DirectX::XMFLOAT3 xAxis = { 1.f, 0.f, 0.f }, 
                DirectX::XMFLOAT3 yAxis = { 0.f, 0.f, 1.f },
                DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true) noexcept;

    public:
        std::uint16_t GetXDivisions() const noexcept;
        std::uint16_t GetYDivisions() const noexcept;

        DirectX::XMFLOAT3& GetXAxis() noexcept;
        DirectX::XMFLOAT3& GetYAxis() noexcept;
        DirectX::XMFLOAT3& GetOrigin() noexcept;

        void SetXDivisions(std::uint16_t xDivsions) noexcept;
        void SetYDivisions(std::uint16_t yDivsions) noexcept;

    private:
        std::uint16_t m_xDivisions;
        std::uint16_t m_yDivisions;

        DirectX::XMFLOAT3 m_xAxis;
        DirectX::XMFLOAT3 m_yAxis;
        DirectX::XMFLOAT3 m_origin;
};

class RingOutline : public Outline {
    public:
        RingOutline(std::string name = "",
                DirectX::XMFLOAT3 majorAxis = { .5f, 0.f, 0.f },
                DirectX::XMFLOAT3 minorAxis = { 0.f, 0.f, .5f },
                DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true) noexcept;

    public:
        DirectX::XMFLOAT3& GetMajorAxis() noexcept;
        DirectX::XMFLOAT3& GetMinorAxis() noexcept;
        DirectX::XMFLOAT3& GetOrigin() noexcept;

    private:
        DirectX::XMFLOAT3 m_majorAxis;
        DirectX::XMFLOAT3 m_minorAxis;
        DirectX::XMFLOAT3 m_origin;
};

class RayOutline : public Outline {
    public:
        RayOutline(std::string name = "",
                DirectX::XMFLOAT3 direction = { 1.f, 0.f, 0.f },
                DirectX::XMFLOAT3 origin = { 0.f, 0.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool normalized = false,
                bool visible = true) noexcept;

    public:
        DirectX::XMFLOAT3& GetDirection() noexcept;
        DirectX::XMFLOAT3& GetOrigin() noexcept;
        bool IsNormalized() const noexcept;

        void SetNormalized(bool normalized) noexcept;

    private:
        DirectX::XMFLOAT3 m_direction;
        DirectX::XMFLOAT3 m_origin;
        bool m_normalized;
};

class TriangleOutline : public Outline {
    public:
        TriangleOutline(std::string name = "",
                DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f },
                DirectX::XMFLOAT3 pointB = { 1.f, 0.f, 0.f },
                DirectX::XMFLOAT3 pointC = { .5f, 1.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true) noexcept;

    public:
        DirectX::XMFLOAT3& GetPointA() noexcept;
        DirectX::XMFLOAT3& GetPointB() noexcept;
        DirectX::XMFLOAT3& GetPointC() noexcept;

    private:
        DirectX::XMFLOAT3 m_pointA;
        DirectX::XMFLOAT3 m_pointB;
        DirectX::XMFLOAT3 m_pointC;
};

class QuadOutline : public Outline {
    public:
        QuadOutline(std::string name = "",
                DirectX::XMFLOAT3 pointA = { 0.f, 0.f, 0.f },
                DirectX::XMFLOAT3 pointB = { 0.f, 1.f, 0.f },
                DirectX::XMFLOAT3 pointC = { 1.f, 0.f, 0.f },
                DirectX::XMFLOAT3 pointD = { 1.f, 1.f, 0.f },
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true) noexcept;

    public:
        DirectX::XMFLOAT3& GetPointA() noexcept;
        DirectX::XMFLOAT3& GetPointB() noexcept;
        DirectX::XMFLOAT3& GetPointC() noexcept;
        DirectX::XMFLOAT3& GetPointD() noexcept;

    private:
        DirectX::XMFLOAT3 m_pointA;
        DirectX::XMFLOAT3 m_pointB;
        DirectX::XMFLOAT3 m_pointC;
        DirectX::XMFLOAT3 m_pointD;
};

