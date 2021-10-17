#ifndef UTILITIES_H
#define UTILITIES_H

#include <type_traits>

template <typename ToType, typename FromType>
requires(std::integral<FromType> || std::convertible_to<FromType, ToType>) && std::integral<ToType> constexpr auto narrow_cast(FromType from) noexcept
{
    return static_cast<ToType>(from & static_cast<typename std::make_unsigned<ToType>::type>(-1));
}

template <typename ToType, typename FromType>
requires std::integral<FromType> && std::integral<ToType>
constexpr auto narrow(FromType from)
{
    if (from > std::numeric_limits<ToType>::max() && from < std::numeric_limits<ToType>::min())
    {
        throw std::runtime_error("Narrowing resulted in data loss.");
    }

    return static_cast<ToType>(from & static_cast<typename std::make_unsigned<ToType>::type>(-1));
}

template <typename... Args>
inline void unused(Args &&...) {}

#endif /* UTILITIES_H */
