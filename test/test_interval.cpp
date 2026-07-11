#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <limits>
#include <tuple>
#include <type_traits>

#include <ka/common/fixed.hpp>
#include <ka/common/interval.hpp>
#include <ka/common/interval_traits.hpp>

#include "helpers/interval.hpp"

namespace ka
{

using std::get;

// Trait type aliases for testing different operator combinations

using LessOnly = NoBuiltinOperatorsIntIntervalTraits<true, false, false, size_t>;
using CmpOnly = NoBuiltinOperatorsIntIntervalTraits<false, false, true, size_t>;
using LessEqual = NoBuiltinOperatorsIntIntervalTraits<true, true, false, size_t>;
using CmpEqual = NoBuiltinOperatorsIntIntervalTraits<false, true, true, size_t>;
using AllTraits = NoBuiltinOperatorsIntIntervalTraits<true, true, true, size_t>;
using LessCmp = NoBuiltinOperatorsIntIntervalTraits<true, false, true, size_t>;

// RingInterval default construction and basic operations

TEST(RingIntervalTest, default_first_last)
{
    const RingInterval<s32> interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), interval.first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), interval.last());
}

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

TEST(RingIntervalTest, default_complement)
{
    const RingInterval<s32> interval;
    const auto comp = interval.complement();
    EXPECT_TRUE(comp.full());
}

TEST(RingIntervalTest, complement_double)
{
    const RingInterval<s32> interval;
    const auto comp = interval.complement();
    const auto comp2 = comp.complement();
    EXPECT_TRUE(comp2.empty());
    EXPECT_EQ(interval.first(), comp2.first());
    EXPECT_EQ(interval.last(), comp2.last());
}

TEST(RingIntervalTest, complement_empty_is_full)
{
    const RingInterval<s32> empty_interval;
    const auto comp = empty_interval.complement();
    EXPECT_TRUE(comp.full());
    EXPECT_FALSE(comp.empty());
    EXPECT_TRUE(comp.continuous());
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

TEST(RingIntervalTest, structured_bindings)
{
    const RingInterval<s32> interval;
    const auto & [first, last] = interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), first);
    EXPECT_EQ(std::numeric_limits<s32>::min(), last);
}

TEST(RingIntervalTest, copy_constructible)
{
    const RingInterval<s32> interval;
    auto copy = interval;
    EXPECT_EQ(interval.first(), copy.first());
    EXPECT_EQ(interval.last(), copy.last());
    EXPECT_EQ(interval.empty(), copy.empty());
}

TEST(RingIntervalTest, copy_assignable)
{
    const RingInterval<s32> interval;
    RingInterval<s32> other;
    other = interval;
    EXPECT_EQ(interval.first(), other.first());
    EXPECT_EQ(interval.last(), other.last());
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

    EXPECT_EQ(min_val.value, interval.first().value);
    EXPECT_EQ(min_val.value, interval.last().value);

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

    EXPECT_EQ(min_val.value, interval.first().value);
    EXPECT_EQ(min_val.value, interval.last().value);

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

TEST(RingIntervalTest, structured_bindings_less_only)
{
    using R = RingInterval<NoBuiltinOperatorsInt, LessOnly>;
    const R interval;
    const auto & [first, last] = interval;
    EXPECT_EQ(LessOnly::min().value, first.value);
    EXPECT_EQ(LessOnly::min().value, last.value);
}

// Interval default construction and basic operations

TEST(IntervalTest, default_first_last)
{
    const Interval<s32> interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), interval.first());
    EXPECT_EQ(std::numeric_limits<s32>::min(), interval.last());
}

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

TEST(IntervalTest, structured_bindings)
{
    const Interval<s32> interval;
    const auto & [first, last] = interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), first);
    EXPECT_EQ(std::numeric_limits<s32>::min(), last);
}

TEST(IntervalTest, copy_constructible)
{
    const Interval<s32> interval;
    auto copy = interval;
    EXPECT_EQ(interval.first(), copy.first());
    EXPECT_EQ(interval.last(), copy.last());
    EXPECT_EQ(interval.empty(), copy.empty());
}

TEST(IntervalTest, copy_assignable)
{
    const Interval<s32> interval;
    Interval<s32> other;
    other = interval;
    EXPECT_EQ(interval.first(), other.first());
    EXPECT_EQ(interval.last(), other.last());
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

    EXPECT_EQ(min_val.value, interval.first().value);
    EXPECT_EQ(min_val.value, interval.last().value);
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

TEST(IntervalTest, structured_bindings_less_only)
{
    using I = Interval<NoBuiltinOperatorsInt, LessOnly>;
    const I interval;
    const auto & [first, last] = interval;
    EXPECT_EQ(LessOnly::min().value, first.value);
    EXPECT_EQ(LessOnly::min().value, last.value);
}

// std::tuple_size / std::tuple_element / std::get support

TEST(IntervalTupleSupport, tuple_size)
{
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<Interval<s32>>);
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<RingInterval<s32>>);
}

TEST(IntervalTupleSupport, tuple_size_all_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, LessOnly>;
    using R = RingInterval<NoBuiltinOperatorsInt, LessOnly>;
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<I>);
    EXPECT_EQ(size_t { 2u }, std::tuple_size_v<R>);
}

TEST(IntervalTupleSupport, get_interval)
{
    const Interval<s32> interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), std::get<0>(interval));
    EXPECT_EQ(std::numeric_limits<s32>::min(), std::get<1>(interval));
}

TEST(IntervalTupleSupport, get_ring_interval)
{
    const RingInterval<s32> interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), std::get<0>(interval));
    EXPECT_EQ(std::numeric_limits<s32>::min(), std::get<1>(interval));
}

TEST(IntervalTupleSupport, get_all_traits)
{
    using I = Interval<NoBuiltinOperatorsInt, LessOnly>;
    using R = RingInterval<NoBuiltinOperatorsInt, LessOnly>;

    const I interval_i;
    EXPECT_EQ(LessOnly::min().value, std::get<0>(interval_i).value);
    EXPECT_EQ(LessOnly::min().value, std::get<1>(interval_i).value);

    const R interval_r;
    EXPECT_EQ(LessOnly::min().value, std::get<0>(interval_r).value);
    EXPECT_EQ(LessOnly::min().value, std::get<1>(interval_r).value);
}

TEST(IntervalTupleSupport, structured_binding_as_tuple)
{
    const Interval<s32> interval;
    const auto [first, last] = interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), first);
    EXPECT_EQ(std::numeric_limits<s32>::min(), last);
}

TEST(IntervalTupleSupport, structured_binding_ring_as_tuple)
{
    const RingInterval<s32> interval;
    const auto [first, last] = interval;
    EXPECT_EQ(std::numeric_limits<s32>::min(), first);
    EXPECT_EQ(std::numeric_limits<s32>::min(), last);
}

} // namespace ka
