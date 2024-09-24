#include "Rox/Camera.h"

#include "../Util/pch.h"

Camera::Camera(float fovY, float aspect, float nearZ, float farZ) 
    noexcept : m_nearZ(nearZ), m_farZ(farZ),
    m_aspect(aspect), m_fovY(fovY),
    m_nearWindowHeight(0.f), m_farWindowHeight(0.f),
    m_orthographic(false), m_outdated(false),
    m_localXAxis({ 1.f, 0.f, 0.f }),
    m_localYAxis({ 0.f, 1.f, 0.f }),
    m_localZAxis({ 0.f, 0.f, 1.f }),
    m_position({ 0.f, 0.f, 0.f }),
    m_view({1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f}),
    m_projection({
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f})
{
    SetPerspectiveView(fovY, aspect, nearZ, farZ);
}

Camera::~Camera() noexcept {
}

void Camera::TranslateAlongLocalXAxis(float distance) noexcept {
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
    DirectX::XMVECTOR x = DirectX::XMLoadFloat3(&m_localXAxis);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&m_position);
    DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(s, x, p));

    m_outdated = true;
}
 
void Camera::TranslateAlongLocalYAxis(float distance) noexcept {
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
    DirectX::XMVECTOR y = DirectX::XMLoadFloat3(&m_localYAxis);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&m_position);
    DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(s, y, p));

    m_outdated = true;
}

void Camera::TranslateAlongLocalZAxis(float distance) noexcept {
    DirectX::XMVECTOR s = DirectX::XMVectorReplicate(distance);
    DirectX::XMVECTOR z = DirectX::XMLoadFloat3(&m_localZAxis);
    DirectX::XMVECTOR p = DirectX::XMLoadFloat3(&m_position);
    DirectX::XMStoreFloat3(&m_position, DirectX::XMVectorMultiplyAdd(s, z, p));

    m_outdated = true;
}

void Camera::TranslateAlongGlobalXAxis(float distance) noexcept {
    m_position.x += distance;

    m_outdated = true;
}
 
void Camera::TranslateAlongGlobalYAxis(float distance) noexcept {
    m_position.y += distance;

    m_outdated = true;
}

void Camera::TranslateAlongGlobalZAxis(float distance) noexcept {
    m_position.z += distance;

    m_outdated = true;
}

void Camera::RotateAroundLocalXAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_localXAxis), angle);
    DirectX::XMStoreFloat3(&m_localYAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localYAxis), R));
    DirectX::XMStoreFloat3(&m_localZAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localZAxis), R));

    m_outdated = true;
}
 
void Camera::RotateAroundLocalYAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_localYAxis), angle);
    DirectX::XMStoreFloat3(&m_localXAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localXAxis), R));
    DirectX::XMStoreFloat3(&m_localZAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localZAxis), R));

    m_outdated = true;
}
 
void Camera::RotateAroundLocalZAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_localZAxis), angle);
    DirectX::XMStoreFloat3(&m_localYAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localYAxis), R));
    DirectX::XMStoreFloat3(&m_localXAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localXAxis), R));

    m_outdated = true;
}
 
void Camera::RotateAroundGlobalXAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationX(angle);
    DirectX::XMStoreFloat3(&m_localYAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localYAxis), R));
    DirectX::XMStoreFloat3(&m_localZAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localZAxis), R));

    m_outdated = true;
}
 
void Camera::RotateAroundGlobalYAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationY(angle);
    DirectX::XMStoreFloat3(&m_localXAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localXAxis), R));
    DirectX::XMStoreFloat3(&m_localZAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localZAxis), R));

    m_outdated = true;
}

void Camera::RotateAroundGlobalZAxis(float angle) noexcept {
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationZ(angle);
    DirectX::XMStoreFloat3(&m_localYAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localYAxis), R));
    DirectX::XMStoreFloat3(&m_localXAxis, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&m_localXAxis), R));

    m_outdated = true;
}

void Camera::PointAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up) noexcept {
    DirectX::XMVECTOR Z = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(target, position));
    DirectX::XMVECTOR X = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, Z));
    DirectX::XMVECTOR Y = DirectX::XMVector3Cross(Z, X);
    
    DirectX::XMStoreFloat3(&m_position, position);
    DirectX::XMStoreFloat3(&m_localXAxis, X);
    DirectX::XMStoreFloat3(&m_localYAxis, Y);
    DirectX::XMStoreFloat3(&m_localZAxis, Z);

    m_outdated = true;
}

void Camera::PointAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up) noexcept {
    DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&target);
    DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR U = DirectX::XMLoadFloat3(&up);

    PointAt(P, T, U);

    m_outdated = true;
}

void Camera::SetPerspectiveView(float fovY, float aspect, float nearZ, float farZ) noexcept {
    m_fovY = std::max(fovY, .1f);
    m_aspect = std::max(aspect, .1f);
    m_nearZ = std::max(m_nearZ, .1f);
    m_farZ = std::max(farZ, .1f);
    m_orthographic = false;

    m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
    m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
    DirectX::XMStoreFloat4x4(&m_projection, P);
}

void Camera::SetOrthographicView(float width, float height, float nearZ, float farZ) noexcept {
    m_aspect = width/std::max(height, .1f); 
    m_nearZ = std::max(nearZ, .1f);
    m_farZ = std::max(farZ, .1f);
    m_orthographic = true;

    m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * m_fovY);
    m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * m_fovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(GetNearWindowWidth(), GetNearWindowHeight(), nearZ, farZ);
    DirectX::XMStoreFloat4x4(&m_projection, P);
}

void Camera::SetPosition(float x, float y, float z) noexcept {
    m_position = {x, y, z};
    m_outdated = true;
}

void Camera::SetPosition(DirectX::XMFLOAT3& position) noexcept {
    m_position = position;
    m_outdated = true;
}

void Camera::Update() noexcept {
    if (!m_outdated)
        return;
 
    DirectX::XMVECTOR X = DirectX::XMLoadFloat3(&m_localXAxis);
    DirectX::XMVECTOR Y = DirectX::XMLoadFloat3(&m_localYAxis);
    DirectX::XMVECTOR Z = DirectX::XMLoadFloat3(&m_localZAxis);
    DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&m_position);

    // Keep the cameras axes orthogonal to each other and of unit length.
    Z = DirectX::XMVector3Normalize(Z);
    Y = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(Z, X));

    // Y, Z already ortho-normal, so no need to normalize cross product.
    X = DirectX::XMVector3Cross(Y, Z);

    // Fill in the view matrix entries.
    float x = DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, X));
    float y = DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, Y));
    float z = DirectX::XMVectorGetX(DirectX::XMVector3Dot(P, Z));

    DirectX::XMStoreFloat3(&m_localXAxis, X);
    DirectX::XMStoreFloat3(&m_localYAxis, Y);
    DirectX::XMStoreFloat3(&m_localZAxis, Z);

    m_view(0, 0) = m_localXAxis.x;
    m_view(1, 0) = m_localXAxis.y;
    m_view(2, 0) = m_localXAxis.z;
    m_view(3, 0) = x;
    m_view(0, 1) = m_localYAxis.x;
    m_view(1, 1) = m_localYAxis.y;
    m_view(2, 1) = m_localYAxis.z;
    m_view(3, 1) = y;
    m_view(0, 2) = m_localZAxis.x;
    m_view(1, 2) = m_localZAxis.y;
    m_view(2, 2) = m_localZAxis.z;
    m_view(3, 2) = z;
    m_view(0, 3) = 0.0f;
    m_view(1, 3) = 0.0f;
    m_view(2, 3) = 0.0f;
    m_view(3, 3) = 1.0f;

    m_outdated = false;
}

bool Camera::IsOrthographic() const noexcept {
    return m_orthographic;
}

const DirectX::XMFLOAT3& Camera::GetPosition() const noexcept {
    return m_position;
}

float Camera::GetNearZ() const noexcept {
    return m_nearZ;
}

float Camera::GetFarZ() const noexcept {
    return m_farZ;
}

float Camera::GetAspect() const noexcept {
    return m_aspect;
}

float Camera::GetFovX() const noexcept {
    float halfWidth = 0.5f * GetNearWindowWidth();
    return (float)(2.0f * atan(halfWidth / m_nearZ));
}

float Camera::GetFovY() const noexcept { 
    return m_fovY;
}

float Camera::GetNearWindowWidth() const noexcept {
    return m_aspect * m_nearWindowHeight;
}

float Camera::GetNearWindowHeight() const noexcept {
    return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth() const noexcept {
    return m_aspect * m_farWindowHeight;
}

float Camera::GetFarWindowHeight() const noexcept {
    return m_farWindowHeight;
}

const DirectX::XMFLOAT4X4& Camera::GetView() const noexcept {
    return m_view;
}

const DirectX::XMFLOAT4X4& Camera::GetProjection() const noexcept {
    return m_projection;
}

