#pragma once

#include <string>

// Abstract class that contains properties that every asset has in common.
// Used internally, the client should NEVER be required to inherit this class.
class Asset {
    protected:
        Asset(std::string defaultName, std::string name = "") noexcept;
        virtual ~Asset() = default;

        Asset(const Asset& other) noexcept;

    public:
        std::string GetName() const noexcept;
        std::uint64_t GetGUID() const noexcept;

        void SetName(std::string name) noexcept;

    private:
        static std::uint64_t NEXT_GUID;
        const std::string m_defaultName;

        std::string m_name;

        std::uint64_t m_GUID;
};
