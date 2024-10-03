#include "RoX/Identifiable.h"

std::uint64_t Identifiable::NEXT_GUID = 0;

Identifiable::Identifiable(std::string defaultName, std::string name)
    noexcept : m_defaultName(defaultName),
    m_GUID(NEXT_GUID++)
{
    SetName(name);
}

Identifiable::Identifiable(const Identifiable& other)
    noexcept : m_defaultName(other.m_defaultName),
    m_name(other.m_name),
    m_GUID(other.m_GUID)
{}

std::string Identifiable::GetName() const noexcept {
    return m_name;    
}

std::uint64_t Identifiable::GetGUID() const noexcept {
    return m_GUID;
}

void Identifiable::SetName(std::string name) noexcept {
    if (name.empty())
        m_name = m_defaultName + "_" + std::to_string(m_GUID);
    else 
        m_name = name;
}

