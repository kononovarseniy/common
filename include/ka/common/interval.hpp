/// @brief Intervals defined by two numbers on the ring.
/// @details
/// This module contains two classes \c RingInterval and \c Interval.
/// The set of intervals represented by the \c RingInterval
/// is the superset of the set of intervals represented by the \c Interval.
/// \c RingInterval is closed over set complement and not closed over set intersection.
/// \c Interval is closed over set intersection and not closed over set complement (almost no support other than for
/// empty and full intervals).
/// There is no class \c ComplementInterval that would be other superset of \c RingInterval and be closed over set
/// union. The \c ComplementInterval should be prety easy to simulate using the set complement of set intersection of
/// \c Interval instances.
///
/// Let T be a totally ordered type with discrete values, that includes its infimum (min) and supremum (max) values.
/// Such as fixed-width integer types or floating-points types without negative zero and NaNs.
/// Let first and last be two instances of T.
/// We can define several cases:
/// * first < last: naturaly this case represents half-open interval [first, last);
/// * first > last: can be used to represent complement of any half-open interval which is [min, last) U [first, max].
/// * first == last: any of these values can represent either empty or full set, we choose first=last=min to represent
/// the empty set, and first=last=max to represent the full set. This seems to allow better use of modular arithmetics.
///
/// @note All these special values are considered implementation detail. All public interfaces should treat the case
/// first=last as an empty set, like most C++ libraries do.

#pragma once

#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

#include <ka/common/assert.hpp>
#include <ka/common/interval_traits.hpp>

namespace ka
{

/// @brief A half-open interval over a totally ordered type T, with cyclic (wrap‑around) order semantics.
/// @note Set complement is closed over the type.
/// @details
/// The interval can be in one of four forms:
/// * Normal (first < last): half-open interval [first, last).
/// * Empty: empty set
/// * Full: full range of values [Traits::min(), Traits::max()]
/// * Discontinuous (first > last): [Traits::min(), last) U [first, Traits::max()) which is the complement of a
/// half-open interval.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class RingInterval final
{
    using Utils = IntervalValueUtils<T, Traits>;

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

public:
    /// @brief Returns the inclusive left endpoint of the half-open interval.
    /// @note Structured binding is also supported.
    [[nodiscard]] constexpr const T & first() const noexcept
    {
        KA_PRE(valid());
        return first_;
    }

    /// @brief Returns the exclusive right endpoint of the half-open interval.
    /// @note Structured binding is also supported.
    [[nodiscard]] constexpr const T & last() const noexcept
    {
        KA_PRE(valid());
        return last_;
    }

    /// @brief Returns true if the interval is empty.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && Utils::equal(first_, Utils::min());
    }

    /// @brief Returns true if the interval contains all possible values.
    [[nodiscard]] constexpr bool full() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && !Utils::equal(first_, Utils::min());
    }

    /// @brief Returns true if has the form [first, last), including full and empty,
    /// and false if it has the form [Traits::min(), last) U [first, Traits::max()]
    [[nodiscard]] constexpr bool continuous() const noexcept
    {
        KA_PRE(valid());
        return Utils::less_or_equal(first_, last_) || Utils::equal(last_, Utils::min());
    }

    [[nodiscard]] constexpr bool contains(const T & value) const noexcept
    {
        KA_PRE(valid());
        KA_PRE(Utils::value_is_valid(value));

        const auto cmp = Utils::cmp(first_, last_);
        if (cmp < 0)
        {
            return Utils::less_or_equal(first_, value) && Utils::less(value, last_);
        }
        if (cmp > 0)
        {
            return Utils::less(value, last_) || Utils::less_or_equal(first_, value);
        }
        // When all values are valid this line is equivalent to first_ != min (full range).
        return Utils::less(Utils::min(), first_);
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
        else if (cmp == 0 && Utils::equal(first_, Utils::max()))
        {
            return IntervalValueTraitsSizeType<Traits> {};
        }
        KA_ASSERT(cmp >= 0);
        return Utils::distance(Utils::min(), last_) + Utils::distance(first_, Utils::max()) + 1;
    }

    [[nodiscard]] constexpr RingInterval complement() const noexcept
    {
        KA_PRE(valid());
        if (!Utils::equal(first_, last_))
        {
            return RingInterval { last_, first_ };
        }
        return Utils::equal(first_, Utils::min())
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
        KA_PRE(Utils::less_or_equal(first, last));
        if (Utils::equal(first, last))
        {
            return make_full();
        }
        return RingInterval(last, first);
    }

    /// @brief Returns an interval of all values strictly less than value.
    [[nodiscard]] static constexpr RingInterval make_less(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));

        if (Utils::less(Utils::min(), value))
        {
            return RingInterval(Utils::min(), value);
        }
        return make_empty();
    }

    /// @brief Returns an interval of all values less than or equal to value.
    [[nodiscard]] static constexpr RingInterval make_less_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));
        // If the value is max() we would get a full range represented as first == last == min().
        return RingInterval(Utils::min(), Utils::next_wrap(value));
    }

    /// @brief Returns an interval containing only value.
    [[nodiscard]] static constexpr RingInterval make_equal(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));
        return RingInterval(value, Utils::next_wrap(value));
    }

    /// @brief Returns an interval containing all values except given value.
    [[nodiscard]] static constexpr RingInterval make_not_equal(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));
        return RingInterval(Utils::next_wrap(value), value);
    }

    /// @brief Returns an interval of all values greater than or equal to value.
    [[nodiscard]] static constexpr RingInterval make_greater_or_equal(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));
        return RingInterval(value, Utils::min());
    }

    /// @brief Returns an interval of all values strictly greater than value.
    [[nodiscard]] static constexpr RingInterval make_greater(const T & value) noexcept
    {
        KA_PRE(Utils::value_is_valid(value));
        if (Utils::less(value, Utils::max()))
        {
            return RingInterval(Utils::next(value), Utils::min());
        }
        return make_empty();
    }

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::value_is_valid(first_))
        {
            return false;
        }
        if (!Utils::value_is_valid(last_))
        {
            return false;
        }
        if (!Utils::equal(first_, last_))
        {
            return true;
        }
        return Utils::equal(first_, Utils::min()) || Utils::equal(first_, Utils::max());
    }

private:
    // Special ranges encoded with first_ == last_.
    // Empty: first == last == Traits::min()
    // Full: first == last == Traits::max()
    T first_;
    T last_;
};

/// @brief A half-open interval [first, last) over a totally ordered type T.
/// @note Set intersection is closed over the type.
/// @details
/// The interval can be in one of three forms:
/// * Normal (first < last): half-open interval [first, last).
/// * Empty: empty set
/// * Full: full range of values [Traits::min(), Traits::max()]
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class Interval final
{
    using Utils = IntervalValueUtils<T, Traits>;

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
        : Interval(Utils::min(), Utils::min())
    {
        KA_PRE(valid());
    }

    constexpr Interval(const Interval &) noexcept = default;
    constexpr Interval & operator=(const Interval &) noexcept = default;
    constexpr Interval(Interval &&) noexcept = default;
    constexpr Interval & operator=(Interval &&) noexcept = default;

public:
    /// @brief Returns the inclusive left endpoint of the half-open interval.
    /// @note Structured binding is also supported.
    [[nodiscard]] constexpr const T & first() const noexcept
    {
        KA_PRE(valid());
        return first_;
    }

    /// @brief Returns the exclusive right endpoint of the half-open interval.
    /// @note Structured binding is also supported.
    [[nodiscard]] constexpr const T & last() const noexcept
    {
        KA_PRE(valid());
        return last_;
    }

    /// @brief Returns true if the interval is empty.
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && Utils::equal(first_, Utils::min());
    }

    /// @brief Returns true if the interval contains all possible values.
    [[nodiscard]] constexpr bool full() const noexcept
    {
        KA_PRE(valid());
        return Utils::equal(first_, last_) && !Utils::equal(first_, Utils::min());
    }

    [[nodiscard]] constexpr bool contains(const T & value) const noexcept
    {
        KA_PRE(valid());
        return Utils::less_or_equal(first_, value) && Utils::less(value, last_);
    }

    [[nodiscard]] constexpr auto size() const noexcept
        requires SizedIntervalValueTraitsFor<Traits, T>
    {
        KA_PRE(valid());
        return Utils::distance(first_, last_);
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
        KA_PRE(Utils::less_or_greater(first, last));
        if (Utils::equal(first, last))
        {
            return make_empty();
        }
        return Interval(first, last);
    }

    /// @brief Returns an interval of all values strictly less than value.
    [[nodiscard]] static constexpr Interval make_less(const T & value) noexcept
    {
        // Empty range is handled naturaly.
        return Interval(Utils::min(), value);
    }

    /// @brief Returns an interval of all values less than or equal to value.
    [[nodiscard]] static constexpr Interval make_less_or_equal(const T & value) noexcept
    {
        if (Utils::equal(value, Utils::max()))
        {
            return make_full();
        }
        return Interval(Utils::min(), Utils::next(value));
    }

    /// @brief Returns an interval containing only value.
    [[nodiscard]] static constexpr Interval make_equal(const T & value) noexcept
    {
        // TODO: Fix value == max().
        return Interval(value, Utils::next(value));
    }

    /// @brief Returns an interval of all values greater than or equal to value.
    /// @note When value == Traits::min(), returns full. For other values, max() is excluded from the result.
    [[nodiscard]] static constexpr Interval make_greater_or_equal(const T & value) noexcept
    {
        if (Utils::equal(value, Utils::min()))
        {
            return make_full();
        }
        return Interval(value, Utils::max());
    }

    /// @brief Returns an interval of all values strictly greater than value.
    [[nodiscard]] static constexpr Interval make_greater(const T & value) noexcept
    {
        if (Utils::equal(value, Utils::max()))
        {
            return make_empty();
        }
        return Interval(Utils::next(value), Utils::max());
    }

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::value_is_valid(first_))
        {
            return false;
        }
        if (!Utils::value_is_valid(last_))
        {
            return false;
        }
        if (!Utils::less_or_equal(first_, last_))
        {
            return false;
        }
        if (!Utils::equal(first_, last_))
        {
            return true;
        }
        return Utils::equal(first_, Utils::min()) || Utils::equal(first_, Utils::max());
    }

private:
    // Special ranges encoded with first_ == last_.
    // Empty: first == last == Traits::min()
    // Full: first == last == Traits::max()
    T first_;
    T last_;
};

} // namespace ka

namespace std
{

template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_size<::ka::Interval<T, Traits>> : ::std::integral_constant<size_t, 2>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_element<I, ::ka::Interval<T, Traits>> : ::std::tuple_element<I, ::std::pair<T, T>>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
const std::tuple_element_t<I, ::ka::Interval<T, Traits>> & get(const ::ka::Interval<T, Traits> & interval)
{
    static_assert(I < 2);
    if constexpr (I == 0)
    {
        return interval.first();
    }
    if constexpr (I == 1)
    {
        return interval.last();
    }
};

template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_size<::ka::RingInterval<T, Traits>> : ::std::integral_constant<size_t, 2>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_element<I, ::ka::RingInterval<T, Traits>> : ::std::tuple_element<I, ::std::pair<T, T>>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
const std::tuple_element_t<I, ::ka::RingInterval<T, Traits>> & get(const ::ka::RingInterval<T, Traits> & interval)
{
    static_assert(I < 2);
    if constexpr (I == 0)
    {
        return interval.first();
    }
    if constexpr (I == 1)
    {
        return interval.last();
    }
};

} // namespace std
