#include <gtest/gtest.h>

#include <cmath>
#include <concepts>
#include <limits>
#include <type_traits>

#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>

namespace ka
{

inline namespace
{

enum class TestEnum : u8
{
};

#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L

template <typename T>
[[nodiscard]] consteval T next_float(const T value) noexcept
{
    return std::nextafter(value, std::numeric_limits<T>::infinity());
}

template <typename T>
[[nodiscard]] consteval T prev_float(const T value) noexcept
{
    return std::nextafter(value, -std::numeric_limits<T>::infinity());
}

#else

/// @brief Constexpr C++20 alternative to std::nextafter.
/// @note Non finite values, zeros and largest finite values are not supported.
template <typename T>
    requires(std::same_as<T, f32> || std::same_as<T, f64>)
[[nodiscard]] consteval T next_float(const T value) noexcept
{
    using BitsType = std::conditional_t<std::same_as<T, f32>, u32, u64>;
    const auto bits = std::bit_cast<BitsType>(value);
    // https://randomascii.wordpress.com/2012/01/23/stupid-float-tricks-2/
    return std::bit_cast<T>(value > 0 ? bits + 1 : bits - 1);
}

/// @brief Constexpr C++20 alternative to std::nextafter.
/// @note Non finite values, zeros and largest finite values are not supported.
template <typename T>
    requires(std::same_as<T, f32> || std::same_as<T, f64>)
[[nodiscard]] consteval T prev_float(const T value) noexcept
{
    using BitsType = std::conditional_t<std::same_as<T, f32>, u32, u64>;
    auto bits = std::bit_cast<BitsType>(value);
    // https://randomascii.wordpress.com/2012/01/23/stupid-float-tricks-2/
    return std::bit_cast<T>(value > 0 ? bits - 1 : bits + 1);
}

static_assert(-0x7fff.ff8p0 == next_float(f32 { std::numeric_limits<s16>::min() }));
static_assert(-0x8000.010p0 == prev_float(f32 { std::numeric_limits<s16>::min() }));
static_assert(+0x7fff.008p0 == next_float(f32 { std::numeric_limits<s16>::max() }));
static_assert(+0x7ffe.ff8p0 == prev_float(f32 { std::numeric_limits<s16>::max() }));

#endif

} // namespace

TEST(SafeCastTestSuite, enum_and_underlying_type)
{
    static_assert(!SafelyConvertibleTo<u8, TestEnum>);
    static_assert(!SafelyConvertibleTo<TestEnum, u8>);

    static_assert(!SafelyConvertibleTo<u32, TestEnum>);
    static_assert(!SafelyConvertibleTo<TestEnum, u32>);
}

TEST(SafeCastTestSuite, integral_to_integral)
{
    static_assert(SafelyConvertibleTo<bool, bool>);
    static_assert(SafelyConvertibleTo<bool, u8>);
    static_assert(SafelyConvertibleTo<bool, s8>);
    static_assert(SafelyConvertibleTo<bool, u16>);
    static_assert(SafelyConvertibleTo<bool, s16>);
    static_assert(SafelyConvertibleTo<bool, u32>);
    static_assert(SafelyConvertibleTo<bool, s32>);
    static_assert(SafelyConvertibleTo<bool, u64>);
    static_assert(SafelyConvertibleTo<bool, s64>);

    static_assert(!SafelyConvertibleTo<u8, bool>);
    static_assert(SafelyConvertibleTo<u8, u8>);
    static_assert(!SafelyConvertibleTo<u8, s8>);
    static_assert(SafelyConvertibleTo<u8, u16>);
    static_assert(SafelyConvertibleTo<u8, s16>);
    static_assert(SafelyConvertibleTo<u8, u32>);
    static_assert(SafelyConvertibleTo<u8, s32>);
    static_assert(SafelyConvertibleTo<u8, u64>);
    static_assert(SafelyConvertibleTo<u8, s64>);
    static_assert(!SafelyConvertibleTo<s8, bool>);
    static_assert(!SafelyConvertibleTo<s8, u8>);
    static_assert(SafelyConvertibleTo<s8, s8>);
    static_assert(!SafelyConvertibleTo<s8, u16>);
    static_assert(SafelyConvertibleTo<s8, s16>);
    static_assert(!SafelyConvertibleTo<s8, u32>);
    static_assert(SafelyConvertibleTo<s8, s32>);
    static_assert(!SafelyConvertibleTo<s8, u64>);
    static_assert(SafelyConvertibleTo<s8, s64>);

    static_assert(!SafelyConvertibleTo<u16, bool>);
    static_assert(!SafelyConvertibleTo<u16, u8>);
    static_assert(!SafelyConvertibleTo<u16, s8>);
    static_assert(SafelyConvertibleTo<u16, u16>);
    static_assert(!SafelyConvertibleTo<u16, s16>);
    static_assert(SafelyConvertibleTo<u16, u32>);
    static_assert(SafelyConvertibleTo<u16, s32>);
    static_assert(SafelyConvertibleTo<u16, u64>);
    static_assert(SafelyConvertibleTo<u16, s64>);
    static_assert(!SafelyConvertibleTo<s16, bool>);
    static_assert(!SafelyConvertibleTo<s16, u8>);
    static_assert(!SafelyConvertibleTo<s16, s8>);
    static_assert(!SafelyConvertibleTo<s16, u16>);
    static_assert(SafelyConvertibleTo<s16, s16>);
    static_assert(!SafelyConvertibleTo<s16, u32>);
    static_assert(SafelyConvertibleTo<s16, s32>);
    static_assert(!SafelyConvertibleTo<s16, u64>);
    static_assert(SafelyConvertibleTo<s16, s64>);

    static_assert(!SafelyConvertibleTo<u32, bool>);
    static_assert(!SafelyConvertibleTo<u32, u8>);
    static_assert(!SafelyConvertibleTo<u32, s8>);
    static_assert(!SafelyConvertibleTo<u32, u16>);
    static_assert(!SafelyConvertibleTo<u32, s16>);
    static_assert(SafelyConvertibleTo<u32, u32>);
    static_assert(!SafelyConvertibleTo<u32, s32>);
    static_assert(SafelyConvertibleTo<u32, u64>);
    static_assert(SafelyConvertibleTo<u32, s64>);
    static_assert(!SafelyConvertibleTo<s32, bool>);
    static_assert(!SafelyConvertibleTo<s32, u8>);
    static_assert(!SafelyConvertibleTo<s32, s8>);
    static_assert(!SafelyConvertibleTo<s32, u16>);
    static_assert(!SafelyConvertibleTo<s32, s16>);
    static_assert(!SafelyConvertibleTo<s32, u32>);
    static_assert(SafelyConvertibleTo<s32, s32>);
    static_assert(!SafelyConvertibleTo<s32, u64>);
    static_assert(SafelyConvertibleTo<s32, s64>);

    static_assert(!SafelyConvertibleTo<u64, bool>);
    static_assert(!SafelyConvertibleTo<u64, u8>);
    static_assert(!SafelyConvertibleTo<u64, s8>);
    static_assert(!SafelyConvertibleTo<u64, u16>);
    static_assert(!SafelyConvertibleTo<u64, s16>);
    static_assert(!SafelyConvertibleTo<u64, u32>);
    static_assert(!SafelyConvertibleTo<u64, s32>);
    static_assert(SafelyConvertibleTo<u64, u64>);
    static_assert(!SafelyConvertibleTo<u64, s64>);
    static_assert(!SafelyConvertibleTo<s64, bool>);
    static_assert(!SafelyConvertibleTo<s64, u8>);
    static_assert(!SafelyConvertibleTo<s64, s8>);
    static_assert(!SafelyConvertibleTo<s64, u16>);
    static_assert(!SafelyConvertibleTo<s64, s16>);
    static_assert(!SafelyConvertibleTo<s64, u32>);
    static_assert(!SafelyConvertibleTo<s64, s32>);
    static_assert(!SafelyConvertibleTo<s64, u64>);
    static_assert(SafelyConvertibleTo<s64, s64>);
}

TEST(SafeCastTestSuite, floating_point_to_floating_point)
{
    static_assert(SafelyConvertibleTo<f32, f32>);
    static_assert(SafelyConvertibleTo<f32, f64>);
    static_assert(!SafelyConvertibleTo<f64, f32>);
    static_assert(SafelyConvertibleTo<f64, f64>);
}

TEST(SafeCastTestSuite, integral_to_floating_point)
{
    static_assert(SafelyConvertibleTo<bool, f32>);
    static_assert(SafelyConvertibleTo<bool, f64>);

    static_assert(SafelyConvertibleTo<u8, f32>);
    static_assert(SafelyConvertibleTo<u8, f64>);
    static_assert(SafelyConvertibleTo<s8, f32>);
    static_assert(SafelyConvertibleTo<s8, f64>);

    static_assert(SafelyConvertibleTo<u16, f32>);
    static_assert(SafelyConvertibleTo<u16, f64>);
    static_assert(SafelyConvertibleTo<s16, f32>);
    static_assert(SafelyConvertibleTo<s16, f64>);

    static_assert(!SafelyConvertibleTo<u32, f32>);
    static_assert(SafelyConvertibleTo<u32, f64>);
    static_assert(!SafelyConvertibleTo<s32, f32>);
    static_assert(SafelyConvertibleTo<s32, f64>);

    static_assert(!SafelyConvertibleTo<u64, f32>);
    static_assert(!SafelyConvertibleTo<u64, f64>);
    static_assert(!SafelyConvertibleTo<s64, f32>);
    static_assert(!SafelyConvertibleTo<s64, f64>);
}

TEST(SafeCastTestSuite, floating_point_to_integral)
{
    static_assert(!SafelyConvertibleTo<f32, bool>);
    static_assert(!SafelyConvertibleTo<f32, u8>);
    static_assert(!SafelyConvertibleTo<f32, s8>);
    static_assert(!SafelyConvertibleTo<f32, u16>);
    static_assert(!SafelyConvertibleTo<f32, s16>);
    static_assert(!SafelyConvertibleTo<f32, u32>);
    static_assert(!SafelyConvertibleTo<f32, s32>);
    static_assert(!SafelyConvertibleTo<f32, u64>);
    static_assert(!SafelyConvertibleTo<f32, s64>);

    static_assert(!SafelyConvertibleTo<f64, bool>);
    static_assert(!SafelyConvertibleTo<f64, u8>);
    static_assert(!SafelyConvertibleTo<f64, s8>);
    static_assert(!SafelyConvertibleTo<f64, u16>);
    static_assert(!SafelyConvertibleTo<f64, s16>);
    static_assert(!SafelyConvertibleTo<f64, u32>);
    static_assert(!SafelyConvertibleTo<f64, s32>);
    static_assert(!SafelyConvertibleTo<f64, u64>);
    static_assert(!SafelyConvertibleTo<f64, s64>);
}

TEST(SafeCastTestSuite, safe_cast_instantiation)
{
    const u16 value = 42424;
    const auto result = safe_cast<s64>(value);
    static_assert(std::same_as<decltype(result), const s64>);
    EXPECT_EQ(result, s64 { 42424 });
}

TEST(ExactCastTestSuite, exact_cast_instantiation)
{
    EXPECT_EQ(exact_cast<bool>(bool { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(u8 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(s8 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(u16 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(s16 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(u32 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(s32 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(u64 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<bool>(s64 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<u8>(bool { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(u8 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(s8 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(u16 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(s16 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(u32 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(s32 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(u64 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<u8>(s64 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<s8>(bool { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(u8 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(s8 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(u16 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(s16 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(u32 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(s32 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(u64 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<s8>(s64 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<u16>(bool { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(u8 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(s8 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(u16 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(s16 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(u32 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(s32 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(u64 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<u16>(s64 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<s16>(bool { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(u8 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(s8 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(u16 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(s16 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(u32 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(s32 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(u64 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<s16>(s64 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<u32>(bool { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(u8 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(s8 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(u16 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(s16 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(u32 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(s32 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(u64 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<u32>(s64 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<s32>(bool { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(u8 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(s8 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(u16 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(s16 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(u32 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(s32 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(u64 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<s32>(s64 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<u64>(bool { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(u8 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(s8 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(u16 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(s16 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(u32 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(s32 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(u64 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<u64>(s64 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<s64>(bool { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(u8 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(s8 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(u16 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(s16 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(u32 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(s32 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(u64 { 0 }), s64 { 0 });
    EXPECT_EQ(exact_cast<s64>(s64 { 0 }), s64 { 0 });

    EXPECT_EQ(exact_cast<bool>(f32 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<u8>(f32 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<s8>(f32 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<u16>(f32 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<s16>(f32 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<u32>(f32 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<s32>(f32 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<u64>(f32 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<s64>(f32 { 0 }), s64 { 0 });

    EXPECT_EQ(exact_cast<bool>(f64 { 0 }), bool { 0 });
    EXPECT_EQ(exact_cast<u8>(f64 { 0 }), u8 { 0 });
    EXPECT_EQ(exact_cast<s8>(f64 { 0 }), s8 { 0 });
    EXPECT_EQ(exact_cast<u16>(f64 { 0 }), u16 { 0 });
    EXPECT_EQ(exact_cast<s16>(f64 { 0 }), s16 { 0 });
    EXPECT_EQ(exact_cast<u32>(f64 { 0 }), u32 { 0 });
    EXPECT_EQ(exact_cast<s32>(f64 { 0 }), s32 { 0 });
    EXPECT_EQ(exact_cast<u64>(f64 { 0 }), u64 { 0 });
    EXPECT_EQ(exact_cast<s64>(f64 { 0 }), s64 { 0 });

    EXPECT_EQ(exact_cast<f32>(bool { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(u8 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(s8 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(u16 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(s16 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(u32 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(s32 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(u64 { 0 }), f32 { 0 });
    EXPECT_EQ(exact_cast<f32>(s64 { 0 }), f32 { 0 });

    EXPECT_EQ(exact_cast<f64>(bool { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(u8 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(s8 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(u16 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(s16 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(u32 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(s32 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(u64 { 0 }), f64 { 0 });
    EXPECT_EQ(exact_cast<f64>(s64 { 0 }), f64 { 0 });
}

TEST(ExactCastTestSuite, cast_to_bool)
{
    static_assert(exactly_castable_to<bool>(false));
    static_assert(exactly_castable_to<bool>(true));

    static_assert(exactly_castable_to<bool>(u8 { 0 }));
    static_assert(exactly_castable_to<bool>(u8 { 1 }));
    static_assert(!exactly_castable_to<bool>(u8 { 2 }));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<u8>::max()));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<u8>::max() - 1));

    static_assert(exactly_castable_to<bool>(s8 { 0 }));
    static_assert(exactly_castable_to<bool>(s8 { 1 }));
    static_assert(!exactly_castable_to<bool>(s8 { -1 }));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<s8>::min()));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<s8>::min() + 1));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<s8>::max() - 1));

    static_assert(exactly_castable_to<bool>(f32 { 0 }));
    static_assert(exactly_castable_to<bool>(f32 { 1 }));
    static_assert(!exactly_castable_to<bool>(f32 { 0.5 }));
    static_assert(!exactly_castable_to<bool>(f32 { -0.5 }));
    static_assert(!exactly_castable_to<bool>(f32 { 2 }));
    static_assert(!exactly_castable_to<bool>(f32 { -2 }));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<f32>::lowest()));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<f32>::max()));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<bool>(std::numeric_limits<f32>::quiet_NaN()));
}

TEST(ExactCastTestSuite, signed_int_to_unsigned_int)
{
    static_assert(exactly_castable_to<u8>(s8 { 0 }));
    static_assert(exactly_castable_to<u8>(s8 { 127 }));
    static_assert(!exactly_castable_to<u8>(std::numeric_limits<s8>::min()));
    static_assert(exactly_castable_to<u8>(std::numeric_limits<s8>::max()));

    static_assert(exactly_castable_to<u8>(s16 { 0 }));
    static_assert(exactly_castable_to<u8>(s16 { 127 }));
    static_assert(!exactly_castable_to<u8>(std::numeric_limits<s16>::min()));
    static_assert(!exactly_castable_to<u8>(std::numeric_limits<s16>::max()));

    static_assert(exactly_castable_to<u8>(s32 { 0 }));
    static_assert(exactly_castable_to<u8>(s32 { 127 }));
    static_assert(!exactly_castable_to<u8>(s32 { -127 }));
    static_assert(!exactly_castable_to<u8>(std::numeric_limits<s32>::min()));
    static_assert(!exactly_castable_to<u8>(std::numeric_limits<s32>::max()));
    static_assert(exactly_castable_to<u8>(s32 { std::numeric_limits<u8>::max() }));

    static_assert(exactly_castable_to<u16>(s8 { 0 }));
    static_assert(exactly_castable_to<u16>(s8 { 127 }));
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<s8>::min()));
    static_assert(exactly_castable_to<u16>(std::numeric_limits<s8>::max()));
}

TEST(ExactCastTestSuite, signed_int_to_signed_int)
{
    static_assert(exactly_castable_to<s8>(s16 { 0 }));
    static_assert(exactly_castable_to<s8>(s16 { -1 }));
    static_assert(exactly_castable_to<s8>(s16 { 1 }));
    static_assert(exactly_castable_to<s8>(s16 { std::numeric_limits<s8>::min() }));
    static_assert(exactly_castable_to<s8>(s16 { std::numeric_limits<s8>::max() }));
    static_assert(!exactly_castable_to<s8>(std::numeric_limits<s16>::min()));
    static_assert(!exactly_castable_to<s8>(std::numeric_limits<s16>::max()));

    static_assert(exactly_castable_to<s32>(s32 { 0 }));
    static_assert(exactly_castable_to<s32>(s32 { -1 }));
    static_assert(exactly_castable_to<s32>(s32 { 1 }));
    static_assert(exactly_castable_to<s32>(std::numeric_limits<s32>::min()));
    static_assert(exactly_castable_to<s32>(std::numeric_limits<s32>::max()));

    static_assert(exactly_castable_to<s64>(s32 { 0 }));
    static_assert(exactly_castable_to<s64>(s32 { -1 }));
    static_assert(exactly_castable_to<s64>(s32 { 1 }));
    static_assert(exactly_castable_to<s64>(std::numeric_limits<s32>::min()));
    static_assert(exactly_castable_to<s64>(std::numeric_limits<s32>::max()));
}

TEST(ExactCastTestSuite, unsigned_int_to_unsigned_int)
{
    static_assert(exactly_castable_to<u8>(u32 { 0 }));
    static_assert(exactly_castable_to<u8>(u32 { 127 }));
    static_assert(exactly_castable_to<u8>(u32 { std::numeric_limits<u8>::max() }));

    static_assert(exactly_castable_to<u8>(u8 { 0 }));
    static_assert(exactly_castable_to<u8>(u8 { 127 }));
    static_assert(exactly_castable_to<u8>(u8 { std::numeric_limits<u8>::max() }));

    static_assert(exactly_castable_to<u64>(u16 { 0 }));
    static_assert(exactly_castable_to<u64>(u16 { 1 }));
    static_assert(exactly_castable_to<u64>(std::numeric_limits<u16>::max()));
}

TEST(ExactCastTestSuite, unsigned_int_to_signed_int)
{
    static_assert(exactly_castable_to<s8>(u32 { 0 }));
    static_assert(exactly_castable_to<s8>(u32 { 127 }));
    static_assert(exactly_castable_to<s8>(u32 { std::numeric_limits<s8>::max() }));
    static_assert(!exactly_castable_to<s8>(u32 { std::numeric_limits<s8>::max() } + 1));
    static_assert(exactly_castable_to<s8>(std::numeric_limits<u32>::min()));
    static_assert(!exactly_castable_to<s8>(std::numeric_limits<u32>::max()));

    static_assert(exactly_castable_to<s32>(u32 { 0 }));
    static_assert(exactly_castable_to<s32>(u32 { 1 }));
    static_assert(exactly_castable_to<s32>(u32 { std::numeric_limits<s32>::max() } - 1));
    static_assert(exactly_castable_to<s32>(u32 { std::numeric_limits<s32>::max() }));
    static_assert(!exactly_castable_to<s32>(std::numeric_limits<u32>::max()));

    static_assert(exactly_castable_to<s64>(u32 { 0 }));
    static_assert(exactly_castable_to<s64>(u32 { 1 }));
    static_assert(exactly_castable_to<s64>(std::numeric_limits<u32>::max()));
}

TEST(ExactCastTestSuite, detail_max_castable)
{
    static_assert(detail::max_castable<f32, bool>() == 0x1p0);
    static_assert(detail::max_castable<f32, s8>() == 0x7fp0);
    static_assert(detail::max_castable<f32, u8>() == 0xffp0);
    static_assert(detail::max_castable<f32, s16>() == 0x7fffp0);
    static_assert(detail::max_castable<f32, u16>() == 0xffffp0);
    static_assert(detail::max_castable<f32, s32>() == 0x7fffff80p0);
    static_assert(detail::max_castable<f32, u32>() == 0xffffff00p0);
    static_assert(detail::max_castable<f32, s64>() == 0x7fffff8000000000p0);
    static_assert(detail::max_castable<f32, u64>() == 0xffffff0000000000p0);

    static_assert(detail::max_castable<f64, bool>() == 0x1p0);
    static_assert(detail::max_castable<f64, s8>() == 0x7fp0);
    static_assert(detail::max_castable<f64, u8>() == 0xffp0);
    static_assert(detail::max_castable<f64, s16>() == 0x7fffp0);
    static_assert(detail::max_castable<f64, u16>() == 0xffffp0);
    static_assert(detail::max_castable<f64, s32>() == 0x7fffffffp0);
    static_assert(detail::max_castable<f64, u32>() == 0xffffffffp0);
    static_assert(detail::max_castable<f64, s64>() == 0x7ffffffffffffc00p0);
    static_assert(detail::max_castable<f64, u64>() == 0xfffffffffffff800p0);
}

TEST(ExactCastTestSuite, float_to_signed_int)
{
    static_assert(!exactly_castable_to<s16>(std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<s16>(-std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<s16>(std::numeric_limits<f32>::quiet_NaN()));
    static_assert(!exactly_castable_to<s16>(std::numeric_limits<f32>::lowest()));
    static_assert(!exactly_castable_to<s16>(std::numeric_limits<f32>::min()));
    static_assert(!exactly_castable_to<s16>(std::numeric_limits<f32>::max()));

    static_assert(!exactly_castable_to<s32>(std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<s32>(-std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<s32>(std::numeric_limits<f32>::quiet_NaN()));
    static_assert(!exactly_castable_to<s32>(std::numeric_limits<f32>::lowest()));
    static_assert(!exactly_castable_to<s32>(std::numeric_limits<f32>::min()));
    static_assert(!exactly_castable_to<s32>(std::numeric_limits<f32>::max()));

    static_assert(exactly_castable_to<s16>(f32 { 0.0 }));
    static_assert(exactly_castable_to<s16>(f32 { -1.0 }));
    static_assert(exactly_castable_to<s16>(f32 { 1.0 }));
    static_assert(!exactly_castable_to<s16>(f32 { 1.1 }));

    static_assert(exactly_castable_to<s32>(f32 { 0.0 }));
    static_assert(exactly_castable_to<s32>(f32 { -1.0 }));
    static_assert(exactly_castable_to<s32>(f32 { 1.0 }));
    static_assert(!exactly_castable_to<s32>(f32 { 1.1 }));

    static_assert(exactly_castable_to<s16>(f64 { 0.0 }));
    static_assert(exactly_castable_to<s16>(f64 { -1.0 }));
    static_assert(exactly_castable_to<s16>(f64 { 1.0 }));
    static_assert(!exactly_castable_to<s16>(f64 { 1.1 }));

    static_assert(exactly_castable_to<s32>(f64 { 0.0 }));
    static_assert(exactly_castable_to<s32>(f64 { -1.0 }));
    static_assert(exactly_castable_to<s32>(f64 { 1.0 }));
    static_assert(!exactly_castable_to<s32>(f64 { 1.1 }));

    static_assert(exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::min() }));
    static_assert(exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::max() }));
    static_assert(exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::min() + s16 { 1 } }));
    static_assert(exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::max() - s16 { 1 } }));
    static_assert(!exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::min() - s32 { 1 } }));
    static_assert(!exactly_castable_to<s16>(f32 { std::numeric_limits<s16>::max() + s32 { 1 } }));

    static_assert(!exactly_castable_to<s16>(next_float(f32 { std::numeric_limits<s16>::min() })));
    static_assert(!exactly_castable_to<s16>(prev_float(f32 { std::numeric_limits<s16>::min() })));
    static_assert(!exactly_castable_to<s16>(next_float(f32 { std::numeric_limits<s16>::max() })));
    static_assert(!exactly_castable_to<s16>(prev_float(f32 { std::numeric_limits<s16>::max() })));

    static_assert(exactly_castable_to<s32>(f32 { -0x1p31 }));
    static_assert(exactly_castable_to<s32>(next_float(f32 { -0x1p31 })));
    static_assert(!exactly_castable_to<s32>(prev_float(f32 { -0x1p31 })));
    static_assert(exactly_castable_to<s32>(f32 { 0x7fffff80p0 }));
    static_assert(exactly_castable_to<s32>(prev_float(f32 { 0x7fffff80p0 })));
    static_assert(!exactly_castable_to<s32>(next_float(f32 { 0x7fffff80p0 })));

    static_assert(exactly_castable_to<s64>(f64 { -0x1p63 }));
    static_assert(exactly_castable_to<s64>(next_float(f64 { -0x1p63 })));
    static_assert(!exactly_castable_to<s64>(prev_float(f64 { -0x1p63 })));
    static_assert(exactly_castable_to<s64>(f64 { 0x7ffffffffffffc00p0 }));
    static_assert(exactly_castable_to<s64>(prev_float(f64 { 0x7ffffffffffffc00p0 })));
    static_assert(!exactly_castable_to<s64>(next_float(f64 { 0x7ffffffffffffc00p0 })));
}

TEST(ExactCastTestSuite, float_to_unsigned_int)
{
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<u16>(-std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<f32>::quiet_NaN()));
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<f32>::lowest()));
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<f32>::min()));
    static_assert(!exactly_castable_to<u16>(std::numeric_limits<f32>::max()));

    static_assert(!exactly_castable_to<u32>(std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<u32>(-std::numeric_limits<f32>::infinity()));
    static_assert(!exactly_castable_to<u32>(std::numeric_limits<f32>::quiet_NaN()));
    static_assert(!exactly_castable_to<u32>(std::numeric_limits<f32>::lowest()));
    static_assert(!exactly_castable_to<u32>(std::numeric_limits<f32>::min()));
    static_assert(!exactly_castable_to<u32>(std::numeric_limits<f32>::max()));

    static_assert(exactly_castable_to<u16>(f32 { 0.0 }));
    static_assert(!exactly_castable_to<u16>(f32 { -1.0 }));
    static_assert(exactly_castable_to<u16>(f32 { 1.0 }));
    static_assert(!exactly_castable_to<u16>(f32 { 1.1 }));

    static_assert(exactly_castable_to<u32>(f32 { 0.0 }));
    static_assert(!exactly_castable_to<u32>(f32 { -1.0 }));
    static_assert(exactly_castable_to<u32>(f32 { 1.0 }));
    static_assert(!exactly_castable_to<u32>(f32 { 1.1 }));

    static_assert(exactly_castable_to<u16>(f64 { 0.0 }));
    static_assert(!exactly_castable_to<u16>(f64 { -1.0 }));
    static_assert(exactly_castable_to<u16>(f64 { 1.0 }));
    static_assert(!exactly_castable_to<u16>(f64 { 1.1 }));

    static_assert(exactly_castable_to<u32>(f64 { 0.0 }));
    static_assert(!exactly_castable_to<u32>(f64 { -1.0 }));
    static_assert(exactly_castable_to<u32>(f64 { 1.0 }));
    static_assert(!exactly_castable_to<u32>(f64 { 1.1 }));

    static_assert(exactly_castable_to<u16>(f32 { std::numeric_limits<u16>::min() }));
    static_assert(exactly_castable_to<u16>(f32 { std::numeric_limits<u16>::max() }));
    static_assert(exactly_castable_to<u16>(f32 { std::numeric_limits<u16>::min() + u16 { 1 } }));
    static_assert(exactly_castable_to<u16>(f32 { std::numeric_limits<u16>::max() - u16 { 1 } }));
    static_assert(!exactly_castable_to<u16>(f32 { std::numeric_limits<u16>::max() + u32 { 1 } }));

    static_assert(!exactly_castable_to<u16>(next_float(f32 { std::numeric_limits<u16>::max() })));
    static_assert(!exactly_castable_to<u16>(prev_float(f32 { std::numeric_limits<u16>::max() })));

    static_assert(exactly_castable_to<u32>(f32 { 0xffffff00p0 }));
    static_assert(exactly_castable_to<u32>(prev_float(f32 { 0xffffff00p0 })));
    static_assert(!exactly_castable_to<u32>(next_float(f32 { 0xffffff00p0 })));

    static_assert(exactly_castable_to<u64>(f64 { 0xfffffffffffff800p0 }));
    static_assert(exactly_castable_to<u64>(prev_float(f64 { 0xfffffffffffff800p0 })));
    static_assert(!exactly_castable_to<u64>(next_float(f64 { 0xfffffffffffff800p0 })));
}

TEST(ExactCastTestSuite, unsigned_int_to_float)
{
    static_assert(exactly_castable_to<f32>(false));
    static_assert(exactly_castable_to<f32>(true));
    static_assert(exactly_castable_to<f32>(u32 { 0 }));
    static_assert(exactly_castable_to<f32>(u32 { 1 }));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<u8>::max()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<u16>::max()));
    static_assert(exactly_castable_to<f32>(u32 { std::numeric_limits<u16>::max() }));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<u32>::max()));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<u64>::max()));

    static_assert(exactly_castable_to<f64>(u32 { 0 }));
    static_assert(exactly_castable_to<f64>(u32 { 1 }));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<u8>::max()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<u16>::max()));
    static_assert(exactly_castable_to<f64>(u32 { std::numeric_limits<u16>::max() }));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<u32>::max()));
    static_assert(!exactly_castable_to<f64>(std::numeric_limits<u64>::max()));

    static_assert(exactly_castable_to<f32>(u32 { 0xffffff }));
    static_assert(exactly_castable_to<f32>(u32 { 0x1000000 }));
    static_assert(!exactly_castable_to<f32>(u32 { 0x1000001 }));
    static_assert(exactly_castable_to<f32>(u32 { 0x1000002 }));
    static_assert(exactly_castable_to<f32>(u32 { 0x2000000 }));
    static_assert(!exactly_castable_to<f32>(u32 { 0x2000001 }));
    static_assert(!exactly_castable_to<f32>(u32 { 0x2000002 }));
    static_assert(!exactly_castable_to<f32>(u32 { 0x2000003 }));
    static_assert(exactly_castable_to<f32>(u32 { 0x2000004 }));
}

TEST(ExactCastTestSuite, signed_int_to_float)
{
    static_assert(exactly_castable_to<f32>(s32 { 0 }));
    static_assert(exactly_castable_to<f32>(s32 { 1 }));
    static_assert(exactly_castable_to<f32>(s32 { -1 }));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s8>::min()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s8>::max()));
    static_assert(exactly_castable_to<f32>(-std::numeric_limits<s8>::max()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s16>::min()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s16>::max()));
    static_assert(exactly_castable_to<f32>(-std::numeric_limits<s16>::max()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s32>::min()));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<s32>::max()));
    static_assert(!exactly_castable_to<f32>(-std::numeric_limits<s32>::max()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<s64>::min()));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<s64>::max()));
    static_assert(!exactly_castable_to<f32>(-std::numeric_limits<s64>::max()));

    static_assert(exactly_castable_to<f64>(s32 { 0 }));
    static_assert(exactly_castable_to<f64>(s32 { 1 }));
    static_assert(exactly_castable_to<f64>(s32 { -1 }));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s8>::min()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s8>::max()));
    static_assert(exactly_castable_to<f64>(-std::numeric_limits<s8>::max()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s16>::min()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s16>::max()));
    static_assert(exactly_castable_to<f64>(-std::numeric_limits<s16>::max()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s32>::min()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s32>::max()));
    static_assert(exactly_castable_to<f64>(-std::numeric_limits<s32>::max()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<s64>::min()));
    static_assert(!exactly_castable_to<f64>(std::numeric_limits<s64>::max()));
    static_assert(!exactly_castable_to<f64>(-std::numeric_limits<s64>::max()));

    static_assert(exactly_castable_to<f32>(s32 { 0xffffff }));
    static_assert(exactly_castable_to<f32>(s32 { 0x1000000 }));
    static_assert(!exactly_castable_to<f32>(s32 { 0x1000001 }));
    static_assert(exactly_castable_to<f32>(s32 { 0x1000002 }));
    static_assert(exactly_castable_to<f32>(s32 { 0x2000000 }));
    static_assert(!exactly_castable_to<f32>(s32 { 0x2000001 }));
    static_assert(!exactly_castable_to<f32>(s32 { 0x2000002 }));
    static_assert(!exactly_castable_to<f32>(s32 { 0x2000003 }));
    static_assert(exactly_castable_to<f32>(s32 { 0x2000004 }));

    static_assert(exactly_castable_to<f32>(s32 { -0xffffff }));
    static_assert(exactly_castable_to<f32>(s32 { -0x1000000 }));
    static_assert(!exactly_castable_to<f32>(s32 { -0x1000001 }));
    static_assert(exactly_castable_to<f32>(s32 { -0x1000002 }));
    static_assert(exactly_castable_to<f32>(s32 { -0x2000000 }));
    static_assert(!exactly_castable_to<f32>(s32 { -0x2000001 }));
    static_assert(!exactly_castable_to<f32>(s32 { -0x2000002 }));
    static_assert(!exactly_castable_to<f32>(s32 { -0x2000003 }));
    static_assert(exactly_castable_to<f32>(s32 { -0x2000004 }));
}

TEST(ExactCastTestSuite, float_to_float)
{
    static_assert(exactly_castable_to<f32>(std::numeric_limits<f64>::infinity()));
    static_assert(exactly_castable_to<f32>(-std::numeric_limits<f64>::infinity()));
    static_assert(exactly_castable_to<f32>(std::numeric_limits<f64>::quiet_NaN()));
    static_assert(exactly_castable_to<f32>(f64 { 0 }));
    static_assert(exactly_castable_to<f32>(f64 { -1.5 }));
    static_assert(exactly_castable_to<f32>(f64 { 1.25 }));
    static_assert(!exactly_castable_to<f32>(f64 { -1.1 }));
    static_assert(!exactly_castable_to<f32>(f64 { 1.1 }));

    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::infinity()));
    static_assert(exactly_castable_to<f64>(-std::numeric_limits<f32>::infinity()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::quiet_NaN()));
    static_assert(exactly_castable_to<f64>(f32 { 0 }));
    static_assert(exactly_castable_to<f64>(f32 { -1.22893457089386545673245 }));
    static_assert(exactly_castable_to<f64>(f32 { 1.10984368796345876232342 }));

    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::denorm_min()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::min()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::max()));
    static_assert(exactly_castable_to<f64>(std::numeric_limits<f32>::lowest()));

    // Suppressing EDG noise: IntelliSense incorrectly flags conversions of near-zero values as non-constant, but target
    // compilers handle it fine.
#ifdef __INTELLISENSE__
    #pragma diag_suppress 28
#endif
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<f64>::denorm_min()));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<f64>::min()));
#ifdef __INTELLISENSE__
    #pragma diag_default 28
#endif
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<f64>::max()));
    static_assert(!exactly_castable_to<f32>(std::numeric_limits<f64>::lowest()));
}

} // namespace ka
