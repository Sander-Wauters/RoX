#pragma once

#include <vector>
#include <string>

#include <RoX/VertexTypes.h>

namespace MathUI {
    void Vertex(VertexPositionNormalTexture& vertex);
    void Vertex(VertexPositionNormalTextureSkinning& vertex);

    void Vertices(std::vector<VertexPositionNormalTexture>& vertices);
    void Vertices(std::vector<VertexPositionNormalTextureSkinning>& vertices);

    void Indices(std::vector<std::uint16_t>& indices, std::uint16_t numVertices);
    void Matrix(DirectX::XMMATRIX& matrix);
    void Matrix(DirectX::XMFLOAT4X4& matrix);
    void Matrix(DirectX::XMFLOAT3X4& matrix);

    bool AffineTransformation(DirectX::XMMATRIX& matrix);
    bool AffineTransformation(DirectX::XMFLOAT4X4& matrix);
    bool AffineTransformation(DirectX::XMFLOAT3X4& matrix);

    void Vector(std::string label, DirectX::XMFLOAT2& vector, float dragSpeed = 0.05f);
    void Vector(std::string label, DirectX::XMFLOAT3& vector, float dragSpeed = 0.05f);
    void Vector(std::string label, DirectX::XMFLOAT4& vector, float dragSpeed = 0.05f);

    void ColorVector(std::string label, DirectX::XMFLOAT3& colorVector, float dragSpeed = 0.05f);
    void ColorVector(std::string label, DirectX::XMFLOAT4& colorVector, float dragSpeed = 0.05f);

    void QuaternionWithEulerControlls(std::string label, DirectX::XMFLOAT4& quaternion, float dragSpeed = 0.05f);
}

