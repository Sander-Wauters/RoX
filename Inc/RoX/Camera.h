#pragma once

#include "../../Src/Util/pch.h"

class Camera { 
    public:
        Camera();
        ~Camera();

        void TranslateAlongLocalXAxis(float distance); 
        void TranslateAlongLocalYAxis(float distance);
        void TranslateAlongLocalZAxis(float distance);

        void TranslateAlongGlobalXAxis(float distance); 
        void TranslateAlongGlobalYAxis(float distance);
        void TranslateAlongGlobalZAxis(float distance);

        void RotateAroundLocalXAxis(float angle); 
        void RotateAroundLocalYAxis(float angle); 
        void RotateAroundLocalZAxis(float angle); 

        void RotateAroundGlobalXAxis(float angle); 
        void RotateAroundGlobalYAxis(float angle);
        void RotateAroundGlobalZAxis(float angle);

        void PointAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up);
        void PointAt(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

        void SetFrustum(float fovY, float aspect, float nearZ, float farZ);
        void SetPosition(float x, float y, float z);
        void SetPosition(DirectX::XMFLOAT3& position);

        void Update();

    public:
        const DirectX::XMFLOAT3& GetPosition() const;

        float GetNearZ() const;
        float GetFarZ() const;
        float GetAspect() const;
        float GetFovX() const;
        float GetFovY() const;

        float GetNearWindowWidth() const;
        float GetNearWindowHeight() const;
        float GetFarWindowWidth() const;
        float GetFarWindowHeight() const;

        const DirectX::XMFLOAT4X4& GetView() const;
        const DirectX::XMFLOAT4X4& GetProjection() const;

    private:
        float m_nearZ = 0.0f;
        float m_farZ = 0.0f;
        float m_aspect = 0.0f;
        float m_fovY = 0.0f;
        float m_nearWindowHeight = 0.0f;
        float m_farWindowHeight = 0.0f;

        bool m_outdated = false;

        DirectX::XMFLOAT3 m_localXAxis = { 1.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 m_localYAxis = { 0.0f, 1.0f, 0.0f };
        DirectX::XMFLOAT3 m_localZAxis = { 0.0f, 0.0f, 1.0f };
        DirectX::XMFLOAT3 m_position =   { 0.0f, 0.0f, 0.0f };

        DirectX::XMFLOAT4X4 m_view =
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
        DirectX::XMFLOAT4X4 m_projection = 
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
        };
};
