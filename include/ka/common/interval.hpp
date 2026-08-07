/// @brief Intervals over a totally ordered discrete type.
/// @details
/// This module contains two classes \c Interval and \c RingInterval.
/// Both represent subsets of the full value range [Traits::min(), Traits::max()].
///
/// \c Interval represents an arbitrary continuous sub-interval, including empty and full ranges.
/// It is closed over set intersection but not over set complement.
///
/// \c RingInterval represents either a continuous sub-interval (like \c Interval) or the complement of one.
/// A complement consists of two non-adjacent sub-intervals: one part including Traits::min() and the other
/// including Traits::max().
/// It is closed over set complement but not over set intersection.
///
/// The exact endpoint semantics (open vs. closed) and the internal representation are implementation details.
/// The observable behavior of any interval is fully determined by its construction functions and its conversion
/// to \c ClosedInterval (via to_closed_interval() or to_closed_intervals()).
///
/// There is no class \c ComplementInterval that would be other subset of \c RingInterval and be closed over set
/// union. The \c ComplementInterval should be prety easy to simulate using the set complement of set intersection of
/// \c Interval instances.
///
/// Implementation details:
///
/// Let T be a totally ordered type with discrete values, that includes its infimum (min) and supremum (max) values.
/// Such as fixed-width integer types or floating-points types without negative zero and NaNs.
/// Let first and last be two instances of T.
/// We can define several cases:
/// * first < last: naturaly this case represents half-open interval [first, last);
/// * first > last: can be used to represent complement of any half-open interval which is [min, last) U [first, max].
/// * first == last: any of these values can represent either empty or full set, we choose
/// first = last = max to represent the empty set, and
/// first = last = min to represent the full set.
/// This seems to allow better use of modular arithmetics.
/// All these special values are considered implementation detail. All public interfaces should treat the case
/// first = last as an empty set, like most C++ libraries do.

#pragma once

#include <concepts>
#include <limits>
#include <optional>
#include <type_traits>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/closed_interval.hpp>
#include <ka/common/hash.hpp>
#include <ka/common/interval_traits.hpp>

namespace ka
{

template <typename T, IntervalValueTraitsFor<T> Traits>
class Interval;

/// @brief A subset of [Traits::min(), Traits::max()], either continuous sub-interval or its complement.
/// @note Set complement is closed over the type.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class RingInterval final
{
    using Utils = IntervalValueUtils<T, Traits>;

    friend class Interval<T, Traits>;

private:
    constexpr RingInterval(const T & first, const T & last) noexcept
        : first_ { first }
        , last_ { last }
    {
        KA_PRE(valid());
    }

public:
    /// @brief Constructs empty interval.
    constexpr RingInterval() noexcept
        : RingInterval(Utils::max(), Utils::max())
    {
        KA_PRE(valid());
    }

    constexpr RingInterval(const RingInterval &) noexcept = default;
    constexpr RingInterval & operator=(const RingInterval &) noexcept = default;
    constexpr RingInterval(RingInterval &&) noexcept = default;
    constexpr RingInterval & operator=(RingInterval &&) noexcept = default;

    /// @brief Returns true if two ring intervals are equal.
    [[nodiscard]] constexpr bool operator==(const RingInterval & other) const noexcept
    {
        return Utils::equal(first_, other.first_) && Utils::equal(last_, other.last_);
    }

    /// @brief Returns true if two ring intervals are not equal.
    [[nodiscard]] constexpr bool operator!=(const RingInterval & other) const noexcept
    {
        return !(*this == other);
    }

    /// @brief Feeds the interval into the hasher.
    void hash(Hasher & hasher) const noexcept
    {
        KA_ASSERT(valid());
        hasher.update(first_);
        hasher.update(last_);
    }

public:
    /// @brief Returns true if the interval is empty.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && Utils::is_max(first_);
    }

    /// @brief Returns true if the interval contains all possible values.
    [[nodiscard]] constexpr bool full() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && !Utils::is_max(first_);
    }

    /// @brief Returns true if the interval is a single continuous range (including empty and full).
    [[nodiscard]] constexpr bool continuous() const noexcept
    {
        KA_PRE(valid());
        return Utils::less_or_equal(first_, last_) || Utils::is_min(last_);
    }

    [[nodiscard]] constexpr bool contains(const T & value) const noexcept
    {
        KA_PRE(valid());
        KA_PRE(Utils::is_valid(value));

        const auto cmp = Utils::cmp(first_, last_);
        if (cmp < 0)
        {
            return Utils::less_or_equal(first_, value) && Utils::less(value, last_);
        }
        if (cmp > 0)
        {
            return Utils::less(value, last_) || Utils::less_or_equal(first_, value);
        }
        return Utils::is_min(first_);
    }

    [[nodiscard]] constexpr auto size() const noexcept
        requires SizedIntervalValueTraitsFor<Traits, T>
    {
        KA_PRE(valid());

        const auto cmp = Utils::cmp(first_, last_);
        if (cmp < 0)
        {
            return Utils::distance(first_, last_);
        }
        else if (cmp == 0 && Utils::is_max(first_))
        {
            return IntervalValueTraitsSizeType<Traits> {};
        }
        KA_ASSERT(cmp >= 0);
        return Utils::distance(Utils::min(), last_) + Utils::distance(first_, Utils::max()) + 1;
    }

    /// @brief Converts the ring interval to zero, one or two closed intervals.
    /// @details
    /// Empty interval converts to an empty set (zero intervals).
    /// Full interval and continuous non-empty intervals convert to a single closed interval.
    /// Two-interval form converts to two closed intervals.
    [[nodiscard]] constexpr MaybeTwoClosedIntervals<T, Traits> to_closed_intervals() const noexcept
    {
        KA_PRE(valid());
        const auto cmp = Utils::cmp(first_, last_);
        if (cmp < 0)
        {
            // Normal: [first, last) -> [first, last-1].
            return MaybeTwoClosedIntervals(ClosedInterval<T, Traits>(first_, Utils::prev(last_)));
        }
        if (cmp > 0)
        {
            // Discontinuous: [min, last) U [first, max].
            const auto right = ClosedInterval<T, Traits>(first_, Utils::max());
            if (Utils::is_min(last_))
            {
                // Left part is empty.
                return MaybeTwoClosedIntervals<T, Traits>(right);
            }
            // Left part: [min, last) -> [min, last-1].
            const auto left = ClosedInterval<T, Traits>(Utils::min(), Utils::prev(last_));
            return MaybeTwoClosedIntervals<T, Traits>(left, right);
        }
        if (empty())
        {
            return MaybeTwoClosedIntervals<T, Traits>();
        }
        return MaybeTwoClosedIntervals(ClosedInterval<T, Traits>(Utils::min(), Utils::max()));
    }

    /// @brief Converts this ring interval to a single interval if possible.
    /// @details Returns std::nullopt when the ring interval is discontinuous.
    [[nodiscard]] constexpr std::optional<Interval<T, Traits>> to_interval() const noexcept
    {
        KA_PRE(valid());
        if (continuous())
        {
            return Interval<T, Traits>(first_, last_);
        }
        return std::nullopt;
    }

    /// @brief Converts the ring interval to a pair of intervals.
    /// @details
    /// For discontinuous ring intervals, returns the left and right parts as a pair of non-empty intervals.
    /// For continuous ring intervals (including empty and full), returns the interval as the first element and
    /// an empty interval as the second element.
    [[nodiscard]] constexpr std::pair<Interval<T, Traits>, Interval<T, Traits>> to_intervals() const noexcept
    {
        KA_PRE(valid());
        if (continuous())
        {
            return { Interval<T, Traits> { first_, last_ }, Interval<T, Traits> {} };
        }
        return { Interval<T, Traits> { Utils::min(), last_ }, Interval<T, Traits> { first_, Utils::min() } };
    }

    /// @brief Returns the complement of this interval.
    [[nodiscard]] constexpr RingInterval complement() const noexcept
    {
        KA_PRE(valid());
        if (!Utils::equal(first_, last_))
        {
            return RingInterval { last_, first_ };
        }
        return Utils::is_min(first_)
                   ? RingInterval { Utils::max(), Utils::max() }
                   : RingInterval { Utils::min(), Utils::min() };
    }

    /// @brief Returns an empty interval.
    [[nodiscard]] static constexpr RingInterval make_empty() noexcept
    {
        return RingInterval(Utils::max(), Utils::max());
    }

    /// @brief Returns a full interval containing all values.
    [[nodiscard]] static constexpr RingInterval make_full() noexcept
    {
        return RingInterval(Utils::min(), Utils::min());
    }

    /// @brief Returns a half-open interval [first, last).
    /// @note Returns empty range when first == last.
    /// @pre first <= last
    [[nodiscard]] static constexpr RingInterval make_half_open(const T & first, const T & last) noexcept
    {
        KA_PRE(Utils::is_valid(first));
        KA_PRE(Utils::is_valid(last));
        KA_PRE(Utils::less_or_equal(first, last));
        if (Utils::equal(first, last))
        {
            return make_empty();
        }
        return RingInterval(first, last);
    }

    /// @brief Returns the complement of the half-open interval [first, last).
    /// @note Returns full range when first == last.
    /// @pre first <= last
    [[nodiscard]] static constexpr RingInterval make_half_open_complement(const T & first, const T & last) noexcept
    {
        KA_PRE(Utils::is_valid(first));
        KA_PRE(Utils::is_valid(last));
        KA_PRE(Utils::less_or_equal(first, last));
        if (Utils::equal(first, last))
        {
            return make_full();
        }
        return RingInterval(last, first);
    }

    /// @brief Returns a closed interval [first, last].
    /// @pre first <= last
    [[nodiscard]] static constexpr RingInterval make_closed(const T & first, const T & last) noexcept
    {
        KA_PRE(Utils::is_valid(first));
        KA_PRE(Utils::is_valid(last));
        KA_PRE(Utils::less_or_equal(first, last));
        return RingInterval(first, Utils::next_wrap(last));
    }

    /// @brief Returns a closed interval from a ClosedInterval.
    [[nodiscard]] static constexpr RingInterval make_closed(const ClosedInterval<T, Traits> & interval) noexcept
    {
        return make_closed(interval.first(), interval.last());
    }

    /// @brief Returns an interval of all values strictly less than value.
    [[nodiscard]] static constexpr RingInterval make_less(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));

        if (!Utils::is_min(value))
        {
            return RingInterval(Utils::min(), value);
        }
        return make_empty();
    }

    /// @brief Returns an interval of all values less than or equal to value.
    [[nodiscard]] static constexpr RingInterval make_less_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        // If the value is max() we would get a full range represented as first == last == min().
        return RingInterval(Utils::min(), Utils::next_wrap(value));
    }

    /// @brief Returns an interval containing only value.
    [[nodiscard]] static constexpr RingInterval make_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return RingInterval(value, Utils::next_wrap(value));
    }

    /// @brief Returns an interval containing all values except given value.
    [[nodiscard]] static constexpr RingInterval make_not_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return RingInterval(Utils::next_wrap(value), value);
    }

    /// @brief Returns an interval of all values greater than or equal to value.
    [[nodiscard]] static constexpr RingInterval make_greater_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return RingInterval(value, Utils::min());
    }

    /// @brief Returns an interval of all values strictly greater than value.
    [[nodiscard]] static constexpr RingInterval make_greater(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        if (!Utils::is_max(value))
        {
            return RingInterval(Utils::next(value), Utils::min());
        }
        return make_empty();
    }

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::is_valid(first_))
        {
            return false;
        }
        if (!Utils::is_valid(last_))
        {
            return false;
        }
        if (!Utils::equal(first_, last_))
        {
            return true;
        }
        return Utils::is_min(first_) || Utils::is_max(first_);
    }

private:
    // Special ranges encoded with first_ == last_.
    // Empty: first == last == Traits::min()
    // Full: first == last == Traits::max()
    T first_;
    T last_;
};

/// @brief An arbitrary continuous sub-interval of [Traits::min(), Traits::max()].
/// @note Set intersection is closed over the type.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class Interval final
{
    using Utils = IntervalValueUtils<T, Traits>;

    friend class RingInterval<T, Traits>;

private:
    constexpr Interval(const T & first, const T & last) noexcept
        : first_ { first }
        , last_ { last }
    {
        KA_PRE(valid());
    }

public:
    /// @brief Constructs empty interval.
    constexpr Interval() noexcept
        : Interval(Utils::max(), Utils::max())
    {
        KA_PRE(valid());
    }

    constexpr Interval(const Interval &) noexcept = default;
    constexpr Interval & operator=(const Interval &) noexcept = default;
    constexpr Interval(Interval &&) noexcept = default;
    constexpr Interval & operator=(Interval &&) noexcept = default;

    /// @brief Returns true if two intervals are equal.
    [[nodiscard]] constexpr bool operator==(const Interval & other) const noexcept
    {
        return Utils::equal(first_, other.first_) && Utils::equal(last_, other.last_);
    }

    /// @brief Returns true if two intervals are not equal.
    [[nodiscard]] constexpr bool operator!=(const Interval & other) const noexcept
    {
        return !(*this == other);
    }

    /// @brief Feeds the interval into the hasher.
    void hash(Hasher & hasher) const noexcept
    {
        KA_ASSERT(valid());
        hasher.update(first_);
        hasher.update(last_);
    }

public:
    /// @brief Returns true if the interval is empty.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && Utils::is_max(first_);
    }

    /// @brief Returns true if the interval contains all possible values.
    [[nodiscard]] constexpr bool full() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && !Utils::is_max(first_);
    }

    [[nodiscard]] constexpr bool contains(const T & value) const noexcept
    {
        KA_PRE(valid());
        KA_PRE(Utils::is_valid(value));
        if (Utils::greater(first_, value))
        {
            return false;
        }
        return Utils::is_min(last_) || Utils::less(value, last_);
    }

    [[nodiscard]] constexpr auto size() const noexcept
        requires SizedIntervalValueTraitsFor<Traits, T>
    {
        KA_PRE(valid());
        if (empty())
        {
            return IntervalValueTraitsSizeType<Traits> {};
        }
        if (full())
        {
            return Utils::distance(Utils::min(), Utils::max()) + IntervalValueTraitsSizeType<Traits> { 1 };
        }
        if (Utils::less(first_, last_))
        {
            return Utils::distance(first_, last_);
        }
        // Max-inclusive: [first, Traits::max()].
        return Utils::distance(first_, Utils::max()) + IntervalValueTraitsSizeType<Traits> { 1 };
    }

    /// @brief Converts the interval to a closed interval.
    /// @details
    /// Empty interval converts to std::nullopt.
    /// Non-empty intervals convert to a single closed interval covering the same values.
    [[nodiscard]] constexpr std::optional<ClosedInterval<T, Traits>> to_closed_interval() const noexcept
    {
        KA_PRE(valid());
        if (empty())
        {
            return std::nullopt;
        }
        return ClosedInterval<T, Traits>(first_, Utils::prev_wrap(last_));
    }

    /// @brief Converts this interval to a ring interval.
    /// @details The internal representation is identical, so this is a trivial copy.
    [[nodiscard]] constexpr RingInterval<T, Traits> to_ring_interval() const noexcept
    {
        KA_PRE(valid());
        return RingInterval<T, Traits>(first_, last_);
    }

    /// @brief Returns the intersection of this interval with another.
    [[nodiscard]] constexpr Interval intersect(const Interval & other) const noexcept
    {
        KA_PRE(valid());
        KA_PRE(other.valid());

        if (empty() || other.empty())
        {
            return make_empty();
        }

        const T start = Utils::greater(first_, other.first_) ? first_ : other.first_;

        // The exclusive end: min() as last_ means max-inclusive (treated as "largest end").
        const T end =
            Utils::is_min(last_)         ? other.last_
            : Utils::is_min(other.last_) ? last_
            : Utils::less(last_, other.last_)
                ? last_
                : other.last_;

        if (!Utils::is_min(end) && Utils::greater_or_equal(start, end))
        {
            return make_empty();
        }
        return Interval(start, end);
    }

    /// @brief Returns an empty interval.
    [[nodiscard]] static constexpr Interval make_empty() noexcept
    {
        return Interval(Utils::max(), Utils::max());
    }

    /// @brief Returns a full interval containing all values.
    [[nodiscard]] static constexpr Interval make_full() noexcept
    {
        return Interval(Utils::min(), Utils::min());
    }

    /// @brief Returns a half-open interval [first, last).
    /// @note Returns empty range when first == last.
    /// @pre first <= last
    [[nodiscard]] static constexpr Interval make_half_open(const T & first, const T & last) noexcept
    {
        KA_PRE(Utils::is_valid(first));
        KA_PRE(Utils::is_valid(last));
        KA_PRE(Utils::less_or_equal(first, last));
        if (Utils::equal(first, last))
        {
            return make_empty();
        }
        return Interval(first, last);
    }

    /// @brief Returns a closed interval [first, last].
    /// @pre first <= last
    [[nodiscard]] static constexpr Interval make_closed(const T & first, const T & last) noexcept
    {
        KA_PRE(Utils::is_valid(first));
        KA_PRE(Utils::is_valid(last));
        KA_PRE(Utils::less_or_equal(first, last));
        return Interval(first, Utils::next_wrap(last));
    }

    /// @brief Returns a closed interval from a ClosedInterval.
    [[nodiscard]] static constexpr Interval make_closed(const ClosedInterval<T, Traits> & interval) noexcept
    {
        return make_closed(interval.first(), interval.last());
    }

    /// @brief Returns an interval of all values strictly less than value.
    [[nodiscard]] static constexpr Interval make_less(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        if (!Utils::is_min(value))
        {
            return Interval(Utils::min(), value);
        }
        return make_empty();
    }

    /// @brief Returns an interval of all values less than or equal to value.
    [[nodiscard]] static constexpr Interval make_less_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return Interval(Utils::min(), Utils::next_wrap(value));
    }

    /// @brief Returns an interval containing only value.
    [[nodiscard]] static constexpr Interval make_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return Interval(value, Utils::next_wrap(value));
    }

    /// @brief Returns an interval of all values greater than or equal to value.
    [[nodiscard]] static constexpr Interval make_greater_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        return Interval(value, Utils::min());
    }

    /// @brief Returns an interval of all values strictly greater than value.
    [[nodiscard]] static constexpr Interval make_greater(const T & value) noexcept
    {
        KA_PRE(Utils::is_valid(value));
        if (Utils::is_max(value))
        {
            return make_empty();
        }
        return Interval(Utils::next(value), Utils::min());
    }

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::is_valid(first_))
        {
            return false;
        }
        if (!Utils::is_valid(last_))
        {
            return false;
        }
        if (!Utils::less_or_equal(first_, last_))
        {
            // Max-inclusive: first > last only allowed when last == min.
            return Utils::is_min(last_);
        }
        if (!Utils::equal(first_, last_))
        {
            return true;
        }
        return Utils::is_min(first_) || Utils::is_max(first_);
    }

private:
    // Special ranges encoded with first_ == last_.
    // Empty: first == last == Traits::max()
    // Full: first == last == Traits::min()
    T first_;
    T last_;
};

/// @brief Returns true if a ring interval and an interval are equal.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] constexpr bool operator==(const RingInterval<T, Traits> & lhs, const Interval<T, Traits> & rhs) noexcept
{
    return lhs == rhs.to_ring_interval();
}

/// @brief Returns true if a ring interval and an interval are not equal.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] constexpr bool operator!=(const RingInterval<T, Traits> & lhs, const Interval<T, Traits> & rhs) noexcept
{
    return !(lhs == rhs);
}

/// @brief Returns true if an interval and a ring interval are equal.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] constexpr bool operator==(const Interval<T, Traits> & lhs, const RingInterval<T, Traits> & rhs) noexcept
{
    return lhs.to_ring_interval() == rhs;
}

/// @brief Returns true if an interval and a ring interval are not equal.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] constexpr bool operator!=(const Interval<T, Traits> & lhs, const RingInterval<T, Traits> & rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace ka
