#include "RoX/Asset.h"

std::uint64_t Asset::NEXT_GUID = 0;

Asset::Asset(std::string defaultName, std::string name)
    noexcept : m_defaultName(defaultName),
    m_GUID(NEXT_GUID++)
{
    SetName(name);
}

Asset::Asset(const Asset& other)
    noexcept : m_defaultName(other.m_defaultName),
    m_name(other.m_name),
    m_GUID(other.m_GUID)
{}

std::string Asset::GetName() const noexcept {
    return m_name;    
}

std::uint64_t Asset::GetGUID() const noexcept {
    return m_GUID;
}

void Asset::SetName(std::string name) noexcept {
    if (name.empty())
        m_name = m_defaultName + "_" + std::to_string(m_GUID);
    else 
        m_name = name;
}

