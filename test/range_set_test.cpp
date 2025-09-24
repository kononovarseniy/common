#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>

#include <ka/common/fixed.hpp>
#include <ka/common/range_set.hpp>

namespace ka
{

TEST(RangeSetTest, size)
{
    RangeSet<s32> set;
    EXPECT_EQ(size_t { 0u }, set.size());

    EXPECT_EQ(size_t { 1u }, RangeSet<s32>::make_single_value(0).size());
    EXPECT_EQ(size_t { 3u }, RangeSet<s32>::make_interval(2, true, 5, false).size());
    EXPECT_EQ(size_t { 11u }, RangeSet<s32>::make_greater_equal(RangeSetValueTraits<s32>::max() - 10).size());
}

TEST(RangeSetTest, default_constructor)
{
    const RangeSet<s32> set;
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.all());
}

TEST(RangeSetTest, make_empty)
{
    const auto set = RangeSet<s32>::make_empty();
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_all)
{
    const auto set = RangeSet<s32>::make_all();
    EXPECT_FALSE(set.empty());
    EXPECT_TRUE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less)
{
    const auto set = RangeSet<s32>::make_less(0);
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less_min)
{
    const auto set = RangeSet<s32>::make_less(std::numeric_limits<s32>::min());
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less_max)
{
    const auto set = RangeSet<s32>::make_less(std::numeric_limits<s32>::max());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less_equal)
{
    const auto set = RangeSet<s32>::make_less_equal(0);
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less_equal_min)
{
    const auto set = RangeSet<s32>::make_less_equal(std::numeric_limits<s32>::min());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_less_equal_max)
{
    const auto set = RangeSet<s32>::make_less_equal(std::numeric_limits<s32>::max());
    EXPECT_FALSE(set.empty());
    EXPECT_TRUE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_single_value)
{
    const auto set = RangeSet<s32>::make_single_value(0);
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_single_value_min)
{
    const auto set = RangeSet<s32>::make_single_value(std::numeric_limits<s32>::min());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_single_value_max)
{
    const auto set = RangeSet<s32>::make_single_value(std::numeric_limits<s32>::max());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater_equal)
{
    const auto set = RangeSet<s32>::make_greater_equal(0);
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater_equal_min)
{
    const auto set = RangeSet<s32>::make_greater_equal(std::numeric_limits<s32>::min());
    EXPECT_FALSE(set.empty());
    EXPECT_TRUE(set.all());

    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater_equal_max)
{
    const auto set = RangeSet<s32>::make_greater_equal(std::numeric_limits<s32>::max());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater)
{
    const auto set = RangeSet<s32>::make_greater(0);
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater_min)
{
    const auto set = RangeSet<s32>::make_greater(std::numeric_limits<s32>::min());
    EXPECT_FALSE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(set.contains(-1));
    EXPECT_TRUE(set.contains(0));
    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_greater_max)
{
    const auto set = RangeSet<s32>::make_greater(std::numeric_limits<s32>::max());
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.all());

    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(set.contains(-1));
    EXPECT_FALSE(set.contains(0));
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(std::numeric_limits<s32>::max()));
}

TEST(RangeSetTest, make_interval)
{
    {
        const auto set = RangeSet<s32>::make_interval(-37, true, 42, true);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater_equal(-37), RangeSet<s32>::make_less_equal(42)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(-37, true, 42, false);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater_equal(-37), RangeSet<s32>::make_less(42)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(-37, false, 42, true);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater(-37), RangeSet<s32>::make_less_equal(42)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(-37, false, 42, false);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater(-37), RangeSet<s32>::make_less(42)));
    }
}

TEST(RangeSetTest, make_interval_min_max)
{
    constexpr auto min = std::numeric_limits<s32>::min();
    constexpr auto max = std::numeric_limits<s32>::max();
    {
        const auto set = RangeSet<s32>::make_interval(min, true, max, true);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater_equal(min), RangeSet<s32>::make_less_equal(max)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, true, max, false);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater_equal(min), RangeSet<s32>::make_less(max)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, false, max, true);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater(min), RangeSet<s32>::make_less_equal(max)));
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, false, max, false);
        EXPECT_EQ(set, make_intersection(RangeSet<s32>::make_greater(min), RangeSet<s32>::make_less(max)));
    }
}

TEST(RangeSetTest, make_interval_matching_endpoints)
{
    {
        const auto set = RangeSet<s32>::make_interval(42, true, 42, true);
        EXPECT_EQ(set, RangeSet<s32>::make_single_value(42));
    }
    {
        const auto set = RangeSet<s32>::make_interval(42, true, 42, false);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(42, false, 42, true);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(42, false, 42, false);
        EXPECT_TRUE(set.empty());
    }
}

TEST(RangeSetTest, make_interval_matching_endpoints_min)
{
    constexpr auto min = std::numeric_limits<s32>::min();
    {
        const auto set = RangeSet<s32>::make_interval(min, true, min, true);
        EXPECT_EQ(set, RangeSet<s32>::make_single_value(min));
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, true, min, false);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, false, min, true);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(min, false, min, false);
        EXPECT_TRUE(set.empty());
    }
}

TEST(RangeSetTest, make_interval_matching_endpoints_max)
{
    constexpr auto max = std::numeric_limits<s32>::min();
    {
        const auto set = RangeSet<s32>::make_interval(max, true, max, true);
        EXPECT_EQ(set, RangeSet<s32>::make_single_value(max));
    }
    {
        const auto set = RangeSet<s32>::make_interval(max, true, max, false);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(max, false, max, true);
        EXPECT_TRUE(set.empty());
    }
    {
        const auto set = RangeSet<s32>::make_interval(max, false, max, false);
        EXPECT_TRUE(set.empty());
    }
}

TEST(RangeSetTest, make_interval_neighbor_exclusive_endpoints)
{
    const auto set = RangeSet<s32>::make_interval(42, false, 43, false);
    EXPECT_TRUE(set.empty());
}

TEST(RangeSetTest, complement_empty)
{
    const auto set = RangeSet<s32>::make_empty();
    const auto complement = set.complement();
    EXPECT_TRUE(complement.all());
    EXPECT_EQ(set, complement.complement());
}

TEST(RangeSetTest, complement_all)
{
    const auto set = RangeSet<s32>::make_all();
    const auto complement = set.complement();
    EXPECT_TRUE(complement.empty());
    EXPECT_EQ(set, complement.complement());
}

TEST(RangeSetTest, complement_single_value)
{
    const auto set = RangeSet<s32>::make_single_value(42);
    const auto complement = set.complement();

    EXPECT_TRUE(complement.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(complement.contains(41));
    EXPECT_FALSE(complement.contains(42));
    EXPECT_TRUE(complement.contains(43));
    EXPECT_TRUE(complement.contains(std::numeric_limits<s32>::max()));
    EXPECT_EQ(set, complement.complement());
}

TEST(RangeSetTest, complement_single_value_min)
{
    const auto set = RangeSet<s32>::make_single_value(std::numeric_limits<s32>::min());
    const auto complement = set.complement();

    EXPECT_FALSE(complement.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(complement.contains(std::numeric_limits<s32>::min() + 1));
    EXPECT_TRUE(complement.contains(std::numeric_limits<s32>::max()));
    EXPECT_EQ(set, complement.complement());
}

TEST(RangeSetTest, make_union_with_empty)
{
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_empty();
        EXPECT_EQ(lhs, make_union(lhs, rhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_empty();
        const auto rhs = RangeSet<s32>::make_empty();
        EXPECT_EQ(lhs, make_union(lhs, rhs));
    }
}

TEST(RangeSetTest, make_union_with_all)
{
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_all();
        EXPECT_EQ(rhs, make_union(lhs, rhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_all();
        const auto rhs = RangeSet<s32>::make_all();
        EXPECT_EQ(rhs, make_union(lhs, rhs));
    }
}

TEST(RangeSetTest, make_union)
{
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, false);
        const auto rhs = RangeSet<s32>::make_interval(30, false, 42, false);
        const auto expected = RangeSet<s32>::make_interval(-73, true, 42, false);
        EXPECT_EQ(expected, make_union(lhs, rhs));
        EXPECT_EQ(expected, make_union(rhs, lhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_interval(37, true, 42, false);
        const auto expected = RangeSet<s32>::make_interval(-73, true, 42, false);
        EXPECT_EQ(expected, make_union(lhs, rhs));
        EXPECT_EQ(expected, make_union(rhs, lhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, false);
        const auto rhs = RangeSet<s32>::make_interval(37, true, 42, false);
        const auto expected = RangeSet<s32>::make_interval(-73, true, 42, false);
        EXPECT_EQ(expected, make_union(lhs, rhs));
        EXPECT_EQ(expected, make_union(rhs, lhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-5, true, -1, false);
        const auto rhs = RangeSet<s32>::make_interval(1, false, 5, true);

        const auto result = make_union(lhs, rhs);
        EXPECT_FALSE(result.contains(-6));
        EXPECT_TRUE(result.contains(-5));
        EXPECT_TRUE(result.contains(-4));
        EXPECT_TRUE(result.contains(-3));
        EXPECT_TRUE(result.contains(-2));
        EXPECT_FALSE(result.contains(-1));
        EXPECT_FALSE(result.contains(0));
        EXPECT_FALSE(result.contains(1));
        EXPECT_TRUE(result.contains(2));
        EXPECT_TRUE(result.contains(3));
        EXPECT_TRUE(result.contains(4));
        EXPECT_TRUE(result.contains(5));
        EXPECT_FALSE(result.contains(6));
        EXPECT_EQ(2, result.intervals().size());
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-5, true, -1, false);
        const auto rhs = RangeSet<s32>::make_interval(1, false, 5, true);
        const auto mhs = RangeSet<s32>::make_interval(-1, true, 1, true);
        const auto expected = RangeSet<s32>::make_interval(-5, true, 5, true);
        EXPECT_EQ(expected, make_union(mhs, make_union(lhs, rhs)));
        EXPECT_EQ(expected, make_union(make_union(lhs, rhs), mhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_less_equal(0);
        const auto rhs = RangeSet<s32>::make_greater(0);
        EXPECT_TRUE(make_union(lhs, rhs).all());
    }
}

TEST(RangeSetTest, make_intersection)
{
    {
        const auto lhs = RangeSet<s32>::make_less_equal(42);
        const auto rhs = RangeSet<s32>::make_greater(42);
        EXPECT_TRUE(make_intersection(lhs, rhs).empty());
    }
    {
        const auto lhs = RangeSet<s32>::make_less_equal(42);
        const auto rhs = RangeSet<s32>::make_greater_equal(42);
        const auto expected = RangeSet<s32>::make_single_value(42);
        EXPECT_EQ(expected, make_intersection(lhs, rhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_interval(-42, true, 42, true);
        const auto expected = RangeSet<s32>::make_interval(-42, true, 37, true);
        EXPECT_EQ(expected, make_intersection(lhs, rhs));
    }
}

TEST(RangeSetTest, make_difference)
{
    {
        const auto lhs = RangeSet<s32>::make_less(42);
        const auto rhs = RangeSet<s32>::make_less(42);
        EXPECT_TRUE(make_difference(lhs, rhs).empty());
    }
    {
        const auto lhs = RangeSet<s32>::make_less_equal(42);
        const auto rhs = RangeSet<s32>::make_greater_equal(42);
        const auto expected = RangeSet<s32>::make_less(42);
        EXPECT_EQ(expected, make_difference(lhs, rhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_interval(-42, true, 42, true);
        const auto expected = RangeSet<s32>::make_interval(-73, true, -42, false);
        EXPECT_EQ(expected, make_difference(lhs, rhs));
    }
}

TEST(RangeSetTest, make_symetric_difference)
{
    {
        const auto lhs = RangeSet<s32>::make_less(42);
        const auto rhs = RangeSet<s32>::make_less(42);
        EXPECT_TRUE(make_symetric_difference(lhs, rhs).empty());
    }
    {
        const auto lhs = RangeSet<s32>::make_less_equal(42);
        const auto rhs = RangeSet<s32>::make_greater_equal(42);
        const auto expected = RangeSet<s32>::make_single_value(42).complement();
        EXPECT_EQ(expected, make_symetric_difference(lhs, rhs));
    }
    {
        const auto lhs = RangeSet<s32>::make_interval(-73, true, 37, true);
        const auto rhs = RangeSet<s32>::make_interval(-42, true, 42, true);
        const auto expected = make_union(
            RangeSet<s32>::make_interval(-73, true, -42, false),
            RangeSet<s32>::make_interval(37, false, 42, true));
        EXPECT_EQ(expected, make_symetric_difference(lhs, rhs));
    }
}

TEST(RangeSetTest, min_value)
{
    EXPECT_EQ(std::numeric_limits<s32>::min(), RangeSet<s32>::make_single_value(std::numeric_limits<s32>::min()).min());
    EXPECT_EQ(42, RangeSet<s32>::make_greater_equal(42).min());
    EXPECT_EQ(43, RangeSet<s32>::make_greater(42).min());
}

TEST(RangeSetTest, max_value)
{
    EXPECT_EQ(std::numeric_limits<s32>::min(), RangeSet<s32>::make_single_value(std::numeric_limits<s32>::min()).max());
    EXPECT_EQ(std::numeric_limits<s32>::max(), RangeSet<s32>::make_greater_equal(42).max());
    EXPECT_EQ(73, RangeSet<s32>::make_interval(-73, true, 73, true).max());
    EXPECT_EQ(72, RangeSet<s32>::make_interval(-73, true, 73, false).max());
}

static_assert(std::ranges::bidirectional_range<RangeSet<s32>>);
static_assert(std::ranges::random_access_range<RangeSet<s32>::Intervals>);

TEST(RangeSetTest, elements_iterators)
{
    auto set = RangeSet<s32>::make_interval(-2, true, 2, true);
    std::vector<s32> expected { -2, -1, 0, 1, 2 };
    std::vector<s32> result;
    std::ranges::copy(set, std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::ranges::reverse(expected);
    result.clear();
    std::ranges::copy(set | std::views::reverse, std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::vector<std::pair<s32, s32>> expected_intervals { { -2, 2 } };
    std::vector<std::pair<s32, s32>> result_intervals;
    std::ranges::copy(set.intervals(), std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);

    std::ranges::reverse(expected_intervals);
    result_intervals.clear();
    std::ranges::copy(set.intervals() | std::views::reverse, std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);
}

TEST(RangeSetTest, elements_iterators_empty)
{
    auto set = RangeSet<s32>::make_empty();
    std::vector<s32> expected {};
    std::vector<s32> result;
    std::ranges::copy(set.begin(), set.end(), std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::ranges::reverse(expected);
    result.clear();
    std::ranges::copy(set | std::views::reverse, std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::vector<std::pair<s32, s32>> expected_intervals {};
    std::vector<std::pair<s32, s32>> result_intervals;
    std::ranges::copy(set.intervals(), std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);

    std::ranges::reverse(expected_intervals);
    result_intervals.clear();
    std::ranges::copy(set.intervals() | std::views::reverse, std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);
}

TEST(RangeSetTest, elements_iterators_unbounded)
{
    constexpr auto min = std::numeric_limits<s32>::min();
    constexpr auto max = std::numeric_limits<s32>::max();
    auto set = make_union(
        make_union(RangeSet<s32>::make_less_equal(min + 3), RangeSet<s32>::make_greater_equal(max - 3)),
        make_union(RangeSet<s32>::make_interval(-2, true, 2, true), RangeSet<s32>::make_single_value(42)));
    std::vector<s32> expected { min, min + 1, min + 2, min + 3, -2, -1, 0, 1, 2, 42, max - 3, max - 2, max - 1, max };
    std::vector<s32> result;
    std::ranges::copy(set.begin(), set.end(), std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::ranges::reverse(expected);
    result.clear();
    std::ranges::copy(set | std::views::reverse, std::back_inserter(result));
    EXPECT_EQ(expected, result);

    std::vector<std::pair<s32, s32>> expected_intervals { { min, min + 3 }, { -2, 2 }, { 42, 42 }, { max - 3, max } };
    std::vector<std::pair<s32, s32>> result_intervals;
    std::ranges::copy(set.intervals(), std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);

    std::ranges::reverse(expected_intervals);
    result_intervals.clear();
    std::ranges::copy(set.intervals() | std::views::reverse, std::back_inserter(result_intervals));
    EXPECT_EQ(expected_intervals, result_intervals);
}

TEST(RangeSetTest, intervals_iterator_random_access)
{
    const auto set = make_union(
        RangeSet<s32>::make_interval(-10, true, -5, true),
        make_union(RangeSet<s32>::make_interval(0, true, 5, true), RangeSet<s32>::make_interval(10, true, 15, true)));

    const auto intervals = set.intervals();
    EXPECT_EQ(3u, intervals.size());

    // Testing operators +, -, +=, -=.
    auto it = intervals.begin();

    // operator+.
    auto it_plus_2 = it + 2;
    EXPECT_EQ(intervals.begin()[2], *it_plus_2);

    // operator+ with reverse order.
    auto it_plus_2_alt = 2 + it;
    EXPECT_EQ(it_plus_2, it_plus_2_alt);

    // operator- with integer.
    auto it_minus_1 = it_plus_2 - 1;
    EXPECT_EQ(intervals.begin()[1], *it_minus_1);

    // operator- between iterators.
    auto diff = it_plus_2 - it;
    EXPECT_EQ(2, diff);

    // operator+=.
    it += 2;
    EXPECT_EQ(intervals.begin()[2], *it);

    // operator-=.
    it -= 1;
    EXPECT_EQ(intervals.begin()[1], *it);

    // Testing comparison operators.
    const auto begin = intervals.begin();
    const auto mid = begin + 1;
    const auto end = intervals.end();

    // operator<.
    EXPECT_TRUE(begin < mid);
    EXPECT_FALSE(mid < begin);
    EXPECT_FALSE(begin < begin);

    // operator>.
    EXPECT_TRUE(mid > begin);
    EXPECT_FALSE(begin > mid);
    EXPECT_FALSE(begin > begin);

    // operator<=.
    EXPECT_TRUE(begin <= mid);
    EXPECT_TRUE(begin <= begin);
    EXPECT_FALSE(mid <= begin);

    // operator>=.
    EXPECT_TRUE(mid >= begin);
    EXPECT_TRUE(begin >= begin);
    EXPECT_FALSE(begin >= mid);

    // Testing operator[].
    EXPECT_EQ(intervals.begin()[0], *begin);
    EXPECT_EQ(intervals.begin()[1], *(begin + 1));
    EXPECT_EQ(intervals.begin()[2], *(begin + 2));

    // Checking interval values correctness via operator[].
    const auto & first_interval = begin[0];
    EXPECT_EQ(-10, first_interval.first);
    EXPECT_EQ(-5, first_interval.second);

    const auto & second_interval = begin[1];
    EXPECT_EQ(0, second_interval.first);
    EXPECT_EQ(5, second_interval.second);

    const auto & third_interval = begin[2];
    EXPECT_EQ(10, third_interval.first);
    EXPECT_EQ(15, third_interval.second);

    // Testing decrement
    auto end_minus_1 = end - 1;
    EXPECT_EQ(third_interval, *end_minus_1);

    // Testing prefix and postfix decrement.
    auto dec_it = end_minus_1;
    --dec_it;
    EXPECT_EQ(second_interval, *dec_it);

    dec_it = end_minus_1;
    auto old_it = dec_it--;
    EXPECT_EQ(third_interval, *old_it);
    EXPECT_EQ(second_interval, *dec_it);
}

TEST(RangeSetTest, intervals_iterator_random_access_empty)
{
    const auto set = RangeSet<s32>::make_empty();
    const auto intervals = set.intervals();

    EXPECT_EQ(0u, intervals.size());
    EXPECT_EQ(intervals.begin(), intervals.end());

    // Checking that arithmetic operations with empty set are safe.
    auto it = intervals.begin();
    auto it_plus_0 = it + 0;
    EXPECT_EQ(it, it_plus_0);

    auto it_minus_0 = it - 0;
    EXPECT_EQ(it, it_minus_0);
}

TEST(RangeSetTest, intervals_iterator_random_access_single)
{
    const auto set = RangeSet<s32>::make_interval(-5, true, 5, true);
    const auto intervals = set.intervals();

    EXPECT_EQ(1u, intervals.size());

    auto begin = intervals.begin();
    auto end = intervals.end();

    // Checking difference between begin and end.
    EXPECT_EQ(1, end - begin);
    EXPECT_EQ(-1, begin - end);

    // Testing operator[] with single element.
    const auto & interval = begin[0];
    EXPECT_EQ(-5, interval.first);
    EXPECT_EQ(5, interval.second);

    // Testing comparison.
    EXPECT_TRUE(begin < end);
    EXPECT_FALSE(end < begin);
    EXPECT_TRUE(begin <= end);
    EXPECT_FALSE(end <= begin);
    EXPECT_TRUE(end > begin);
    EXPECT_FALSE(begin > end);
    EXPECT_TRUE(end >= begin);
    EXPECT_FALSE(begin >= end);
}

TEST(RangeSetTest, no_builtin_operators_type)
{
    struct NoBuiltinOperatorsInt final
    {
        s32 value;
    };

    // Traits specialization for type without built-in operators.
    struct RangeSetValueTraits final
    {
        [[nodiscard]] constexpr static NoBuiltinOperatorsInt min() noexcept
        {
            return { std::numeric_limits<s32>::min() };
        }

        [[nodiscard]] constexpr static NoBuiltinOperatorsInt max() noexcept
        {
            return { std::numeric_limits<s32>::max() };
        }

        [[nodiscard]] constexpr static NoBuiltinOperatorsInt prev(const NoBuiltinOperatorsInt & value) noexcept
        {
            AR_PRE(less(min(), value));
            return { value.value - 1 };
        }

        [[nodiscard]] constexpr static NoBuiltinOperatorsInt next(const NoBuiltinOperatorsInt & value) noexcept
        {
            AR_PRE(less(value, max()));
            return { value.value + 1 };
        }

        [[nodiscard]] constexpr static bool less(
            const NoBuiltinOperatorsInt & lhs,
            const NoBuiltinOperatorsInt & rhs) noexcept
        {
            return lhs.value < rhs.value;
        }

        [[nodiscard]] constexpr static size_t distance(
            const NoBuiltinOperatorsInt & lhs,
            const NoBuiltinOperatorsInt & rhs) noexcept
        {
            AR_PRE(!less(rhs, lhs));
            return rhs.value - lhs.value;
        }
    };

    using TestRangeSet = RangeSet<NoBuiltinOperatorsInt, RangeSetValueTraits>;

    // Testing compilation of all methods.
    TestRangeSet empty_set;
    EXPECT_TRUE(empty_set.empty());
    EXPECT_FALSE(empty_set.all());

    const NoBuiltinOperatorsInt zero { 0 };
    const NoBuiltinOperatorsInt five { 5 };
    const NoBuiltinOperatorsInt ten { 10 };

    // Static factory methods.
    auto all_set = TestRangeSet::make_all();
    auto empty_set2 = TestRangeSet::make_empty();
    auto single_set = TestRangeSet::make_single_value(zero);
    auto ge_set = TestRangeSet::make_greater_equal(five);
    auto g_set = TestRangeSet::make_greater(five);
    auto le_set = TestRangeSet::make_less_equal(five);
    auto l_set = TestRangeSet::make_less(five);
    auto interval_set = TestRangeSet::make_interval(zero, true, ten, false);

    // Access methods.
    EXPECT_FALSE(all_set.empty());
    EXPECT_TRUE(all_set.all());
    EXPECT_FALSE(single_set.contains(five));
    EXPECT_TRUE(single_set.contains(zero));

    if (!single_set.empty())
    {
        auto min_val = single_set.min();
        auto max_val = single_set.max();
        static_cast<void>(min_val);
        static_cast<void>(max_val);
    }

    // Element iterators.
    for (auto it = single_set.begin(); it != single_set.end(); ++it)
    {
        auto value = *it;
        static_cast<void>(value);
    }

    // Interval iterators.
    auto intervals = single_set.intervals();
    EXPECT_LE(intervals.size(), 1u);

    for (auto it = intervals.begin(); it != intervals.end(); ++it)
    {
        auto interval = *it;
        static_cast<void>(interval);
    }

    // Boolean operations.
    auto union_set = make_union(single_set, ge_set);
    auto intersection_set = make_intersection(single_set, ge_set);
    auto difference_set = make_difference(single_set, ge_set);
    auto sym_diff_set = make_symetric_difference(single_set, ge_set);

    // Complement.
    auto complement_set = single_set.complement();

    // Size.
    auto size = single_set.size();
    static_cast<void>(size);

    // Comparison operator.
    bool equal = (single_set == single_set);
    bool not_equal = (single_set == empty_set);
    static_cast<void>(equal);
    static_cast<void>(not_equal);

    // Testing range-based for loop.
    for (const auto & element : single_set)
    {
        static_cast<void>(element);
    }

    // Testing reverse iteration.
    for (auto it = single_set.end(); it != single_set.begin();)
    {
        --it;
        auto value = *it;
        static_cast<void>(value);
    }

    // Testing random_access for intervals.
    if (intervals.size() > 0)
    {
        auto it = intervals.begin();
        auto it2 = it + 1;
        auto diff = it2 - it;
        static_cast<void>(diff);
        auto interval = it[0];
        static_cast<void>(interval);
    }

    // Everything should compile without errors.
    SUCCEED();
}

} // namespace ka
