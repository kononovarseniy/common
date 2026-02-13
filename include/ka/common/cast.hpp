#pragma once

#include <bit>
#include <cmath>
#include <concepts>
#include <limits>
#include <source_location>
#include <type_traits>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/fixed.hpp>

namespace ka
{

/// @brief Concept for integral types that can always be converted to target floating-point type preserving
/// numeric value.
template <typename Source, typename Target>
concept IntegralSafelyConvertibleToIeeeFloatingPoint =
    std::integral<Source> && ieee_float<Target> &&
    std::numeric_limits<Source>::digits <= std::numeric_limits<Target>::digits &&
    // Note: std::numeric_limits<Float>::max_exponent is the bit_width of the largest integer the IEEE floating-point
    // type can hold. It can be smaller than the width of some integer types. For example, u64, u32, u16, and f16.
    std::numeric_limits<Source>::digits <= std::numeric_limits<Target>::max_exponent;

/// @brief Concept for types that can always be converted to target type without preserving numeric value.
template <typename Source, typename Target>
concept SafelyConvertibleTo =
    std::is_arithmetic_v<Source> && std::is_arithmetic_v<Target> &&
    (
        // List-initialization prohibits narrowing conversions.
        requires(Source source) { Target { source }; } ||
        // List-initialization also prohibits integer to floating-point conversions.
        IntegralSafelyConvertibleToIeeeFloatingPoint<Source, Target>);

/// @brief Performs static_cast for types that support conversion preserving numeric value for each possible value
/// of source type.
template <typename Target, SafelyConvertibleTo<Target> Source>
[[nodiscard]] constexpr Target safe_cast(const Source value) noexcept
{
    return static_cast<Target>(value);
}

namespace detail
{

/// @brief Constexpr std::isfinite for C++20.
template <ieee_float T>
[[nodiscard]] constexpr bool is_finite(const T value) noexcept
{
#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
    return std::isfinite(value);
#else
    const bool is_a_number = value == value;
    return is_a_number && value != std::numeric_limits<T>::infinity() && value != -std::numeric_limits<T>::infinity();
#endif
}

/// @brief Checks min value of Source integral type is exactly representable in Target floating-point type.
template <std::signed_integral Source, ieee_float Target>
[[nodiscard]] consteval bool signed_min_is_representable_in() noexcept
{
    return std::numeric_limits<Source>::digits + 1 <= std::numeric_limits<Target>::max_exponent;
}

/// @brief Largest integer value of floating-point type that is representable in target integral type.
template <ieee_float SourceFloat, std::integral TargetInt>
[[nodiscard]] consteval SourceFloat max_castable() noexcept
{
    if constexpr (std::numeric_limits<TargetInt>::digits > std::numeric_limits<SourceFloat>::max_exponent)
    {
        // Max integer value is not representable in floating-point type.
        // Largest common representable integer is the max value of floating-point type, assuming it is integer.
        static_assert(
            std::numeric_limits<SourceFloat>::max_exponent - 1 >= std::numeric_limits<SourceFloat>::digits,
            "Only floating-point types with integer max value are supported");
        return std::numeric_limits<SourceFloat>::max();
    }
    constexpr auto extra_int_digits = std::numeric_limits<TargetInt>::digits - std::numeric_limits<SourceFloat>::digits;
    if constexpr (extra_int_digits < 0)
    {
        // Floating-point type covers all possible integer values.
        return std::numeric_limits<TargetInt>::max();
    }
    else
    {
        constexpr auto result =
            std::numeric_limits<TargetInt>::max() - ((TargetInt { 1 } << extra_int_digits) - TargetInt { 1 });

        static_assert(static_cast<TargetInt>(static_cast<SourceFloat>(result) == result));

        return static_cast<SourceFloat>(result);
    }
}

/// @brief Lowest integer value of floating-point type that is representable in target integral type.
template <ieee_float SourceFloat, std::integral TargetInt>
[[nodiscard]] consteval SourceFloat min_castable() noexcept
{
    if constexpr (std::unsigned_integral<TargetInt>)
    {
        return SourceFloat {};
    }
    else if constexpr (!signed_min_is_representable_in<TargetInt, SourceFloat>())
    {
        static_assert(
            std::numeric_limits<SourceFloat>::max_exponent() - 1 >= std::numeric_limits<SourceFloat>::digits(),
            "Only floating-point types with integer max value are supported");
        return std::numeric_limits<SourceFloat>::lowest();
    }
    return static_cast<SourceFloat>(std::numeric_limits<TargetInt>::min());
}

template <std::integral Source, std::integral Target>
[[nodiscard]] constexpr bool exactly_castable_to_impl(const Source value) noexcept
{
    if constexpr (std::same_as<Target, bool>)
    {
        return value == Source {} || value == Source { 1 };
    }
    else if constexpr (std::same_as<Source, bool>)
    {
        return true;
    }
    else
    {
        return std::in_range<Target>(value);
    }
}

template <ieee_float Source, std::integral Target>
[[nodiscard]] constexpr bool exactly_castable_to_impl(const Source value) noexcept
{
    if (!is_finite(value))
    {
        return false;
    }
    // We need to check if the value lies between the smallest and largest integers that can be exactly represented by
    // the floating-point type. This ensures that no overflow occurs during conversions to the integer type and back to
    // floating-point type.
    const auto safe_to_cast = min_castable<Source, Target>() <= value && value <= max_castable<Source, Target>();
    // In C++23 we can use constexpr std::trunc to check value is integer and just check for range.
    return safe_to_cast && static_cast<Source>(static_cast<Target>(value)) == value;
}

template <std::unsigned_integral Source, ieee_float Target>
[[nodiscard]] constexpr bool exactly_castable_to_impl(const Source value) noexcept
{
    if constexpr (std::same_as<Source, bool>)
    {
        static_assert(std::numeric_limits<Target>::digits > 0);
        return !value || std::numeric_limits<Target>::max_exponent >= 1;
    }
    else
    {
        // Required exponent may be too large.
        if (std::bit_width(value) > std::numeric_limits<Target>::max_exponent)
        {
            return false;
        }
        // Number of digits the value actually uses should not be greater than the width of the mantissa (counting
        // implicit leading 1).
        // Note: Can be negative because both std::countl_zero(0) and std::countr_zero(0)  return the total number of
        // bits for the type.
        const auto used_bits = std::numeric_limits<Source>::digits - std::countl_zero(value) - std::countr_zero(value);
        return used_bits <= std::numeric_limits<Target>::digits;
    }
}

template <std::signed_integral Source, ieee_float Target>
[[nodiscard]] constexpr bool exactly_castable_to_impl(const Source value) noexcept
{
    if (value == std::numeric_limits<Source>::min())
    {
        return signed_min_is_representable_in<Source, Target>();
    }
    using UnsignedSource = std::make_unsigned_t<Source>;
    return exactly_castable_to_impl<UnsignedSource, Target>(static_cast<UnsignedSource>(value < 0 ? -value : value));
}

template <ieee_float Source, ieee_float Target>
[[nodiscard]] constexpr bool exactly_castable_to_impl(const Source value) noexcept
{
    // Infinities can be converted exactly and nobody cares about distinct NaN values.
    if (!is_finite(value))
    {
        return true;
    }
    using CommonType = std::common_type_t<Source, Target>;
    constexpr CommonType target_min { std::numeric_limits<Target>::lowest() };
    constexpr CommonType target_max { std::numeric_limits<Target>::max() };

    // Casting out of range value is UB even for IEEE floats.
    const auto in_range = target_min <= CommonType { value } && CommonType { value } <= target_max;
    return in_range && static_cast<Source>(static_cast<Target>(value)) == value;
}

} // namespace detail

/// @brief Checks whether the given value can be converted to the target type preserving its numeric value.
template <typename Target, typename Source>
    requires std::is_arithmetic_v<Target> && std::is_arithmetic_v<Source>
[[nodiscard]] constexpr bool exactly_castable_to(const Source value) noexcept
{
    if constexpr (SafelyConvertibleTo<Source, Target>)
    {
        return true;
    }
    else
    {
        return detail::exactly_castable_to_impl<Source, Target>(value);
    }
};

/// @brief Converts value to the target type ensuring the numeric value is preserved.
template <typename Target, typename Source>
    requires std::is_arithmetic_v<Target> && std::is_arithmetic_v<Source>
[[nodiscard]] Target exact_cast(
    const Source source,
    const std::source_location & location = std::source_location::current())
{
    AR_NESTED_ASSERT(exactly_castable_to<Target>(source), "exact_cast", location);
    AR_POST(static_cast<Source>(static_cast<Target>(source)) == source);
    return static_cast<Target>(source);
}

} // namespace ka
