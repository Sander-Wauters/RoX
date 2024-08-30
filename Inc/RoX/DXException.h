#pragma once

#include <Windows.h>
#include <string>

// Used as a fast fail. 
// If this exception is thrown the application cannot resume and needs to be shut down.
class DXException {
    public:
        DXException() = default;
        DXException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

        std::wstring ToString() const;

    public:
        HRESULT m_errorCode = S_OK;
        std::wstring m_functionName = L"";
        std::wstring m_filename = L"";
        int m_lineNumber = -1;
};

