#include <gtest/gtest.h>

#include <concepts>
#include <limits>
#include <tuple>
#include <type_traits>

#include <ka/common/closed_interval.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>
#include <ka/common/interval_traits.hpp>

#include "helpers/interval.hpp"

namespace ka
{

using std::get;

// Concept checks

static_assert(Hashable<ClosedInterval<s32>>);

// Trait type aliases for testing different operator combinations

using LessOnly = NoBuiltinOperatorsIntIntervalTraits<true, false, false, size_t>;
using CmpOnly = NoBuiltinOperatorsIntIntervalTraits<false, false, true, size_t>;
using LessEqual = NoBuiltinOperatorsIntIntervalTraits<true, true, false, size_t>;
using CmpEqual = NoBuiltinOperatorsIntIntervalTraits<false, true, true, size_t>;
using AllTraits = NoBuiltinOperatorsIntIntervalTraits<true, true, true, size_t>;
using LessCmp = NoBuiltinOperatorsIntIntervalTraits<true, false, true, size_t>;

// Construction and basic accessors

TEST(ClosedIntervalTest, construct_and_access_s32)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(1, interval.first());
    EXPECT_EQ(5, interval.last());
}

TEST(ClosedIntervalTest, construct_equal_endpoints_s32)
{
    const ClosedInterval<s32> interval { 3, 3 };
    EXPECT_EQ(3, interval.first());
    EXPECT_EQ(3, interval.last());
}

TEST(ClosedIntervalTest, construct_min_max_s32)
{
    const ClosedInterval<s32> interval { std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max() };
    EXPECT_EQ(std::numeric_limits<s32>::min(), interval.first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), interval.last());
}

TEST(ClosedIntervalTest, construct_and_access_u32)
{
    const ClosedInterval<u32> interval { 0, 100 };
    EXPECT_EQ(u32 { 0 }, interval.first());
    EXPECT_EQ(u32 { 100 }, interval.last());
}

// Equality

TEST(ClosedIntervalTest, equal_same_values)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 1, 5 };
    EXPECT_TRUE(a == b);
}

TEST(ClosedIntervalTest, not_equal_different_first)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 2, 5 };
    EXPECT_FALSE(a == b);
}

TEST(ClosedIntervalTest, not_equal_different_last)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 1, 6 };
    EXPECT_FALSE(a == b);
}

TEST(ClosedIntervalTest, equal_single_value)
{
    const ClosedInterval<s32> a { 42, 42 };
    const ClosedInterval<s32> b { 42, 42 };
    EXPECT_TRUE(a == b);
}

// Copy and move semantics

TEST(ClosedIntervalTest, copy_constructible)
{
    const ClosedInterval<s32> interval { 1, 5 };
    auto copy = interval;
    EXPECT_EQ(interval.first(), copy.first());
    EXPECT_EQ(interval.last(), copy.last());
}

TEST(ClosedIntervalTest, copy_assignable)
{
    const ClosedInterval<s32> interval { 1, 5 };
    ClosedInterval<s32> other { 0, 0 };
    other = interval;
    EXPECT_EQ(interval.first(), other.first());
    EXPECT_EQ(interval.last(), other.last());
}

TEST(ClosedIntervalTest, move_constructible)
{
    ClosedInterval<s32> interval { 1, 5 };
    auto moved = std::move(interval);
    EXPECT_EQ(1, moved.first());
    EXPECT_EQ(5, moved.last());
}

TEST(ClosedIntervalTest, move_assignable)
{
    ClosedInterval<s32> interval { 1, 5 };
    ClosedInterval<s32> other { 0, 0 };
    other = std::move(interval);
    EXPECT_EQ(1, other.first());
    EXPECT_EQ(5, other.last());
}

// Custom traits combinations

TEST(ClosedIntervalTest, less_only_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, cmp_only_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, less_equal_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessEqual>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, cmp_equal_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, CmpEqual>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, all_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, AllTraits>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, less_cmp_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessCmp>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, interval.first().value);
    EXPECT_EQ(1, interval.last().value);
}

TEST(ClosedIntervalTest, custom_traits_equal_endpoints)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt val { 42 };
    const I interval { val, val };
    EXPECT_EQ(42, interval.first().value);
    EXPECT_EQ(42, interval.last().value);
}

// Structured bindings

TEST(ClosedIntervalTest, structured_bindings)
{
    const ClosedInterval<s32> interval { 1, 5 };
    const auto & [first, last] = interval;
    EXPECT_EQ(1, first);
    EXPECT_EQ(5, last);
}

TEST(ClosedIntervalTest, structured_bindings_single_value)
{
    const ClosedInterval<s32> interval { 42, 42 };
    const auto [first, last] = interval;
    EXPECT_EQ(42, first);
    EXPECT_EQ(42, last);
}

TEST(ClosedIntervalTest, structured_bindings_custom_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    const auto & [first, last] = interval;
    EXPECT_EQ(-1, first.value);
    EXPECT_EQ(1, last.value);
}

// std::tuple_size / std::tuple_element / std::get support

TEST(ClosedIntervalTupleSupport, tuple_size)
{
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<ClosedInterval<s32>>);
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<ClosedInterval<u32>>);
}

TEST(ClosedIntervalTupleSupport, tuple_size_custom_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessOnly>;
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<I>);
}

TEST(ClosedIntervalTupleSupport, get_first)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(1, std::get<0>(interval));
}

TEST(ClosedIntervalTupleSupport, get_last)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(5, std::get<1>(interval));
}

TEST(ClosedIntervalTupleSupport, get_custom_traits)
{
    using I = ClosedInterval<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 1 };
    const I interval { a, b };
    EXPECT_EQ(-1, std::get<0>(interval).value);
    EXPECT_EQ(1, std::get<1>(interval).value);
}

// Hash support

TEST(ClosedIntervalHashTest, hash_equal_intervals)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 1, 5 };
    Hasher hasher_a;
    hasher_a.update(a);
    Hasher hasher_b;
    hasher_b.update(b);
    EXPECT_EQ(hasher_a.digest(), hasher_b.digest());
}

TEST(ClosedIntervalHashTest, hash_different_first)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 2, 5 };
    Hasher hasher_a;
    hasher_a.update(a);
    Hasher hasher_b;
    hasher_b.update(b);
    EXPECT_NE(hasher_a.digest(), hasher_b.digest());
}

TEST(ClosedIntervalHashTest, hash_different_last)
{
    const ClosedInterval<s32> a { 1, 5 };
    const ClosedInterval<s32> b { 1, 6 };
    Hasher hasher_a;
    hasher_a.update(a);
    Hasher hasher_b;
    hasher_b.update(b);
    EXPECT_NE(hasher_a.digest(), hasher_b.digest());
}

TEST(ClosedIntervalHashTest, hash_struct)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(Hash {}(interval), Hash {}(interval));
}

TEST(ClosedIntervalHashTest, hash_single_value)
{
    const ClosedInterval<s32> a { 42, 42 };
    const ClosedInterval<s32> b { 42, 42 };
    EXPECT_EQ(Hash {}(a), Hash {}(b));
}

// constexpr support

TEST(ClosedIntervalTest, constexpr_construct)
{
    constexpr ClosedInterval<s32> interval { 1, 5 };
    static_assert(interval.first() == 1);
    static_assert(interval.last() == 5);
}

TEST(ClosedIntervalTest, constexpr_equal)
{
    constexpr ClosedInterval<s32> a { 1, 5 };
    constexpr ClosedInterval<s32> b { 1, 5 };
    constexpr ClosedInterval<s32> c { 2, 5 };
    static_assert(a == b);
    static_assert(!(a == c));
}

} // namespace ka
