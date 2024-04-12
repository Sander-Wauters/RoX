#pragma once

#include "../../Src/Util/pch.h"

class Camera { 
    public:
        Camera(float fovY = DirectX::XM_PIDIV4, float aspect = 1.7f, float nearZ = 1.f, float farZ = 1000.f) noexcept;
        ~Camera() noexcept;

        void TranslateAlongLocalXAxis(float distance) noexcept; 
        void TranslateAlongLocalYAxis(float distance) noexcept;
        void TranslateAlongLocalZAxis(float distance) noexcept;

        void TranslateAlongGlobalXAxis(float distance) noexcept; 
        void TranslateAlongGlobalYAxis(float distance) noexcept;
        void TranslateAlongGlobalZAxis(float distance) noexcept;

        void RotateAroundLocalXAxis(float angle) noexcept; 
        void RotateAroundLocalYAxis(float angle) noexcept; 
        void RotateAroundLocalZAxis(float angle) noexcept; 

        void RotateAroundGlobalXAxis(float angle) noexcept; 
        void RotateAroundGlobalYAxis(float angle) noexcept;
        void RotateAroundGlobalZAxis(float angle) noexcept;

        void PointAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up) noexcept;
        void PointAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up) noexcept;

        void SetPerspectiveView(float fovY, float aspect, float nearZ, float farZ) noexcept;
        void SetOrthographicView(float width, float height, float nearZ, float farZ) noexcept;

        void SetPosition(float x, float y, float z) noexcept;
        void SetPosition(DirectX::XMFLOAT3& position) noexcept;

        void Update() noexcept;

    public:
        bool IsOrthographic() const noexcept;

        const DirectX::XMFLOAT3& GetPosition() const noexcept;

        float GetNearZ() const noexcept;
        float GetFarZ() const noexcept;
        float GetAspect() const noexcept;
        float GetFovX() const noexcept;
        float GetFovY() const noexcept;

        float GetNearWindowWidth() const noexcept;
        float GetNearWindowHeight() const noexcept;
        float GetFarWindowWidth() const noexcept;
        float GetFarWindowHeight() const noexcept;

        const DirectX::XMFLOAT4X4& GetView() const noexcept;
        const DirectX::XMFLOAT4X4& GetProjection() const noexcept;

    private:
        float m_nearZ;
        float m_farZ;
        float m_aspect;
        float m_fovY;
        float m_nearWindowHeight;
        float m_farWindowHeight;

        bool m_orthographic;
        bool m_outdated;

        DirectX::XMFLOAT3 m_localXAxis;
        DirectX::XMFLOAT3 m_localYAxis;
        DirectX::XMFLOAT3 m_localZAxis;
        DirectX::XMFLOAT3 m_position;

        DirectX::XMFLOAT4X4 m_view;
        DirectX::XMFLOAT4X4 m_projection;
};
