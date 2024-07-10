#pragma once

#include <type_traits>

#include "../../Src/Util/pch.h"

namespace Outline {

    class Base {
        protected: 
            Base(
                    const std::string name,
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                ) noexcept :
                m_name(name),
                m_color(color), 
                m_visible(visible)
                {}

        public:
            virtual ~Base() noexcept {}

            std::string GetName() const noexcept { return m_name; }

            const DirectX::XMVECTOR& GetColor() const noexcept { return m_color; }

            bool IsVisible() const noexcept { return m_visible; }

            void SetColor(DirectX::XMVECTOR& color) noexcept { m_color = color; }
            void SetVisible(bool visible) noexcept { m_visible = visible; }

        private:
            const std::string m_name;

            DirectX::XMVECTOR m_color;

            bool m_visible;
    };

    template<typename Bounds> class BoundingBody : public Base {
        private:
            static_assert(
                    std::is_base_of<DirectX::BoundingBox,         Bounds>::value ||
                    std::is_base_of<DirectX::BoundingFrustum,     Bounds>::value ||
                    std::is_base_of<DirectX::BoundingOrientedBox, Bounds>::value ||
                    std::is_base_of<DirectX::BoundingSphere,      Bounds>::value,
                    "T must either be a DirectX::BoundingBox, DirectX::BoundingFrustum, DirectX::BoundingOrientedBox or DirectX::BoundingSphere"
                    );

        public:
            BoundingBody(
                    const std::string name,
                    Bounds& bounds,
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                    ) noexcept :
                Base(name, color, visible),
                m_bounds(bounds)
                {}

        public:
            Bounds& GetBounds() noexcept { return m_bounds; }

        private:
            Bounds& m_bounds;
    };

    class Grid : public Base {
        public:
            Grid(
                    const std::string name,
                    std::uint16_t xDivsions,
                    std::uint16_t yDivsions,
                    DirectX::XMVECTOR xAxis, 
                    DirectX::XMVECTOR yAxis, 
                    DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                ) noexcept :
                Base(name, color, visible),
                m_xDivsions(xDivsions), m_yDivsions(yDivsions),
                m_xAxis(xAxis), m_yAxis(yAxis), m_origin(origin)
                {}

        public:
            std::uint16_t GetXDivsions() const noexcept { return m_xDivsions; };
            std::uint16_t GetYDivsions() const noexcept { return m_yDivsions; };

            const DirectX::XMVECTOR& GetXAxis() const noexcept { return m_xAxis; };
            const DirectX::XMVECTOR& GetYAxis() const noexcept { return m_yAxis; };
            const DirectX::XMVECTOR& GetOrigin() const noexcept { return m_origin; };

            void SetXDivisions(std::uint16_t xDivsions) noexcept { m_xDivsions = xDivsions; }
            void SetYDivisions(std::uint16_t yDivsions) noexcept { m_yDivsions = yDivsions; }

            void SetXAxis(DirectX::XMVECTOR& xAxis) noexcept { m_xAxis = xAxis; }
            void SetYAxis(DirectX::XMVECTOR& yAxis) noexcept { m_yAxis = yAxis; }
            void SetOrigin(DirectX::XMVECTOR& origin) noexcept { m_origin = origin; }

        private:
            std::uint16_t m_xDivsions;
            std::uint16_t m_yDivsions;

            DirectX::XMVECTOR m_xAxis;
            DirectX::XMVECTOR m_yAxis;
            DirectX::XMVECTOR m_origin;
    };

    class Ring : public Base {
        public:
            Ring(
                    const std::string name,
                    DirectX::XMVECTOR majorAxis,
                    DirectX::XMVECTOR minorAxis,
                    DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                ) noexcept :
                Base(name, color, visible),
                m_majorAxis(majorAxis), m_minorAxis(minorAxis),
                m_origin(origin)
                {}

        public:
            const DirectX::XMVECTOR& GetMajorAxis() const noexcept { return m_majorAxis; }
            const DirectX::XMVECTOR& GetMinorAxis() const noexcept { return m_minorAxis; }
            const DirectX::XMVECTOR& GetOrigin() const noexcept { return m_origin; }            

            void setMajorAxis(DirectX::XMVECTOR& majorAxis) noexcept { m_majorAxis = majorAxis; }
            void setMinorAxis(DirectX::XMVECTOR& minorAxis) noexcept { m_majorAxis = minorAxis; }
            void SetOrigin(DirectX::XMVECTOR& origin) noexcept { m_origin = origin; }

        private:
            DirectX::XMVECTOR m_majorAxis;
            DirectX::XMVECTOR m_minorAxis;
            DirectX::XMVECTOR m_origin;
    };

    class Ray : public Base {
        public:
            Ray(
                    const std::string name,
                    DirectX::XMVECTOR direction,
                    DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                    bool normalized = false,
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
               ) noexcept :
                Base(name, color, visible),
                m_direction(direction), m_origin(origin),
                m_normalized(normalized)
                {}
        public:
            const DirectX::XMVECTOR& GetDirection() const noexcept { return m_direction; }
            const DirectX::XMVECTOR& GetOrigin() const noexcept { return m_origin; }
            bool IsNormalized() const noexcept { return m_normalized; }

            void SetDirection(DirectX::XMVECTOR& direction) noexcept { m_direction = direction; }
            void SetOrigin(DirectX::XMVECTOR& origin) noexcept { m_origin = origin; }

            void SetNormalized(bool normalized) noexcept { m_normalized = normalized; }

        private:
            DirectX::XMVECTOR m_direction;
            DirectX::XMVECTOR m_origin;
            bool m_normalized;
    };

    class Triangle : public Base {
        public:
            Triangle(
                    const std::string name,
                    DirectX::XMVECTOR pointA,
                    DirectX::XMVECTOR pointB,
                    DirectX::XMVECTOR pointC,
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                    ) noexcept :
                Base(name, color, visible),
                m_pointA(pointA), m_pointB(pointB), m_pointC(pointC)
                {}

        public:
            const DirectX::XMVECTOR& GetPointA() const noexcept { return m_pointA; }
            const DirectX::XMVECTOR& GetPointB() const noexcept { return m_pointB; }
            const DirectX::XMVECTOR& GetPointC() const noexcept { return m_pointC; }

            void SetPointA(DirectX::XMVECTOR& pointA) noexcept { m_pointA = pointA; }
            void SetPointB(DirectX::XMVECTOR& pointB) noexcept { m_pointB = pointB; }
            void SetPointC(DirectX::XMVECTOR& pointC) noexcept { m_pointC = pointC; }

        private:
            DirectX::XMVECTOR m_pointA;
            DirectX::XMVECTOR m_pointB;
            DirectX::XMVECTOR m_pointC;
    };

    class Quad : public Base {
        public:
            Quad(
                    const std::string name,
                    DirectX::XMVECTOR pointA,
                    DirectX::XMVECTOR pointB,
                    DirectX::XMVECTOR pointC,
                    DirectX::XMVECTOR pointD,
                    DirectX::XMVECTOR color = DirectX::Colors::White,
                    bool visible = true
                ) noexcept :
                Base(name, color, visible),
                m_pointA(pointA), m_pointB(pointB), m_pointC(pointC), m_pointD(pointD)
                {}

        public:
            const DirectX::XMVECTOR& GetPointA() const noexcept { return m_pointA; }
            const DirectX::XMVECTOR& GetPointB() const noexcept { return m_pointB; }
            const DirectX::XMVECTOR& GetPointC() const noexcept { return m_pointC; }
            const DirectX::XMVECTOR& GetPointD() const noexcept { return m_pointD; }

            void SetPointA(DirectX::XMVECTOR& pointA) noexcept { m_pointA = pointA; }
            void SetPointB(DirectX::XMVECTOR& pointB) noexcept { m_pointB = pointB; }
            void SetPointC(DirectX::XMVECTOR& pointC) noexcept { m_pointC = pointC; }
            void SetPointD(DirectX::XMVECTOR& pointD) noexcept { m_pointD = pointD; }

        private:
            DirectX::XMVECTOR m_pointA;
            DirectX::XMVECTOR m_pointB;
            DirectX::XMVECTOR m_pointC;
            DirectX::XMVECTOR m_pointD;
    };
};

