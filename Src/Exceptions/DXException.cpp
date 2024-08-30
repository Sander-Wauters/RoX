#ifndef UNICODE
#define UNICODE
#endif

#include <comdef.h>

#include "RoX/DXException.h"

DXException::DXException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) {
    m_errorCode = hr;
    m_functionName = functionName;
    m_filename = filename;
    m_lineNumber = lineNumber;
}

std::wstring DXException::ToString() const {
    _com_error err(m_errorCode);
    std::wstring msg = m_functionName + L" failed in " + m_filename + L"; line " + std::to_wstring(m_lineNumber) + L"; error: " + err.ErrorMessage();
    return msg;
}
