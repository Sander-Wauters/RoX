#pragma once

#include <exception>

#include "RoX/DXException.h"

inline std::wstring AnsiToWString(const std::string& str) {
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x) \
{ \
    HRESULT hr__ = (x); \
    std::wstring wfn = AnsiToWString(__FILE__); \
    if (FAILED(hr__)) { throw DXException(hr__, L#x, wfn, __LINE__); } \
}
#endif
