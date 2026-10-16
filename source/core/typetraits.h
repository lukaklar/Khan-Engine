#pragma once
#include <type_traits>

namespace Khan
{
    // Type Transformations
    template<typename T>
    using remove_const = std::remove_const_t<T>;

    template<typename T>
    using remove_volatile = std::remove_volatile_t<T>;

    template<typename T>
    using remove_cv = std::remove_cv_t<T>;

    template<typename T>
    using remove_reference = std::remove_reference_t<T>;

    template<typename T>
    using remove_cv_reference = std::remove_cv_t<std::remove_reference_t<T>>;

    template<typename T>
    using remove_pointer = std::remove_pointer_t<T>;

    template<typename T>
    using remove_extent = std::remove_extent_t<T>;

    template<typename T>
    using remove_all_extents = std::remove_all_extents_t<T>;

    template<typename T>
    using decay = std::decay_t<T>;

    template<typename T>
    using add_const = std::add_const_t<T>;

    template<typename T>
    using add_volatile = std::add_volatile_t<T>;

    template<typename T>
    using add_cv = std::add_cv_t<T>;

    template<typename T>
    using add_lvalue_reference = std::add_lvalue_reference_t<T>;

    template<typename T>
    using add_rvalue_reference = std::add_rvalue_reference_t<T>;

    template<typename T>
    using add_pointer = std::add_pointer_t<T>;

    template<bool TCondition, typename TTrue, typename TFalse>
    using TConditional = std::conditional_t<TCondition, TTrue, TFalse>;

    template<typename... Types>
    using TVoid = std::void_t<Types...>;

    template<typename T>
    using TUnderlyingType = std::underlying_type_t<T>;

    template<typename T>
    using TMakeSigned = std::make_signed_t<T>;

    template<typename T>
    using TMakeUnsigned = std::make_unsigned_t<T>;


    // Type Identity and Relationships
    template<typename T, typename U>
    inline constexpr bool IsSame = std::is_same_v<T, U>;

    template<typename TBase, typename TDerived>
    inline constexpr bool IsBaseOf = std::is_base_of_v<TBase, TDerived>;

    template<typename T, typename U>
    inline constexpr bool IsConvertible = std::is_convertible_v<T, U>;

    template<typename T, typename U>
    inline constexpr bool IsAssignable = std::is_assignable_v<T, U>;

    template<typename T, typename U>
    inline constexpr bool IsInvocable = std::is_invocable_v<T, U>;

    template<typename T, typename... TArgs>
    inline constexpr bool IsConstructible = std::is_constructible_v<T, TArgs...>;

    template<typename T, typename... TArgs>
    inline constexpr bool IsNothrowConstructible = std::is_nothrow_constructible_v<T, TArgs...>;


    // Fundamental Type Categories
    template<typename T>
    inline constexpr bool IsVoid = std::is_void_v<T>;

    template<typename T>
    inline constexpr bool IsNullPointer = std::is_null_pointer_v<T>;

    template<typename T>
    inline constexpr bool IsIntegral = std::is_integral_v<T>;

    template<typename T>
    inline constexpr bool IsFloatingPoint = std::is_floating_point_v<T>;

    template<typename T>
    inline constexpr bool IsArithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    inline constexpr bool IsEnum = std::is_enum_v<T>;

    template<typename T>
    inline constexpr bool IsUnion = std::is_union_v<T>;

    template<typename T>
    inline constexpr bool IsClass = std::is_class_v<T>;

    template<typename T>
    inline constexpr bool IsFunction = std::is_function_v<T>;

    template<typename T>
    inline constexpr bool IsPointer = std::is_pointer_v<T>;

    template<typename T>
    inline constexpr bool IsLValueReference = std::is_lvalue_reference_v<T>;

    template<typename T>
    inline constexpr bool IsRValueReference = std::is_rvalue_reference_v<T>;

    template<typename T>
    inline constexpr bool IsReference = std::is_reference_v<T>;

    template<typename T>
    inline constexpr bool IsArray = std::is_array_v<T>;

    template<typename T>
    inline constexpr bool IsMemberPointer = std::is_member_pointer_v<T>;


    // Qualifiers
    template<typename T>
    inline constexpr bool IsConst = std::is_const_v<T>;

    template<typename T>
    inline constexpr bool IsVolatile = std::is_volatile_v<T>;


    // Construction, Assignment, and Destruction
    template<typename T>
    inline constexpr bool IsDefaultConstructible = std::is_default_constructible_v<T>;

    template<typename T>
    inline constexpr bool IsCopyConstructible = std::is_copy_constructible_v<T>;

    template<typename T>
    inline constexpr bool IsMoveConstructible = std::is_move_constructible_v<T>;

    template<typename T>
    inline constexpr bool IsCopyAssignable = std::is_copy_assignable_v<T>;

    template<typename T>
    inline constexpr bool IsMoveAssignable = std::is_move_assignable_v<T>;

    template<typename T>
    inline constexpr bool IsDestructible = std::is_destructible_v<T>;

    template<typename T>
    inline constexpr bool IsNothrowDestructible = std::is_nothrow_destructible_v<T>;


    // Memory, Layout, and Polymorphism
    template<typename T>
    inline constexpr bool IsTrivial = std::is_trivial_v<T>;

    template<typename T>
    inline constexpr bool IsTriviallyCopyable = std::is_trivially_copyable_v<T>;

    template<typename T>
    inline constexpr bool IsStandardLayout = std::is_standard_layout_v<T>;

    template<typename T>
    inline constexpr bool IsEmpty = std::is_empty_v<T>;

    template<typename T>
    inline constexpr bool IsPolymorphic = std::is_polymorphic_v<T>;

    template<typename T>
    inline constexpr bool IsAbstract = std::is_abstract_v<T>;

    template<typename T>
    inline constexpr bool IsFinal = std::is_final_v<T>;

    template<typename T>
    inline constexpr bool HasVirtualDestructor = std::has_virtual_destructor_v<T>;

    template<typename T>
    inline constexpr bool IsAggregate = std::is_aggregate_v<T>;


    // Numeric Properties
    template<typename T>
    inline constexpr bool IsSigned = std::is_signed_v<T>;

    template<typename T>
    inline constexpr bool IsUnsigned = std::is_unsigned_v<T>;


    // Alignment and Array Properties
    template<typename T>
    inline constexpr std::size_t AlignmentOf = std::alignment_of_v<T>;

    template<typename T>
    inline constexpr std::size_t Rank = std::rank_v<T>;

    template<typename T, unsigned int Dimension = 0>
    inline constexpr std::size_t Extent = std::extent_v<T, Dimension>;
}