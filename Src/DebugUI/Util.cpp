#include "DebugUI/Util.h"

void Util::StoreFloat2(DirectX::XMFLOAT2& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
}

void Util::StoreFloat3(DirectX::XMFLOAT3& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
}

void Util::StoreFloat4(DirectX::XMFLOAT4& in, float* out) {
    out[0] = in.x;
    out[1] = in.y;
    out[2] = in.z;
    out[3] = in.w;
}

void Util::LoadFloat2(float* in, DirectX::XMFLOAT2& out) {
    out.x = in[0];
    out.y = in[1];
}

void Util::LoadFloat3(float* in, DirectX::XMFLOAT3& out) {
    out.x = in[0];
    out.y = in[1];
    out.z = in[2];
}

void Util::LoadFloat4(float* in, DirectX::XMFLOAT4& out) {
    out.x = in[0];
    out.y = in[1];
    out.z = in[2];
    out.w = in[3];
}

std::string Util::GUIDLabel(std::string label, std::uint64_t GUID) {
    return label + "##" + std::to_string(GUID);
}

std::string Util::GUIDLabel(std::string label, std::string GUID) {
    return label + "##" + GUID;
}

