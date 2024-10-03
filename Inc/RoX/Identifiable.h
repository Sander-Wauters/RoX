#pragma once

#include <string>

// Abstract class that contains properties that helps the engine identify different objects.
class Identifiable {
    public:
        static constexpr std::uint64_t INVALID_GUID = std::uint64_t(-1);

    protected:
        Identifiable(std::string defaultName, std::string name = "") noexcept;
        virtual ~Identifiable() = default;

        Identifiable(const Identifiable& other) noexcept;

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
