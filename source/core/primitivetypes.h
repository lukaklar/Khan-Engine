#pragma once
#include <cstddef>
#include <cstdint>

namespace Khan
{
    // Signed integers
    using int8 = std::int8_t;
    using int16 = std::int16_t;
    using int32 = std::int32_t;
    using int64 = std::int64_t;

    // Unsigned integers
    using uint8 = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;

    // Character types
    using char16 = char16_t;
    using char32 = char32_t;
    using wchar = wchar_t;

    // Memory
    using byte = std::byte;
}