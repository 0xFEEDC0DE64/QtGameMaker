#pragma once

// system includes
#include <cstring>
#include <limits>
#include <type_traits>

// C++20 backports (until espressif finally updates their aged compiler suite)

namespace std {
template <class To, class From>
typename std::enable_if_t<
    sizeof(To) == sizeof(From) && std::is_trivially_copyable_v<From> && std::is_trivially_copyable_v<To>,
    To>
// constexpr support needs compiler magic
bit_cast(const From& src) noexcept
{
    static_assert(std::is_trivially_constructible_v<To>,
                  "This implementation additionally requires destination type to be trivially constructible");

    To dst;
    std::memcpy(&dst, &src, sizeof(To));
    return dst;
}

#if !__cpp_lib_to_underlying
template <typename EnumT, typename = std::enable_if_t<std::is_enum<EnumT>{}>>
constexpr std::underlying_type_t<EnumT> to_underlying(EnumT e) noexcept {
    return static_cast<std::underlying_type_t<EnumT>>(e);
}
#endif
} // namespace std

