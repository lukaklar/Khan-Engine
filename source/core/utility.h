#pragma once
#include <optional>
#include <type_traits>
#include <utility>

namespace Khan
{
    template<typename T>
    using TOptional = std::optional<T>;

    template<typename T, typename... TArgs>
    [[nodiscard]] constexpr TOptional<T> MakeOptional(TArgs&&... args)
    {
        return std::make_optional<T>(std::forward<TArgs>(args)...);
    }

    template<typename T>
    [[nodiscard]] constexpr TOptional<std::decay_t<T>> MakeOptional(T&& value)
    {
        return std::make_optional(std::forward<T>(value));
    }

    template<typename... Types>
    using Tuple = std::tuple<Types...>;

    template<typename TFirst, typename TSecond>
    using TPair = std::pair<TFirst, TSecond>;

    template<typename TFirst, typename TSecond>
    [[nodiscard]] constexpr auto MakePair(TFirst&& first, TSecond&& second)
    {
        return std::make_pair(std::forward<TFirst>(first), std::forward<TSecond>(second));
    }

    template<typename T>
    [[nodiscard]] constexpr typename std::remove_reference<T>::type&& Move(T&& value) noexcept
    {
        return std::move(value);
    }

    template<typename T>
    [[nodiscard]] constexpr T&& Forward(typename std::remove_reference<T>::type& value) noexcept
    {
        return std::forward<T>(value);
    }

    template<typename T>
    [[nodiscard]] constexpr T&& Forward(typename std::remove_reference<T>::type&& value) noexcept
    {
        return std::forward<T>(value);
    }

    template<typename T>
    constexpr void Swap(T& a, T& b) noexcept(noexcept(std::swap(a, b)))
    {
        std::swap(a, b);
    }

    template<typename TTarget, typename TReplacement = TTarget>
    [[nodiscard]] constexpr TTarget Exchange(TTarget& target, TReplacement&& replacement)
    {
        return std::exchange(target, std::forward<TReplacement>(replacement));
    }
}