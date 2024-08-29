#pragma once
#include "pch.h"

inline void DebugPrintMatrix(const DirectX::XMMATRIX& M) {
    std::string out;
    out += std::to_string(M.r[0].m128_f32[0]) + ", "+ std::to_string(M.r[0].m128_f32[1]) + ", " + std::to_string(M.r[0].m128_f32[2]) + ", " + std::to_string(M.r[0].m128_f32[3]) + "\n";
    out += std::to_string(M.r[1].m128_f32[0]) + ", "+ std::to_string(M.r[1].m128_f32[1]) + ", " + std::to_string(M.r[1].m128_f32[2]) + ", " + std::to_string(M.r[1].m128_f32[3]) + "\n";
    out += std::to_string(M.r[2].m128_f32[0]) + ", "+ std::to_string(M.r[2].m128_f32[1]) + ", " + std::to_string(M.r[2].m128_f32[2]) + ", " + std::to_string(M.r[2].m128_f32[3]) + "\n";
    out += std::to_string(M.r[3].m128_f32[0]) + ", "+ std::to_string(M.r[3].m128_f32[1]) + ", " + std::to_string(M.r[3].m128_f32[2]) + ", " + std::to_string(M.r[3].m128_f32[3]) + "\n";
    OutputDebugStringA(out.c_str());    
}
