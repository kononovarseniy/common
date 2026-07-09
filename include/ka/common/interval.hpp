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
/// * Normal (first < last):
///       half-open interval [first, last).
///
/// * Empty (first == last and first == Traits::min()):
///       empty set.
///
/// * Full (first == last and first == Traits::max()):
///       full range of values [Traits::min(), Traits::max()]
///
/// * Discontinuous (first > last):
///       [Traits::min(), last) U [first, Traits::max()) which is the complement of a half-open interval.
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
        : RingInterval(Utils::min(), Utils::min())
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
        return Utils::less_or_equal(first_, last_);
    }

    [[nodiscard]] constexpr bool contains(const T & value) const noexcept
    {
        KA_PRE(valid());

        if (continuous())
        {
            return Utils::less_or_equal(first_, value) && Utils::less(value, last_);
        }
        else
        {
            return Utils::less(value, last_) || Utils::less_or_equal(first_, value);
        }
    }

    [[nodiscard]] constexpr auto size() const noexcept
        requires SizedIntervalValueTraitsFor<Traits, T>
    {
        KA_PRE(valid());

        if (continuous())
        {
            return Utils::distance(first_, last_);
        }
        else
        {
            return Utils::distance(Utils::min(), last_) + Utils::distance(first_, Utils::max()) + 1;
        }
    }

    [[nodiscard]] constexpr RingInterval complement() const noexcept
    {
        KA_PRE(valid());
        return RingInterval { last_, first_ };
    }

    // See TODO.md

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::value_inside_allowed_range(first_))
        {
            return false;
        }
        if (!Utils::value_inside_allowed_range(last_))
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
    T first_;
    T last_;
};

/// @brief A half-open interval [first, last) over a totally ordered type T.
/// @note Set intersection is closed over the type.
/// @details
/// The interval can be in one of three forms:
/// * Normal (first < last):
///       half-open interval [first, last).
///
/// * Empty (first == last and first == Traits::min()):
///       empty set.
///
/// * Full (first == last and first == Traits::max()):
///       full range of values [Traits::min(), Traits::max()]
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

    // See TODO.md

private:
    /// @brief Should always return true.
    [[nodiscard]] constexpr bool valid() const noexcept
    {
        if (!Utils::value_inside_allowed_range(first_))
        {
            return false;
        }
        if (!Utils::value_inside_allowed_range(last_))
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
