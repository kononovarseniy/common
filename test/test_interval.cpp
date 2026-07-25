#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <optional>
#include <type_traits>

#include <ka/common/closed_interval.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/hash.hpp>
#include <ka/common/interval.hpp>
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

// RingInterval default construction and basic operations

TEST(RingIntervalTest, default_empty)
{
    const RingInterval<s32> interval;
    EXPECT_TRUE(interval.empty());
}

TEST(RingIntervalTest, default_full)
{
    const RingInterval<s32> interval;
    EXPECT_FALSE(interval.full());
}

TEST(RingIntervalTest, default_continuous)
{
    const RingInterval<s32> interval;
    EXPECT_TRUE(interval.continuous());
}

TEST(RingIntervalTest, full_is_continuous)
{
    const auto interval = RingInterval<s32>::make_full();
    EXPECT_TRUE(interval.continuous());
}

TEST(RingIntervalTest, greater_or_equal_is_continuous)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(42);
    EXPECT_TRUE(interval.continuous());
}

TEST(RingIntervalTest, default_contains)
{
    const RingInterval<s32> interval;
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(-1));
    EXPECT_FALSE(interval.contains(0));
    EXPECT_FALSE(interval.contains(1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
}

TEST(RingIntervalTest, default_size)
{
    const RingInterval<s32> interval;
    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(RingIntervalTest, copy_constructible)
{
    const RingInterval<s32> interval;
    auto copy = interval;
    EXPECT_EQ(interval.empty(), copy.empty());
}

TEST(RingIntervalTest, copy_assignable)
{
    const RingInterval<s32> interval;
    RingInterval<s32> other;
    other = interval;
    EXPECT_TRUE(other.empty());
}

TEST(RingIntervalTest, move_constructible)
{
    RingInterval<s32> interval;
    auto moved = std::move(interval);
    EXPECT_TRUE(moved.empty());
}

TEST(RingIntervalTest, move_assignable)
{
    RingInterval<s32> interval;
    RingInterval<s32> other;
    other = std::move(interval);
    EXPECT_TRUE(other.empty());
}

// RingInterval make functions

TEST(RingIntervalTest, make_empty)
{
    const auto interval = RingInterval<s32>::make_empty();
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());
    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(RingIntervalTest, make_full)
{
    const auto interval = RingInterval<s32>::make_full();
    EXPECT_TRUE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.continuous());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_TRUE(interval.contains(0));
}

// RingInterval::make_half_open

TEST(RingIntervalTest, make_half_open_normal)
{
    const auto interval = RingInterval<s32>::make_half_open(5, 10);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(9));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(10));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(5, ci.begin()->first());
    EXPECT_EQ(9, ci.begin()->last());
}

TEST(RingIntervalTest, make_half_open_first_equals_last)
{
    const auto interval = RingInterval<s32>::make_half_open(5, 5);
    EXPECT_TRUE(interval.empty());
}

TEST(RingIntervalTest, make_half_open_min_to_max)
{
    const auto interval = RingInterval<s32>::make_half_open(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max() - 1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, ci.begin()->last());
}

// RingInterval::make_half_open_complement

TEST(RingIntervalTest, make_half_open_complement_normal)
{
    const auto interval = RingInterval<s32>::make_half_open_complement(5, 10);
    EXPECT_FALSE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(4));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(5));
    EXPECT_FALSE(interval.contains(9));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(2u, ci.size());
    auto it = ci.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(4, it->last());
    ++it;
    EXPECT_EQ(10, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());
}

TEST(RingIntervalTest, make_half_open_complement_first_equals_last)
{
    const auto interval = RingInterval<s32>::make_half_open_complement(5, 5);
    EXPECT_TRUE(interval.full());
}

TEST(RingIntervalTest, make_half_open_complement_min_to_max)
{
    const auto interval = RingInterval<s32>::make_half_open_complement(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

// RingInterval::make_closed

TEST(RingIntervalTest, make_closed_normal)
{
    const auto interval = RingInterval<s32>::make_closed(5, 10);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(11));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(5, ci.begin()->first());
    EXPECT_EQ(10, ci.begin()->last());
}

TEST(RingIntervalTest, make_closed_single_value)
{
    const auto interval = RingInterval<s32>::make_closed(42, 42);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(42, ci.begin()->first());
    EXPECT_EQ(42, ci.begin()->last());
}

TEST(RingIntervalTest, make_closed_min_to_max)
{
    const auto interval = RingInterval<s32>::make_closed(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_TRUE(interval.contains(0));
}

TEST(RingIntervalTest, make_closed_min_to_min)
{
    const auto interval = RingInterval<s32>::make_closed(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::min());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
}

TEST(RingIntervalTest, make_closed_max_to_max)
{
    const auto interval = RingInterval<s32>::make_closed(
        std::numeric_limits<s32>::max(), std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
}

TEST(RingIntervalTest, make_closed_min_to_value)
{
    const auto interval = RingInterval<s32>::make_closed(std::numeric_limits<s32>::min(), 10);
    EXPECT_TRUE(interval.continuous());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(11));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(10, ci.begin()->last());
}

TEST(RingIntervalTest, make_closed_value_to_max)
{
    const auto interval = RingInterval<s32>::make_closed(5, std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(4));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(5, ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

TEST(RingIntervalTest, make_closed_from_closed_interval)
{
    const ClosedInterval<s32> ci(5, 10);
    const auto interval = RingInterval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.continuous());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(11));
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    EXPECT_EQ(5, result.begin()->first());
    EXPECT_EQ(10, result.begin()->last());
}

TEST(RingIntervalTest, make_closed_from_closed_interval_single_value)
{
    const ClosedInterval<s32> ci(42, 42);
    const auto interval = RingInterval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
}

TEST(RingIntervalTest, make_closed_from_closed_interval_full)
{
    const ClosedInterval<s32> ci(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    const auto interval = RingInterval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.full());
}

// RingInterval::make_less

TEST(RingIntervalTest, make_less_normal)
{
    const auto interval = RingInterval<s32>::make_less(5);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(4));
    EXPECT_FALSE(interval.contains(5));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(4, ci.begin()->last());
}

TEST(RingIntervalTest, make_less_min)
{
    const auto interval = RingInterval<s32>::make_less(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.empty());
}

TEST(RingIntervalTest, make_less_max)
{
    const auto interval = RingInterval<s32>::make_less(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max() - 1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, ci.begin()->last());
}

// RingInterval::make_less_or_equal

TEST(RingIntervalTest, make_less_or_equal_normal)
{
    const auto interval = RingInterval<s32>::make_less_or_equal(5);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(5));
    EXPECT_FALSE(interval.contains(6));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(5, ci.begin()->last());
}

TEST(RingIntervalTest, make_less_or_equal_min)
{
    const auto interval = RingInterval<s32>::make_less_or_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->last());
}

TEST(RingIntervalTest, make_less_or_equal_max)
{
    const auto interval = RingInterval<s32>::make_less_or_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
}

// RingInterval::make_equal

TEST(RingIntervalTest, make_equal_normal)
{
    const auto interval = RingInterval<s32>::make_equal(42);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(42, ci.begin()->first());
    EXPECT_EQ(42, ci.begin()->last());
}

TEST(RingIntervalTest, make_equal_min)
{
    const auto interval = RingInterval<s32>::make_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->last());
}

TEST(RingIntervalTest, make_equal_max)
{
    const auto interval = RingInterval<s32>::make_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

// RingInterval::make_not_equal

TEST(RingIntervalTest, make_not_equal_normal)
{
    const auto interval = RingInterval<s32>::make_not_equal(42);
    EXPECT_FALSE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(41));
    EXPECT_TRUE(interval.contains(43));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(42));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(2u, ci.size());
    auto it = ci.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(41, it->last());
    ++it;
    EXPECT_EQ(43, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());
}

TEST(RingIntervalTest, make_not_equal_min)
{
    const auto interval = RingInterval<s32>::make_not_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min() + 1));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

TEST(RingIntervalTest, make_not_equal_max)
{
    const auto interval = RingInterval<s32>::make_not_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max() - 1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, ci.begin()->last());
}

// RingInterval::make_greater_or_equal

TEST(RingIntervalTest, make_greater_or_equal_normal)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(5);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(4));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(5, ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

TEST(RingIntervalTest, make_greater_or_equal_min)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
}

TEST(RingIntervalTest, make_greater_or_equal_max)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

// RingInterval::make_greater

TEST(RingIntervalTest, make_greater_normal)
{
    const auto interval = RingInterval<s32>::make_greater(5);
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(6));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(5));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(6, ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

TEST(RingIntervalTest, make_greater_min)
{
    const auto interval = RingInterval<s32>::make_greater(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.continuous());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min() + 1));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    const auto ci = interval.to_closed_intervals();
    EXPECT_EQ(1u, ci.size());
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, ci.begin()->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.begin()->last());
}

TEST(RingIntervalTest, make_greater_max)
{
    const auto interval = RingInterval<s32>::make_greater(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.empty());
}

// RingInterval complement

TEST(RingIntervalTest, complement_of_empty_is_full)
{
    const RingInterval<s32> empty_interval;
    const auto comp = empty_interval.complement();
    EXPECT_TRUE(comp.full());
    EXPECT_FALSE(comp.empty());
    EXPECT_TRUE(comp.continuous());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.empty());
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(empty_interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_empty_contains_all_values)
{
    const RingInterval<s32> empty_interval;
    const auto comp = empty_interval.complement();
    EXPECT_TRUE(comp.full());
    EXPECT_FALSE(comp.empty());
    EXPECT_TRUE(comp.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(comp.contains(-1));
    EXPECT_TRUE(comp.contains(0));
    EXPECT_TRUE(comp.contains(1));
    EXPECT_TRUE(comp.contains(std::numeric_limits<s32>::max()));
}

TEST(RingIntervalTest, complement_of_full_is_empty)
{
    const auto interval = RingInterval<s32>::make_full();
    const auto comp = interval.complement();
    EXPECT_TRUE(comp.empty());
    EXPECT_FALSE(comp.full());
    EXPECT_TRUE(comp.continuous());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.full());
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_continuous_interior_is_discontinuous)
{
    const auto interval = RingInterval<s32>::make_half_open(5, 10);
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_FALSE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(2u, intervals.size());
    auto it = intervals.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(4, it->last());
    ++it;
    EXPECT_EQ(10, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_continuous_includes_min_is_continuous)
{
    const auto interval = RingInterval<s32>::make_half_open(std::numeric_limits<s32>::min(), 10);
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(1u, intervals.size());
    const auto & ci = *intervals.begin();
    EXPECT_EQ(10, ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_continuous_includes_max_is_continuous)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(5);
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(1u, intervals.size());
    const auto & ci = *intervals.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.first());
    EXPECT_EQ(4, ci.last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_discontinuous_is_continuous)
{
    const auto interval = RingInterval<s32>::make_half_open_complement(3, 5);
    EXPECT_FALSE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(1u, intervals.size());
    const auto & ci = *intervals.begin();
    EXPECT_EQ(3, ci.first());
    EXPECT_EQ(4, ci.last());

    const auto roundtrip = comp.complement();
    EXPECT_FALSE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_single_value_is_discontinuous)
{
    const auto interval = RingInterval<s32>::make_equal(42);
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_FALSE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(2u, intervals.size());
    auto it = intervals.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(41, it->last());
    ++it;
    EXPECT_EQ(43, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_min_only_is_continuous)
{
    const auto interval = RingInterval<s32>::make_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(1u, intervals.size());
    const auto & ci = *intervals.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

TEST(RingIntervalTest, complement_of_max_only_is_continuous)
{
    const auto interval = RingInterval<s32>::make_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.continuous());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.continuous());
    EXPECT_FALSE(comp.empty());
    EXPECT_FALSE(comp.full());

    const auto intervals = comp.to_closed_intervals();
    EXPECT_EQ(1u, intervals.size());
    const auto & ci = *intervals.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, ci.last());

    const auto roundtrip = comp.complement();
    EXPECT_TRUE(roundtrip.continuous());
    EXPECT_EQ(interval.to_closed_intervals(), roundtrip.to_closed_intervals());
    EXPECT_EQ(interval, roundtrip);
}

// RingInterval to_closed_intervals

TEST(RingIntervalTest, to_closed_intervals_empty)
{
    const RingInterval<s32> interval;
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(0u, result.size());
}

TEST(RingIntervalTest, to_closed_intervals_full)
{
    const auto interval = RingInterval<s32>::make_full();
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    const auto & ci = *result.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.last());
}

TEST(RingIntervalTest, to_closed_intervals_normal)
{
    const auto interval = RingInterval<s32>::make_half_open(5, 10);
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    const auto & ci = *result.begin();
    EXPECT_EQ(5, ci.first());
    EXPECT_EQ(9, ci.last());
}

TEST(RingIntervalTest, to_closed_intervals_single_value)
{
    const auto interval = RingInterval<s32>::make_equal(42);
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    const auto & ci = *result.begin();
    EXPECT_EQ(42, ci.first());
    EXPECT_EQ(42, ci.last());
}

TEST(RingIntervalTest, to_closed_intervals_normal_min_to_max_minus_1)
{
    const auto interval =
        RingInterval<s32>::make_half_open(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    const auto & ci = *result.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, ci.last());
}

TEST(RingIntervalTest, to_closed_intervals_discontinuous_two_parts)
{
    // RingInterval(5, 3) = [min, 3) U [5, max] -> [min, 2] and [5, max]
    const auto interval = RingInterval<s32>::make_half_open_complement(3, 5);
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(2u, result.size());
    auto it = result.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(2, it->last());
    ++it;
    EXPECT_EQ(5, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());
}

TEST(RingIntervalTest, to_closed_intervals_discontinuous_left_empty)
{
    // RingInterval(min+1, min) = [min, min) U [min+1, max] -> just [min+1, max]
    const auto interval = RingInterval<s32>::make_greater(std::numeric_limits<s32>::min());
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(1u, result.size());
    const auto & ci = *result.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, ci.first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), ci.last());
}

TEST(RingIntervalTest, to_closed_intervals_complement_of_single_value)
{
    // Complement of {42}: [min, 42) U [43, max]
    const auto interval = RingInterval<s32>::make_not_equal(42);
    const auto result = interval.to_closed_intervals();
    EXPECT_EQ(2u, result.size());
    auto it = result.begin();
    EXPECT_EQ(std::numeric_limits<s32>::min(), it->first());
    EXPECT_EQ(41, it->last());
    ++it;
    EXPECT_EQ(43, it->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), it->last());
}

// RingInterval to_intervals

TEST(RingIntervalTest, to_intervals_empty)
{
    const RingInterval<s32> interval;
    const auto [left, right] = interval.to_intervals();
    EXPECT_TRUE(left.empty());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_full)
{
    const auto interval = RingInterval<s32>::make_full();
    const auto [left, right] = interval.to_intervals();
    EXPECT_TRUE(left.full());
    EXPECT_FALSE(left.empty());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_normal)
{
    const auto interval = RingInterval<s32>::make_half_open(5, 10);
    const auto [left, right] = interval.to_intervals();
    const auto closed = left.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_single_value)
{
    const auto interval = RingInterval<s32>::make_equal(42);
    const auto [left, right] = interval.to_intervals();
    const auto closed = left.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(42, closed->first());
    EXPECT_EQ(42, closed->last());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_less)
{
    const auto interval = RingInterval<s32>::make_less(5);
    const auto [left, right] = interval.to_intervals();
    const auto closed = left.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), closed->first());
    EXPECT_EQ(4, closed->last());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_greater_or_equal)
{
    const auto interval = RingInterval<s32>::make_greater_or_equal(5);
    const auto [left, right] = interval.to_intervals();
    const auto closed = left.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), closed->last());
    EXPECT_TRUE(right.empty());
}

TEST(RingIntervalTest, to_intervals_discontinuous_two_parts)
{
    // make_half_open_complement(5, 10) = complement of [5, 10) = [min, 5) U [10, max]
    const auto interval = RingInterval<s32>::make_half_open_complement(5, 10);
    const auto [left, right] = interval.to_intervals();
    EXPECT_FALSE(left.empty());
    EXPECT_FALSE(right.empty());
    const auto left_closed = left.to_closed_interval();
    ASSERT_TRUE(left_closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), left_closed->first());
    EXPECT_EQ(4, left_closed->last());
    const auto right_closed = right.to_closed_interval();
    ASSERT_TRUE(right_closed.has_value());
    EXPECT_EQ(10, right_closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), right_closed->last());
}

TEST(RingIntervalTest, to_intervals_not_equal)
{
    // make_not_equal(42) = complement of {42} = [min, 42) U [43, max]
    const auto interval = RingInterval<s32>::make_not_equal(42);
    const auto [left, right] = interval.to_intervals();
    EXPECT_FALSE(left.empty());
    EXPECT_FALSE(right.empty());
    const auto left_closed = left.to_closed_interval();
    ASSERT_TRUE(left_closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), left_closed->first());
    EXPECT_EQ(41, left_closed->last());
    const auto right_closed = right.to_closed_interval();
    ASSERT_TRUE(right_closed.has_value());
    EXPECT_EQ(43, right_closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), right_closed->last());
}

TEST(RingIntervalTest, to_intervals_normal_min_to_max_minus_1)
{
    const auto interval =
        RingInterval<s32>::make_half_open(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    const auto [left, right] = interval.to_intervals();
    const auto closed = left.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, closed->last());
    EXPECT_TRUE(right.empty());
}

// RingInterval with custom traits combinations

TEST(RingIntervalTest, less_only_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, LessOnly>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = LessOnly::min();
    const NoBuiltinOperatorsInt max_val = LessOnly::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, cmp_only_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, CmpOnly>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = CmpOnly::min();
    const NoBuiltinOperatorsInt max_val = CmpOnly::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, less_equal_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, LessEqual>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = LessEqual::min();
    const NoBuiltinOperatorsInt max_val = LessEqual::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, cmp_equal_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, CmpEqual>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = CmpEqual::min();
    const NoBuiltinOperatorsInt max_val = CmpEqual::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, all_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, AllTraits>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = AllTraits::min();
    const NoBuiltinOperatorsInt max_val = AllTraits::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, less_cmp_traits)
{
    using R = RingInterval<NoBuiltinOperatorsInt, LessCmp>;
    R interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.continuous());

    const NoBuiltinOperatorsInt min_val = LessCmp::min();
    const NoBuiltinOperatorsInt max_val = LessCmp::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());

    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

// Interval default construction and basic operations

TEST(IntervalTest, default_empty)
{
    const Interval<s32> interval;
    EXPECT_TRUE(interval.empty());
}

TEST(IntervalTest, default_full)
{
    const Interval<s32> interval;
    EXPECT_FALSE(interval.full());
}

TEST(IntervalTest, default_contains)
{
    const Interval<s32> interval;
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(-1));
    EXPECT_FALSE(interval.contains(0));
    EXPECT_FALSE(interval.contains(1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
}

TEST(IntervalTest, default_size)
{
    const Interval<s32> interval;
    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, copy_constructible)
{
    const Interval<s32> interval;
    auto copy = interval;
    EXPECT_EQ(interval.empty(), copy.empty());
}

TEST(IntervalTest, copy_assignable)
{
    const Interval<s32> interval;
    Interval<s32> other;
    other = interval;
    EXPECT_TRUE(other.empty());
}

TEST(IntervalTest, move_constructible)
{
    Interval<s32> interval;
    auto moved = std::move(interval);
    EXPECT_TRUE(moved.empty());
}

TEST(IntervalTest, move_assignable)
{
    Interval<s32> interval;
    Interval<s32> other;
    other = std::move(interval);
    EXPECT_TRUE(other.empty());
}

// Interval make functions

TEST(IntervalTest, make_empty)
{
    const auto interval = Interval<s32>::make_empty();
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, make_full)
{
    const auto interval = Interval<s32>::make_full();
    EXPECT_TRUE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_TRUE(interval.contains(0));
}

// Interval::make_half_open

TEST(IntervalTest, make_half_open_normal)
{
    const auto interval = Interval<s32>::make_half_open(5, 10);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(9));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(10));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(9, result->last());
}

TEST(IntervalTest, make_half_open_first_equals_last)
{
    const auto interval = Interval<s32>::make_half_open(5, 5);
    EXPECT_TRUE(interval.empty());
}

TEST(IntervalTest, make_half_open_min_to_max)
{
    const auto interval = Interval<s32>::make_half_open(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max() - 1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, result->last());
}

// Interval::make_closed

TEST(IntervalTest, make_closed_normal)
{
    const auto interval = Interval<s32>::make_closed(5, 10);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(11));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(10, result->last());
}

TEST(IntervalTest, make_closed_single_value)
{
    const auto interval = Interval<s32>::make_closed(42, 42);
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(42, result->first());
    EXPECT_EQ(42, result->last());
}

TEST(IntervalTest, make_closed_min_to_max)
{
    const auto interval = Interval<s32>::make_closed(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_TRUE(interval.contains(0));
}

TEST(IntervalTest, make_closed_min_to_min)
{
    const auto interval = Interval<s32>::make_closed(
        std::numeric_limits<s32>::min(), std::numeric_limits<s32>::min());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
}

TEST(IntervalTest, make_closed_max_to_max)
{
    const auto interval = Interval<s32>::make_closed(
        std::numeric_limits<s32>::max(), std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
}

TEST(IntervalTest, make_closed_min_to_value)
{
    const auto interval = Interval<s32>::make_closed(std::numeric_limits<s32>::min(), 10);
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(11));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(10, result->last());
}

TEST(IntervalTest, make_closed_value_to_max)
{
    const auto interval = Interval<s32>::make_closed(5, std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(4));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, make_closed_from_closed_interval)
{
    const ClosedInterval<s32> ci(5, 10);
    const auto interval = Interval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(10));
    EXPECT_FALSE(interval.contains(4));
    EXPECT_FALSE(interval.contains(11));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(10, result->last());
}

TEST(IntervalTest, make_closed_from_closed_interval_single_value)
{
    const ClosedInterval<s32> ci(42, 42);
    const auto interval = Interval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
}

TEST(IntervalTest, make_closed_from_closed_interval_full)
{
    const ClosedInterval<s32> ci(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    const auto interval = Interval<s32>::make_closed(ci);
    EXPECT_TRUE(interval.full());
}

// Interval::make_less

TEST(IntervalTest, make_less_normal)
{
    const auto interval = Interval<s32>::make_less(5);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(4));
    EXPECT_FALSE(interval.contains(5));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(4, result->last());
}

TEST(IntervalTest, make_less_min)
{
    const auto interval = Interval<s32>::make_less(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.empty());
}

TEST(IntervalTest, make_less_max)
{
    const auto interval = Interval<s32>::make_less(std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max() - 1));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, result->last());
}

// Interval::make_less_or_equal

TEST(IntervalTest, make_less_or_equal_normal)
{
    const auto interval = Interval<s32>::make_less_or_equal(5);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(5));
    EXPECT_FALSE(interval.contains(6));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(5, result->last());
}

TEST(IntervalTest, make_less_or_equal_min)
{
    const auto interval = Interval<s32>::make_less_or_equal(std::numeric_limits<s32>::min());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->last());
}

TEST(IntervalTest, make_less_or_equal_max)
{
    const auto interval = Interval<s32>::make_less_or_equal(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
}

// Interval::make_equal

TEST(IntervalTest, make_equal_normal)
{
    const auto interval = Interval<s32>::make_equal(42);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(42));
    EXPECT_FALSE(interval.contains(41));
    EXPECT_FALSE(interval.contains(43));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(42, result->first());
    EXPECT_EQ(42, result->last());
}

TEST(IntervalTest, make_equal_min)
{
    const auto interval = Interval<s32>::make_equal(std::numeric_limits<s32>::min());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min() + 1));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->last());
}

TEST(IntervalTest, make_equal_max)
{
    const auto interval = Interval<s32>::make_equal(std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.empty());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

// Interval::make_greater_or_equal

TEST(IntervalTest, make_greater_or_equal_normal)
{
    const auto interval = Interval<s32>::make_greater_or_equal(5);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(5));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(4));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, make_greater_or_equal_min)
{
    const auto interval = Interval<s32>::make_greater_or_equal(std::numeric_limits<s32>::min());
    EXPECT_TRUE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
}

TEST(IntervalTest, make_greater_or_equal_max)
{
    const auto interval = Interval<s32>::make_greater_or_equal(std::numeric_limits<s32>::max());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::max() - 1));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

// Interval::make_greater

TEST(IntervalTest, make_greater_normal)
{
    const auto interval = Interval<s32>::make_greater(5);
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_TRUE(interval.contains(6));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(interval.contains(5));
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(6, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, make_greater_min)
{
    const auto interval = Interval<s32>::make_greater(std::numeric_limits<s32>::min());
    EXPECT_FALSE(interval.empty());
    EXPECT_FALSE(interval.full());
    EXPECT_FALSE(interval.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::min() + 1));
    EXPECT_TRUE(interval.contains(std::numeric_limits<s32>::max()));
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, make_greater_max)
{
    const auto interval = Interval<s32>::make_greater(std::numeric_limits<s32>::max());
    EXPECT_TRUE(interval.empty());
}

// Interval to_closed_interval

TEST(IntervalTest, to_closed_interval_default_empty)
{
    const Interval<s32> interval;
    const auto result = interval.to_closed_interval();
    EXPECT_FALSE(result.has_value());
}

TEST(IntervalTest, to_closed_interval_full)
{
    const auto interval = Interval<s32>::make_full();
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, to_closed_interval_normal)
{
    const auto interval = Interval<s32>::make_half_open(5, 10);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(9, result->last());
}

TEST(IntervalTest, to_closed_interval_single_value)
{
    const auto interval = Interval<s32>::make_equal(42);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(42, result->first());
    EXPECT_EQ(42, result->last());
}

TEST(IntervalTest, to_closed_interval_less)
{
    const auto interval = Interval<s32>::make_less(5);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(4, result->last());
}

TEST(IntervalTest, to_closed_interval_less_or_equal)
{
    const auto interval = Interval<s32>::make_less_or_equal(5);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(5, result->last());
}

TEST(IntervalTest, to_closed_interval_greater)
{
    const auto interval = Interval<s32>::make_greater(5);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(6, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, to_closed_interval_greater_or_equal)
{
    const auto interval = Interval<s32>::make_greater_or_equal(5);
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(5, result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

TEST(IntervalTest, to_closed_interval_greater_or_equal_min)
{
    const auto interval = Interval<s32>::make_greater_or_equal(std::numeric_limits<s32>::min());
    const auto result = interval.to_closed_interval();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), result->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), result->last());
}

// Interval::intersect

TEST(IntervalTest, intersect_empty_with_empty)
{
    const Interval<s32> a;
    const Interval<s32> b;
    const auto result = a.intersect(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_empty_with_full)
{
    const Interval<s32> empty;
    const auto full = Interval<s32>::make_full();
    const auto result = empty.intersect(full);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_full_with_empty)
{
    const auto full = Interval<s32>::make_full();
    const Interval<s32> empty;
    const auto result = full.intersect(empty);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_full_with_full)
{
    const auto a = Interval<s32>::make_full();
    const auto b = Interval<s32>::make_full();
    const auto result = a.intersect(b);
    EXPECT_TRUE(result.full());
}

TEST(IntervalTest, intersect_identical_normal)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(5, 10);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_identical_single_value)
{
    const auto a = Interval<s32>::make_equal(42);
    const auto b = Interval<s32>::make_equal(42);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(42, closed->first());
    EXPECT_EQ(42, closed->last());
}

TEST(IntervalTest, intersect_overlapping_partial)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(8, 15);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(8, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_overlapping_symmetric)
{
    const auto a = Interval<s32>::make_half_open(8, 15);
    const auto b = Interval<s32>::make_half_open(5, 10);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(8, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_disjoint_left_before_right)
{
    const auto a = Interval<s32>::make_half_open(1, 5);
    const auto b = Interval<s32>::make_half_open(10, 20);
    const auto result = a.intersect(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_disjoint_right_before_left)
{
    const auto a = Interval<s32>::make_half_open(10, 20);
    const auto b = Interval<s32>::make_half_open(1, 5);
    const auto result = a.intersect(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_touching_at_boundary)
{
    const auto a = Interval<s32>::make_half_open(1, 5);
    const auto b = Interval<s32>::make_half_open(5, 10);
    const auto result = a.intersect(b);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_one_contains_other)
{
    const auto outer = Interval<s32>::make_half_open(1, 20);
    const auto inner = Interval<s32>::make_half_open(5, 10);
    const auto result = outer.intersect(inner);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_full_with_normal)
{
    const auto full = Interval<s32>::make_full();
    const auto normal = Interval<s32>::make_half_open(5, 10);
    const auto result = full.intersect(normal);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_normal_with_full)
{
    const auto normal = Interval<s32>::make_half_open(5, 10);
    const auto full = Interval<s32>::make_full();
    const auto result = normal.intersect(full);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_less_with_greater_or_equal)
{
    const auto less = Interval<s32>::make_less(10);
    const auto ge = Interval<s32>::make_greater_or_equal(5);
    const auto result = less.intersect(ge);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(IntervalTest, intersect_disjoint_less_and_greater)
{
    const auto less = Interval<s32>::make_less(5);
    const auto greater = Interval<s32>::make_greater(10);
    const auto result = less.intersect(greater);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_single_value_in_range)
{
    const auto range = Interval<s32>::make_half_open(1, 10);
    const auto single = Interval<s32>::make_equal(5);
    const auto result = range.intersect(single);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(5, closed->last());
}

TEST(IntervalTest, intersect_single_value_out_of_range)
{
    const auto range = Interval<s32>::make_half_open(1, 10);
    const auto single = Interval<s32>::make_equal(15);
    const auto result = range.intersect(single);
    EXPECT_TRUE(result.empty());
}

TEST(IntervalTest, intersect_at_max_boundary)
{
    const auto a = Interval<s32>::make_greater_or_equal(std::numeric_limits<s32>::max() - 2);
    const auto b = Interval<s32>::make_greater_or_equal(std::numeric_limits<s32>::max() - 1);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::max() - 1, closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), closed->last());
}

TEST(IntervalTest, intersect_at_min_boundary)
{
    const auto a = Interval<s32>::make_less(std::numeric_limits<s32>::min() + 3);
    const auto b = Interval<s32>::make_less(std::numeric_limits<s32>::min() + 2);
    const auto result = a.intersect(b);
    const auto closed = result.to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::min() + 1, closed->last());
}

// Interval with custom traits combinations

TEST(IntervalTest, less_only_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, LessOnly>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = LessOnly::min();
    const NoBuiltinOperatorsInt max_val = LessOnly::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, cmp_only_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, CmpOnly>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = CmpOnly::min();
    const NoBuiltinOperatorsInt max_val = CmpOnly::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, less_equal_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, LessEqual>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = LessEqual::min();
    const NoBuiltinOperatorsInt max_val = LessEqual::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, cmp_equal_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, CmpEqual>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = CmpEqual::min();
    const NoBuiltinOperatorsInt max_val = CmpEqual::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, all_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, AllTraits>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = AllTraits::min();
    const NoBuiltinOperatorsInt max_val = AllTraits::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

TEST(IntervalTest, less_cmp_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, LessCmp>;
    I interval;
    EXPECT_TRUE(interval.empty());
    EXPECT_FALSE(interval.full());

    const NoBuiltinOperatorsInt min_val = LessCmp::min();
    const NoBuiltinOperatorsInt max_val = LessCmp::max();
    const NoBuiltinOperatorsInt zero { 0 };

    EXPECT_FALSE(interval.contains(min_val));
    EXPECT_FALSE(interval.contains(zero));
    EXPECT_FALSE(interval.contains(max_val));

    EXPECT_EQ(size_t { 0u }, interval.size());
}

// Interval to RingInterval implicit conversion

TEST(IntervalToRingIntervalTest, empty)
{
    const Interval<s32> interval;
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.empty());
    EXPECT_TRUE(ring.continuous());
}

TEST(IntervalToRingIntervalTest, full)
{
    const auto interval = Interval<s32>::make_full();
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.full());
    EXPECT_TRUE(ring.continuous());
}

TEST(IntervalToRingIntervalTest, normal_half_open)
{
    const auto interval = Interval<s32>::make_half_open(5, 10);
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.continuous());
    EXPECT_TRUE(ring.contains(5));
    EXPECT_TRUE(ring.contains(9));
    EXPECT_FALSE(ring.contains(4));
    EXPECT_FALSE(ring.contains(10));
}

TEST(IntervalToRingIntervalTest, single_value)
{
    const auto interval = Interval<s32>::make_equal(42);
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.continuous());
    EXPECT_TRUE(ring.contains(42));
    EXPECT_FALSE(ring.contains(41));
    EXPECT_FALSE(ring.contains(43));
}

TEST(IntervalToRingIntervalTest, greater_or_equal)
{
    const auto interval = Interval<s32>::make_greater_or_equal(5);
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.continuous());
    EXPECT_TRUE(ring.contains(5));
    EXPECT_TRUE(ring.contains(std::numeric_limits<s32>::max()));
    EXPECT_FALSE(ring.contains(4));
}

TEST(IntervalToRingIntervalTest, less)
{
    const auto interval = Interval<s32>::make_less(5);
    const auto ring = interval.to_ring_interval();
    EXPECT_TRUE(ring.continuous());
    EXPECT_TRUE(ring.contains(std::numeric_limits<s32>::min()));
    EXPECT_TRUE(ring.contains(4));
    EXPECT_FALSE(ring.contains(5));
}

TEST(IntervalToRingIntervalTest, size_preserved)
{
    const auto interval = Interval<s32>::make_half_open(5, 10);
    const auto ring = interval.to_ring_interval();
    EXPECT_EQ(interval.size(), ring.size());
}

TEST(IntervalToRingIntervalTest, to_closed_intervals_match)
{
    const auto interval = Interval<s32>::make_half_open(5, 10);
    const auto ring = interval.to_ring_interval();
    const auto ring_intervals = ring.to_closed_intervals();
    EXPECT_EQ(1u, ring_intervals.size());
    const auto & ci = *ring_intervals.begin();
    EXPECT_EQ(5, ci.first());
    EXPECT_EQ(9, ci.last());
}

// RingInterval::to_interval

TEST(RingIntervalToIntervalTest, empty)
{
    const RingInterval<s32> ring;
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    EXPECT_TRUE(interval->empty());
}

TEST(RingIntervalToIntervalTest, full)
{
    const auto ring = RingInterval<s32>::make_full();
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    EXPECT_TRUE(interval->full());
}

TEST(RingIntervalToIntervalTest, normal_half_open)
{
    const auto ring = RingInterval<s32>::make_half_open(5, 10);
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    const auto closed = interval->to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(9, closed->last());
}

TEST(RingIntervalToIntervalTest, single_value)
{
    const auto ring = RingInterval<s32>::make_equal(42);
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    const auto closed = interval->to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(42, closed->first());
    EXPECT_EQ(42, closed->last());
}

TEST(RingIntervalToIntervalTest, greater_or_equal)
{
    const auto ring = RingInterval<s32>::make_greater_or_equal(5);
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    const auto closed = interval->to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(5, closed->first());
    EXPECT_EQ(std::numeric_limits<s32>::max(), closed->last());
}

TEST(RingIntervalToIntervalTest, less)
{
    const auto ring = RingInterval<s32>::make_less(5);
    const auto interval = ring.to_interval();
    ASSERT_TRUE(interval.has_value());
    const auto closed = interval->to_closed_interval();
    ASSERT_TRUE(closed.has_value());
    EXPECT_EQ(std::numeric_limits<s32>::min(), closed->first());
    EXPECT_EQ(4, closed->last());
}

TEST(RingIntervalToIntervalTest, discontinuous_returns_nullopt)
{
    const auto ring = RingInterval<s32>::make_half_open_complement(3, 5);
    EXPECT_FALSE(ring.continuous());
    EXPECT_FALSE(ring.to_interval().has_value());
}

TEST(RingIntervalToIntervalTest, not_equal_returns_nullopt)
{
    const auto ring = RingInterval<s32>::make_not_equal(42);
    EXPECT_FALSE(ring.continuous());
    EXPECT_FALSE(ring.to_interval().has_value());
}

// Roundtrip: Interval -> RingInterval -> Interval

TEST(IntervalRoundtrip, empty)
{
    const Interval<s32> original;
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

TEST(IntervalRoundtrip, full)
{
    const auto original = Interval<s32>::make_full();
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

TEST(IntervalRoundtrip, normal_half_open)
{
    const auto original = Interval<s32>::make_half_open(5, 10);
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

TEST(IntervalRoundtrip, single_value)
{
    const auto original = Interval<s32>::make_equal(42);
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

TEST(IntervalRoundtrip, greater_or_equal)
{
    const auto original = Interval<s32>::make_greater_or_equal(5);
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

TEST(IntervalRoundtrip, less)
{
    const auto original = Interval<s32>::make_less(5);
    const auto ring = original.to_ring_interval();
    const auto roundtrip = ring.to_interval();
    ASSERT_TRUE(roundtrip.has_value());
    EXPECT_EQ(original.to_closed_interval(), roundtrip->to_closed_interval());
}

// RingInterval equality operators

TEST(RingIntervalEquality, equal_empty)
{
    const RingInterval<s32> a;
    const RingInterval<s32> b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, equal_full)
{
    const auto a = RingInterval<s32>::make_full();
    const auto b = RingInterval<s32>::make_full();
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, equal_half_open)
{
    const auto a = RingInterval<s32>::make_half_open(5, 10);
    const auto b = RingInterval<s32>::make_half_open(5, 10);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, equal_single_value)
{
    const auto a = RingInterval<s32>::make_equal(42);
    const auto b = RingInterval<s32>::make_equal(42);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, not_equal_different_first)
{
    const auto a = RingInterval<s32>::make_half_open(5, 10);
    const auto b = RingInterval<s32>::make_half_open(6, 10);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(RingIntervalEquality, not_equal_different_last)
{
    const auto a = RingInterval<s32>::make_half_open(5, 10);
    const auto b = RingInterval<s32>::make_half_open(5, 11);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(RingIntervalEquality, not_equal_empty_vs_full)
{
    const RingInterval<s32> empty;
    const auto full = RingInterval<s32>::make_full();
    EXPECT_FALSE(empty == full);
    EXPECT_TRUE(empty != full);
}

TEST(RingIntervalEquality, not_equal_empty_vs_single)
{
    const RingInterval<s32> empty;
    const auto single = RingInterval<s32>::make_equal(0);
    EXPECT_FALSE(empty == single);
    EXPECT_TRUE(empty != single);
}

TEST(RingIntervalEquality, not_equal_discontinuous_vs_continuous)
{
    const auto a = RingInterval<s32>::make_half_open_complement(3, 5);
    const auto b = RingInterval<s32>::make_half_open(3, 5);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(RingIntervalEquality, equal_complement_of_single_value)
{
    const auto a = RingInterval<s32>::make_not_equal(42);
    const auto b = RingInterval<s32>::make_not_equal(42);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, equal_greater_or_equal)
{
    const auto a = RingInterval<s32>::make_greater_or_equal(5);
    const auto b = RingInterval<s32>::make_greater_or_equal(5);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(RingIntervalEquality, not_equal_greater_or_equal_vs_less)
{
    const auto a = RingInterval<s32>::make_greater_or_equal(5);
    const auto b = RingInterval<s32>::make_less(5);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

// RingInterval hashing

TEST(RingIntervalHashTest, equal_intervals_have_equal_hashes)
{
    const auto a = RingInterval<s32>::make_half_open(5, 10);
    const auto b = RingInterval<s32>::make_half_open(5, 10);
    Hasher hash_a;
    hash_a.update(a);
    Hasher hash_b;
    hash_b.update(b);
    EXPECT_EQ(hash_a.digest(), hash_b.digest());
}

TEST(RingIntervalHashTest, different_intervals_have_different_hashes)
{
    const auto a = RingInterval<s32>::make_half_open(5, 10);
    const auto b = RingInterval<s32>::make_half_open(6, 10);
    Hasher hash_a;
    hash_a.update(a);
    Hasher hash_b;
    hash_b.update(b);
    EXPECT_NE(hash_a.digest(), hash_b.digest());
}

// Interval equality operators

TEST(IntervalEquality, equal_empty)
{
    const Interval<s32> a;
    const Interval<s32> b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IntervalEquality, equal_full)
{
    const auto a = Interval<s32>::make_full();
    const auto b = Interval<s32>::make_full();
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IntervalEquality, equal_half_open)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(5, 10);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IntervalEquality, equal_single_value)
{
    const auto a = Interval<s32>::make_equal(42);
    const auto b = Interval<s32>::make_equal(42);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IntervalEquality, not_equal_different_first)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(6, 10);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(IntervalEquality, not_equal_different_last)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(5, 11);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

TEST(IntervalEquality, not_equal_empty_vs_full)
{
    const Interval<s32> empty;
    const auto full = Interval<s32>::make_full();
    EXPECT_FALSE(empty == full);
    EXPECT_TRUE(empty != full);
}

TEST(IntervalEquality, not_equal_empty_vs_single)
{
    const Interval<s32> empty;
    const auto single = Interval<s32>::make_equal(0);
    EXPECT_FALSE(empty == single);
    EXPECT_TRUE(empty != single);
}

TEST(IntervalEquality, equal_greater_or_equal)
{
    const auto a = Interval<s32>::make_greater_or_equal(5);
    const auto b = Interval<s32>::make_greater_or_equal(5);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST(IntervalEquality, not_equal_greater_or_equal_vs_less)
{
    const auto a = Interval<s32>::make_greater_or_equal(5);
    const auto b = Interval<s32>::make_less(5);
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
}

// Interval hashing

TEST(IntervalHashTest, equal_intervals_have_equal_hashes)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(5, 10);
    Hasher hash_a;
    hash_a.update(a);
    Hasher hash_b;
    hash_b.update(b);
    EXPECT_EQ(hash_a.digest(), hash_b.digest());
}

TEST(IntervalHashTest, different_intervals_have_different_hashes)
{
    const auto a = Interval<s32>::make_half_open(5, 10);
    const auto b = Interval<s32>::make_half_open(6, 10);
    Hasher hash_a;
    hash_a.update(a);
    Hasher hash_b;
    hash_b.update(b);
    EXPECT_NE(hash_a.digest(), hash_b.digest());
}

// Cross-type equality: RingInterval vs Interval

TEST(CrossIntervalEquality, ring_equal_to_interval_half_open)
{
    const auto ring = RingInterval<s32>::make_half_open(5, 10);
    const auto interval = Interval<s32>::make_half_open(5, 10);
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_not_equal_to_interval_different)
{
    const auto ring = RingInterval<s32>::make_half_open(5, 10);
    const auto interval = Interval<s32>::make_half_open(6, 10);
    EXPECT_FALSE(ring == interval);
    EXPECT_TRUE(ring != interval);
    EXPECT_FALSE(interval == ring);
    EXPECT_TRUE(interval != ring);
}

TEST(CrossIntervalEquality, ring_empty_equal_to_interval_empty)
{
    const RingInterval<s32> ring;
    const Interval<s32> interval;
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_full_equal_to_interval_full)
{
    const auto ring = RingInterval<s32>::make_full();
    const auto interval = Interval<s32>::make_full();
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_single_value_equal_to_interval_single_value)
{
    const auto ring = RingInterval<s32>::make_equal(42);
    const auto interval = Interval<s32>::make_equal(42);
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_not_equal_to_interval_empty_vs_full)
{
    const RingInterval<s32> ring;
    const auto interval = Interval<s32>::make_full();
    EXPECT_FALSE(ring == interval);
    EXPECT_TRUE(ring != interval);
    EXPECT_FALSE(interval == ring);
    EXPECT_TRUE(interval != ring);
}

TEST(CrossIntervalEquality, ring_greater_or_equal_equal_to_interval_greater_or_equal)
{
    const auto ring = RingInterval<s32>::make_greater_or_equal(5);
    const auto interval = Interval<s32>::make_greater_or_equal(5);
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_less_equal_to_interval_less)
{
    const auto ring = RingInterval<s32>::make_less(5);
    const auto interval = Interval<s32>::make_less(5);
    EXPECT_TRUE(ring == interval);
    EXPECT_FALSE(ring != interval);
    EXPECT_TRUE(interval == ring);
    EXPECT_FALSE(interval != ring);
}

TEST(CrossIntervalEquality, ring_discontinuous_not_equal_to_continuous)
{
    const auto ring = RingInterval<s32>::make_half_open_complement(3, 5);
    const auto interval = Interval<s32>::make_half_open(3, 5);
    EXPECT_FALSE(ring == interval);
    EXPECT_TRUE(ring != interval);
    EXPECT_FALSE(interval == ring);
    EXPECT_TRUE(interval != ring);
}

} // namespace ka
