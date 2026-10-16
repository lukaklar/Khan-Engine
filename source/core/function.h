#pragma once
#include <functional>
#include <utility>

namespace Khan
{
	// Callable wrappers
	template<typename TSignature>
	using TFunction = std::function<TSignature>;

	template<typename T>
	using TReferenceWrapper = std::reference_wrapper<T>;

	template<typename T = void>
	using THash = std::hash<T>;

	// Reference helpers
	template<typename T>
	[[nodiscard]] constexpr TReferenceWrapper<T> Ref(T& value) noexcept
	{
		return std::ref(value);
	}

	template<typename T>
	[[nodiscard]] constexpr TReferenceWrapper<const T> CRef(const T& value) noexcept
	{
		return std::cref(value);
	}

	// Invocation
	template<typename TCallable, typename... TArgs>
	constexpr decltype(auto) Invoke(TCallable&& callable, TArgs&&... args) noexcept(noexcept(std::invoke(std::forward<TCallable>(callable), std::forward<TArgs>(args)...)))
	{
		return std::invoke(std::forward<TCallable>(callable), std::forward<TArgs>(args)...);
	}

	// Binding
	template<typename TCallable, typename... TArgs>
	[[nodiscard]] constexpr auto Bind(TCallable&& callable, TArgs&&... args)
	{
		return std::bind(std::forward<TCallable>(callable), std::forward<TArgs>(args)...);
	}

	inline constexpr auto TPlaceholder1 = std::placeholders::_1;
	inline constexpr auto TPlaceholder2 = std::placeholders::_2;
	inline constexpr auto TPlaceholder3 = std::placeholders::_3;

	// Comparison function objects
	template<typename T = void> using TEqualTo = std::equal_to<T>;
	template<typename T = void> using TNotEqualTo = std::not_equal_to<T>;
	template<typename T = void> using TLess = std::less<T>;
	template<typename T = void> using TGreater = std::greater<T>;
	template<typename T = void> using TLessEqual = std::less_equal<T>;
	template<typename T = void> using TGreaterEqual = std::greater_equal<T>;

	// Arithmetic function objects
	template<typename T = void> using TPlus = std::plus<T>;
	template<typename T = void> using TMinus = std::minus<T>;
	template<typename T = void> using TMultiplies = std::multiplies<T>;
	template<typename T = void> using TDivides = std::divides<T>;
	template<typename T = void> using TModulus = std::modulus<T>;
	template<typename T = void> using TNegate = std::negate<T>;

	// Logical function objects
	template<typename T = void> using TLogicalAnd = std::logical_and<T>;
	template<typename T = void> using TLogicalOr = std::logical_or<T>;
	template<typename T = void> using TLogicalNot = std::logical_not<T>;

	// Bitwise function objects
	template<typename T = void> using TBitAnd = std::bit_and<T>;
	template<typename T = void> using TBitOr = std::bit_or<T>;
	template<typename T = void> using TBitXor = std::bit_xor<T>;
	template<typename T = void> using TBitNot = std::bit_not<T>;
}
