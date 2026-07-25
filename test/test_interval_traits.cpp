#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <type_traits>

#include <ka/common/fixed.hpp>
#include <ka/common/interval_traits.hpp>

#include "helpers/interval.hpp"

namespace ka
{

// Trait type aliases for testing different operator combinations

using LessOnly = NoBuiltinOperatorsIntIntervalTraits<true, false, false, size_t>;
using CmpOnly = NoBuiltinOperatorsIntIntervalTraits<false, false, true, size_t>;
using LessEqual = NoBuiltinOperatorsIntIntervalTraits<true, true, false, size_t>;
using CmpEqual = NoBuiltinOperatorsIntIntervalTraits<false, true, true, size_t>;
using AllTraits = NoBuiltinOperatorsIntIntervalTraits<true, true, true, size_t>;
using LessCmp = NoBuiltinOperatorsIntIntervalTraits<true, false, true, size_t>;
using NoneTraits = NoBuiltinOperatorsIntIntervalTraits<false, false, false, void>;
using LessOnlyNoDistance = NoBuiltinOperatorsIntIntervalTraits<true, false, false, void>;
using CmpOnlyNoDistance = NoBuiltinOperatorsIntIntervalTraits<false, false, true, void>;
using LessOnlyS32Size = NoBuiltinOperatorsIntIntervalTraits<true, false, false, s32>;

static_assert(!IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<false, false, false, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<false, false, true, void>, NoBuiltinOperatorsInt>);
static_assert(!IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<false, true, false, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<false, true, true, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<true, false, false, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<true, false, true, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<true, true, false, void>, NoBuiltinOperatorsInt>);
static_assert(IntervalValueTraitsFor<NoBuiltinOperatorsIntIntervalTraits<true, true, true, void>, NoBuiltinOperatorsInt>);

// HasIntervalValueLess concept checks

static_assert(HasIntervalValueLess<LessOnly>);
static_assert(!HasIntervalValueLess<CmpOnly>);
static_assert(HasIntervalValueLess<LessEqual>);
static_assert(!HasIntervalValueLess<CmpEqual>);
static_assert(HasIntervalValueLess<AllTraits>);
static_assert(HasIntervalValueLess<LessCmp>);
static_assert(!HasIntervalValueLess<NoneTraits>);
static_assert(HasIntervalValueLess<IntervalValueTraits<s32>>);
static_assert(HasIntervalValueLess<IntervalValueTraits<u32>>);
static_assert(HasIntervalValueLess<IntervalValueTraits<s64>>);
static_assert(HasIntervalValueLess<IntervalValueTraits<u64>>);

// HasIntervalValueEqual concept checks

static_assert(!HasIntervalValueEqual<LessOnly>);
static_assert(!HasIntervalValueEqual<CmpOnly>);
static_assert(HasIntervalValueEqual<LessEqual>);
static_assert(HasIntervalValueEqual<CmpEqual>);
static_assert(HasIntervalValueEqual<AllTraits>);
static_assert(!HasIntervalValueEqual<LessCmp>);
static_assert(!HasIntervalValueEqual<NoneTraits>);
static_assert(!HasIntervalValueEqual<IntervalValueTraits<s32>>);
static_assert(!HasIntervalValueEqual<IntervalValueTraits<u32>>);
static_assert(!HasIntervalValueEqual<IntervalValueTraits<s64>>);
static_assert(!HasIntervalValueEqual<IntervalValueTraits<u64>>);

// HasIntervalValueCmp concept checks

static_assert(!HasIntervalValueCmp<LessOnly>);
static_assert(HasIntervalValueCmp<CmpOnly>);
static_assert(!HasIntervalValueCmp<LessEqual>);
static_assert(HasIntervalValueCmp<CmpEqual>);
static_assert(HasIntervalValueCmp<AllTraits>);
static_assert(HasIntervalValueCmp<LessCmp>);
static_assert(!HasIntervalValueCmp<NoneTraits>);
static_assert(!HasIntervalValueCmp<IntervalValueTraits<s32>>);
static_assert(!HasIntervalValueCmp<IntervalValueTraits<u32>>);
static_assert(!HasIntervalValueCmp<IntervalValueTraits<s64>>);
static_assert(!HasIntervalValueCmp<IntervalValueTraits<u64>>);

static_assert(SizedIntervalValueTraitsFor<IntervalValueTraits<s32>, s32>);
static_assert(SizedIntervalValueTraitsFor<LessOnly, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<CmpOnly, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<LessEqual, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<CmpEqual, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<AllTraits, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<LessCmp, NoBuiltinOperatorsInt>);
static_assert(!SizedIntervalValueTraitsFor<LessOnlyNoDistance, NoBuiltinOperatorsInt>);
static_assert(!SizedIntervalValueTraitsFor<CmpOnlyNoDistance, NoBuiltinOperatorsInt>);
static_assert(SizedIntervalValueTraitsFor<LessOnlyS32Size, NoBuiltinOperatorsInt>);
static_assert(std::same_as<IntervalValueTraitsSizeType<LessOnly>, size_t>);
static_assert(std::same_as<IntervalValueTraitsSizeType<LessOnlyS32Size>, s32>);

static_assert(ComparableIntervalValueTraits<IntervalValueTraits<s32>>);
static_assert(ComparableIntervalValueTraits<LessOnly>);
static_assert(ComparableIntervalValueTraits<CmpOnly>);
static_assert(ComparableIntervalValueTraits<LessEqual>);
static_assert(ComparableIntervalValueTraits<CmpEqual>);
static_assert(ComparableIntervalValueTraits<AllTraits>);
static_assert(ComparableIntervalValueTraits<LessCmp>);

// IntervalValueTraitsValueType / SizeType type alias checks

static_assert(std::same_as<IntervalValueTraitsValueType<IntervalValueTraits<s32>>, s32>);
static_assert(std::same_as<IntervalValueTraitsValueType<LessOnly>, NoBuiltinOperatorsInt>);
static_assert(std::same_as<IntervalValueTraitsSizeType<IntervalValueTraits<s32>>, size_t>);
static_assert(std::same_as<IntervalValueTraitsSizeType<LessOnly>, size_t>);

// IntervalValueTraitsSizeType must not be defined for types whose distance would overflow size_t.
template <typename T>
concept HasIntervalValueTraitsSizeType = requires { typename IntervalValueTraitsSizeType<T>; };

static_assert(HasIntervalValueTraitsSizeType<IntervalValueTraits<s32>>);
static_assert(HasIntervalValueTraitsSizeType<IntervalValueTraits<u32>>);
static_assert(!HasIntervalValueTraitsSizeType<IntervalValueTraits<s64>>);
static_assert(!HasIntervalValueTraitsSizeType<IntervalValueTraits<u64>>);

// IntervalValueUtils::less

TEST(IntervalValueUtilsTest, less_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_TRUE(Utils::less(a, b));
    EXPECT_FALSE(Utils::less(b, a));
    EXPECT_FALSE(Utils::less(a, a));
    EXPECT_TRUE(Utils::less(b, c));
}

TEST(IntervalValueUtilsTest, less_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_TRUE(Utils::less(a, b));
    EXPECT_FALSE(Utils::less(b, a));
    EXPECT_FALSE(Utils::less(a, a));
    EXPECT_TRUE(Utils::less(b, c));
}

TEST(IntervalValueUtilsTest, less_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::less(-1, 0));
    EXPECT_FALSE(Utils::less(0, -1));
    EXPECT_FALSE(Utils::less(0, 0));
}

// IntervalValueUtils::less_or_equal

TEST(IntervalValueUtilsTest, less_or_equal_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_TRUE(Utils::less_or_equal(a, b));
    EXPECT_FALSE(Utils::less_or_equal(b, a));
    EXPECT_TRUE(Utils::less_or_equal(a, a));
    EXPECT_TRUE(Utils::less_or_equal(b, c));
    EXPECT_TRUE(Utils::less_or_equal(b, b));
}

TEST(IntervalValueUtilsTest, less_or_equal_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_TRUE(Utils::less_or_equal(a, b));
    EXPECT_FALSE(Utils::less_or_equal(b, a));
    EXPECT_TRUE(Utils::less_or_equal(a, a));
    EXPECT_TRUE(Utils::less_or_equal(b, c));
    EXPECT_TRUE(Utils::less_or_equal(b, b));
}

TEST(IntervalValueUtilsTest, less_or_equal_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::less_or_equal(-1, 0));
    EXPECT_FALSE(Utils::less_or_equal(0, -1));
    EXPECT_TRUE(Utils::less_or_equal(0, 0));
}

// IntervalValueUtils::equal

TEST(IntervalValueUtilsTest, equal_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 1 };

    EXPECT_TRUE(Utils::equal(a, a));
    EXPECT_FALSE(Utils::equal(a, b));
    EXPECT_FALSE(Utils::equal(b, a));
}

TEST(IntervalValueUtilsTest, equal_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 1 };

    EXPECT_TRUE(Utils::equal(a, a));
    EXPECT_FALSE(Utils::equal(a, b));
    EXPECT_FALSE(Utils::equal(b, a));
}

TEST(IntervalValueUtilsTest, equal_equal_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessEqual>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 1 };

    EXPECT_TRUE(Utils::equal(a, a));
    EXPECT_FALSE(Utils::equal(a, b));
    EXPECT_FALSE(Utils::equal(b, a));
}

TEST(IntervalValueUtilsTest, equal_cmp_equal)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpEqual>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 1 };

    EXPECT_TRUE(Utils::equal(a, a));
    EXPECT_FALSE(Utils::equal(a, b));
    EXPECT_FALSE(Utils::equal(b, a));
}

TEST(IntervalValueUtilsTest, equal_all_traits)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, AllTraits>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 1 };

    EXPECT_TRUE(Utils::equal(a, a));
    EXPECT_FALSE(Utils::equal(a, b));
    EXPECT_FALSE(Utils::equal(b, a));
}

TEST(IntervalValueUtilsTest, equal_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::equal(0, 0));
    EXPECT_FALSE(Utils::equal(0, 1));
    EXPECT_FALSE(Utils::equal(1, 0));
}

// IntervalValueUtils::greater_or_equal

TEST(IntervalValueUtilsTest, greater_or_equal_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_FALSE(Utils::greater_or_equal(a, b));
    EXPECT_TRUE(Utils::greater_or_equal(b, a));
    EXPECT_TRUE(Utils::greater_or_equal(a, a));
    EXPECT_TRUE(Utils::greater_or_equal(c, b));
    EXPECT_TRUE(Utils::greater_or_equal(b, b));
}

TEST(IntervalValueUtilsTest, greater_or_equal_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_FALSE(Utils::greater_or_equal(a, b));
    EXPECT_TRUE(Utils::greater_or_equal(b, a));
    EXPECT_TRUE(Utils::greater_or_equal(a, a));
    EXPECT_TRUE(Utils::greater_or_equal(c, b));
    EXPECT_TRUE(Utils::greater_or_equal(b, b));
}

TEST(IntervalValueUtilsTest, greater_or_equal_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_FALSE(Utils::greater_or_equal(-1, 0));
    EXPECT_TRUE(Utils::greater_or_equal(0, -1));
    EXPECT_TRUE(Utils::greater_or_equal(0, 0));
}

// IntervalValueUtils::greater

TEST(IntervalValueUtilsTest, greater_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_FALSE(Utils::greater(a, b));
    EXPECT_TRUE(Utils::greater(b, a));
    EXPECT_FALSE(Utils::greater(a, a));
    EXPECT_TRUE(Utils::greater(c, b));
}

TEST(IntervalValueUtilsTest, greater_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_TRUE(Utils::greater(b, a));
    EXPECT_FALSE(Utils::greater(a, b));
    EXPECT_FALSE(Utils::greater(a, a));
    EXPECT_TRUE(Utils::greater(c, b));
}

TEST(IntervalValueUtilsTest, greater_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_FALSE(Utils::greater(-1, 0));
    EXPECT_TRUE(Utils::greater(0, -1));
    EXPECT_FALSE(Utils::greater(0, 0));
}

// IntervalValueUtils::cmp

TEST(IntervalValueUtilsTest, cmp_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_EQ(std::strong_ordering::less, Utils::cmp(a, b));
    EXPECT_EQ(std::strong_ordering::greater, Utils::cmp(b, a));
    EXPECT_EQ(std::strong_ordering::equal, Utils::cmp(a, a));
    EXPECT_EQ(std::strong_ordering::less, Utils::cmp(b, c));
}

TEST(IntervalValueUtilsTest, cmp_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt a { -1 };
    const NoBuiltinOperatorsInt b { 0 };
    const NoBuiltinOperatorsInt c { 1 };

    EXPECT_EQ(std::strong_ordering::less, Utils::cmp(a, b));
    EXPECT_EQ(std::strong_ordering::greater, Utils::cmp(b, a));
    EXPECT_EQ(std::strong_ordering::equal, Utils::cmp(a, a));
    EXPECT_EQ(std::strong_ordering::less, Utils::cmp(b, c));
}

TEST(IntervalValueUtilsTest, cmp_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_EQ(std::strong_ordering::less, Utils::cmp(-1, 0));
    EXPECT_EQ(std::strong_ordering::greater, Utils::cmp(0, -1));
    EXPECT_EQ(std::strong_ordering::equal, Utils::cmp(0, 0));
}

// IntervalValueUtils::distance

TEST(IntervalValueUtilsTest, distance)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt a { 0 };
    const NoBuiltinOperatorsInt b { 5 };

    EXPECT_EQ(size_t { 0u }, Utils::distance(a, a));
    EXPECT_EQ(size_t { 5u }, Utils::distance(a, b));
    EXPECT_EQ(size_t { 3u }, Utils::distance(NoBuiltinOperatorsInt { 2 }, NoBuiltinOperatorsInt { 5 }));
}

TEST(IntervalValueUtilsTest, distance_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_EQ(size_t { 0u }, Utils::distance(0, 0));
    EXPECT_EQ(size_t { 5u }, Utils::distance(0, 5));
    EXPECT_EQ(size_t { 10u }, Utils::distance(-5, 5));
}

// IntervalValueUtils::value_is_valid

TEST(IntervalValueUtilsTest, value_is_valid)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt min_val = LessOnly::min();
    const NoBuiltinOperatorsInt max_val = LessOnly::max();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt below_min { exact_cast<s16>(std::numeric_limits<s16>::min() + 1) };
    const NoBuiltinOperatorsInt above_max { exact_cast<s16>(std::numeric_limits<s16>::max() - 1) };

    EXPECT_TRUE(Utils::value_is_valid(min_val));
    EXPECT_TRUE(Utils::value_is_valid(max_val));
    EXPECT_TRUE(Utils::value_is_valid(mid));
    EXPECT_TRUE(Utils::value_is_valid(below_min));
    EXPECT_TRUE(Utils::value_is_valid(above_max));
}

TEST(IntervalValueUtilsTest, value_inside_allowed_range_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::value_is_valid(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(Utils::value_is_valid(std::numeric_limits<s32>::max()));
    EXPECT_TRUE(Utils::value_is_valid(0));
}

// IntervalValueUtils::prev / next

TEST(IntervalValueUtilsTest, prev_next)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt zero { 0 };
    const NoBuiltinOperatorsInt one { 1 };
    const NoBuiltinOperatorsInt neg_one { -1 };

    EXPECT_EQ(neg_one.value, Utils::prev(zero).value);
    EXPECT_EQ(one.value, Utils::next(zero).value);
    EXPECT_EQ(zero.value, Utils::prev(one).value);
    EXPECT_EQ(zero.value, Utils::next(neg_one).value);
}

TEST(IntervalValueUtilsTest, prev_next_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_EQ(s32 { -1 }, Utils::prev(0));
    EXPECT_EQ(s32 { 1 }, Utils::next(0));
    EXPECT_EQ(s32 { 0 }, Utils::prev(1));
    EXPECT_EQ(s32 { 0 }, Utils::next(-1));
}

// IntervalValueUtils::min / max

TEST(IntervalValueUtilsTest, min_max)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::less(min_val, max_val));
    EXPECT_FALSE(Utils::less(max_val, min_val));
    EXPECT_FALSE(Utils::less(min_val, min_val));
    EXPECT_FALSE(Utils::less(max_val, max_val));
}

TEST(IntervalValueUtilsTest, min_max_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_EQ(std::numeric_limits<s32>::min(), Utils::min());
    EXPECT_EQ(std::numeric_limits<s32>::max(), Utils::max());
    EXPECT_TRUE(Utils::less(Utils::min(), Utils::max()));
}

// IntervalValueUtils::is_min

TEST(IntervalValueUtilsTest, is_min_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_min(min_val));
    EXPECT_FALSE(Utils::is_min(mid));
    EXPECT_FALSE(Utils::is_min(max_val));
}

TEST(IntervalValueUtilsTest, is_min_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_min(min_val));
    EXPECT_FALSE(Utils::is_min(mid));
    EXPECT_FALSE(Utils::is_min(max_val));
}

TEST(IntervalValueUtilsTest, is_min_equal_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessEqual>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_min(min_val));
    EXPECT_FALSE(Utils::is_min(mid));
    EXPECT_FALSE(Utils::is_min(max_val));
}

TEST(IntervalValueUtilsTest, is_min_cmp_equal)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpEqual>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_min(min_val));
    EXPECT_FALSE(Utils::is_min(mid));
    EXPECT_FALSE(Utils::is_min(max_val));
}

TEST(IntervalValueUtilsTest, is_min_all_traits)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, AllTraits>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_min(min_val));
    EXPECT_FALSE(Utils::is_min(mid));
    EXPECT_FALSE(Utils::is_min(max_val));
}

TEST(IntervalValueUtilsTest, is_min_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::is_min(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(Utils::is_min(0));
    EXPECT_FALSE(Utils::is_min(std::numeric_limits<s32>::max()));
}

// IntervalValueUtils::is_max

TEST(IntervalValueUtilsTest, is_max_less_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessOnly>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_max(max_val));
    EXPECT_FALSE(Utils::is_max(mid));
    EXPECT_FALSE(Utils::is_max(min_val));
}

TEST(IntervalValueUtilsTest, is_max_cmp_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpOnly>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_max(max_val));
    EXPECT_FALSE(Utils::is_max(mid));
    EXPECT_FALSE(Utils::is_max(min_val));
}

TEST(IntervalValueUtilsTest, is_max_equal_only)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, LessEqual>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_max(max_val));
    EXPECT_FALSE(Utils::is_max(mid));
    EXPECT_FALSE(Utils::is_max(min_val));
}

TEST(IntervalValueUtilsTest, is_max_cmp_equal)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, CmpEqual>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_max(max_val));
    EXPECT_FALSE(Utils::is_max(mid));
    EXPECT_FALSE(Utils::is_max(min_val));
}

TEST(IntervalValueUtilsTest, is_max_all_traits)
{
    using Utils = IntervalValueUtils<NoBuiltinOperatorsInt, AllTraits>;
    const NoBuiltinOperatorsInt min_val = Utils::min();
    const NoBuiltinOperatorsInt mid { 0 };
    const NoBuiltinOperatorsInt max_val = Utils::max();

    EXPECT_TRUE(Utils::is_max(max_val));
    EXPECT_FALSE(Utils::is_max(mid));
    EXPECT_FALSE(Utils::is_max(min_val));
}

TEST(IntervalValueUtilsTest, is_max_default_integral)
{
    using Utils = IntervalValueUtils<s32, IntervalValueTraits<s32>>;
    EXPECT_TRUE(Utils::is_max(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(Utils::is_max(0));
    EXPECT_FALSE(Utils::is_max(std::numeric_limits<s32>::min()));
}

} // namespace ka
