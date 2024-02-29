#pragma once

#include <DirectXCollision.h>
#include <DirectXColors.h>

namespace Primitive {
    struct Base {
        Base(
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White,
                bool visible = true) noexcept :
            Visible(visible), World(world), Color(color)
            {}
        virtual ~Base() {}

        bool Visible = true;

        DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
        DirectX::XMVECTOR Color = DirectX::Colors::White;
    };

    struct BoundingSphere : public Base {
        BoundingSphere(
                DirectX::BoundingSphere* pBounds = nullptr, 
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            pBounds(pBounds) 
            {
                BoundingSphere::Base(world, color);
            }

        const DirectX::BoundingSphere* pBounds;
    };

    struct BoundingBox : public Base {
        BoundingBox(
                DirectX::BoundingBox* pBounds = nullptr,
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            pBounds(pBounds) 
            {
                BoundingBox::Base(world, color);
            }

        const DirectX::BoundingBox* pBounds;
    };

    struct BoundingOrientedBox : public Base {
        BoundingOrientedBox(
                DirectX::BoundingOrientedBox* pBounds = nullptr,
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            pBounds(pBounds) 
            {
                BoundingOrientedBox::Base(world, color);
            }

        const DirectX::BoundingOrientedBox* pBounds;
    };

    struct BoundingFrustum : public Base {
        BoundingFrustum(
                DirectX::BoundingFrustum* pBounds = nullptr,
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            pBounds(pBounds) 
            {
                BoundingFrustum::Base(world, color);
            }

        const DirectX::BoundingFrustum* pBounds;
    };

    struct Grid : public Base {
        Grid(
                DirectX::XMVECTOR xAxis = {{ 10.f, 0.f, 0.f }}, 
                DirectX::XMVECTOR yAxis = {{ 0.f, 0.f, 10.f }}, 
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                size_t xDivsions = 20,
                size_t yDivsions = 20,
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            XAxis(xAxis), YAxis(yAxis), Origin(origin), 
            XDivsions(xDivsions), YDivsions(yDivsions)
            {
                Grid::Base(world, color);
            }

        DirectX::XMVECTOR XAxis;
        DirectX::XMVECTOR YAxis;
        DirectX::XMVECTOR Origin;
        size_t XDivsions;
        size_t YDivsions;
    };

    struct Ring : public Base {
        Ring(
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                DirectX::XMVECTOR majorAxis = {{ 1.f, 0.f, 0.f }},
                DirectX::XMVECTOR minorAxis = {{ 0.f, 0.f, 1.f }},
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            Origin(origin), MajorAxis(majorAxis), MinorAxis(minorAxis)
            {
                Ring::Base(world, color);
            }

        DirectX::XMVECTOR Origin;
        DirectX::XMVECTOR MajorAxis;
        DirectX::XMVECTOR MinorAxis;
    };

    struct Ray : public Base {
        Ray( 
                DirectX::XMVECTOR origin = {{ 0.f, 0.f, 0.f }},
                DirectX::XMVECTOR direction = {{ 1.f, 0.f, 0.f }},
                bool normalize = false,
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            Origin(origin), Direction(direction), Normalize(normalize)
            {
                Ray::Base(world, color);
            }

        DirectX::XMVECTOR Origin;
        DirectX::XMVECTOR Direction;
        bool Normalize;
    };

    struct Triangle : public Base {
        Triangle(
                DirectX::XMVECTOR pointA = {{ 1.f, 0.f, 0.f }},
                DirectX::XMVECTOR pointB = {{ 0.f, 1.f, 0.f }},
                DirectX::XMVECTOR pointC = {{ 0.f, 0.f, 1.f }},
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            PointA(pointA), PointB(pointB), PointC(pointC)
            {
                Triangle::Base(world, color);
            }

        DirectX::XMVECTOR PointA;
        DirectX::XMVECTOR PointB;
        DirectX::XMVECTOR PointC;
    };

    struct Quad : public Base {
        Quad(
                DirectX::XMVECTOR pointA = {{ 1.f, 1.f, 0.f }},
                DirectX::XMVECTOR pointB = {{ 0.f, 1.f, 1.f }},
                DirectX::XMVECTOR pointC = {{ 1.f, 0.f, 1.f }},
                DirectX::XMVECTOR pointD = {{ 0.f, 0.f, 0.f }},
                DirectX::XMMATRIX world = DirectX::XMMatrixIdentity(), 
                DirectX::XMVECTOR color = DirectX::Colors::White) noexcept :
            PointA(pointA), PointB(pointB), PointC(pointC), PointD(pointD)
            {
                Quad::Base(world, color);
            }

        DirectX::XMVECTOR PointA;
        DirectX::XMVECTOR PointB;
        DirectX::XMVECTOR PointC;
        DirectX::XMVECTOR PointD;
    };

}
