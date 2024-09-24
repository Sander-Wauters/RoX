#pragma once

#include <DirectXMath.h>
#include <string>

namespace Util {
    void StoreFloat2(DirectX::XMFLOAT2& in, float* out);
    void StoreFloat3(DirectX::XMFLOAT3& in, float* out);
    void StoreFloat4(DirectX::XMFLOAT4& in, float* out);

    void LoadFloat2(float* in, DirectX::XMFLOAT2& out);
    void LoadFloat3(float* in, DirectX::XMFLOAT3& out);
    void LoadFloat4(float* in, DirectX::XMFLOAT4& out);

    std::string GUIDLabel(std::string label, std::uint64_t GUID);
    std::string GUIDLabel(std::string label, std::string GUID);
}

