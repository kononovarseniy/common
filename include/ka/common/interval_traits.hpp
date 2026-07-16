#pragma once

#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

#include <ka/common/assert.hpp>

namespace ka
{

/// @brief Interval value type deduced from Traits::min.
template <typename Traits>
    requires std::same_as<std::invoke_result_t<decltype(Traits::min)>, std::invoke_result_t<decltype(Traits::max)>>
using IntervalValueTraitsValueType = std::invoke_result_t<decltype(Traits::min)>;

/// @brief Interval size type deduced from Traits::distance.
template <typename Traits>
using IntervalValueTraitsSizeType = std::invoke_result_t<
    decltype(Traits::distance),
    IntervalValueTraitsValueType<Traits>,
    IntervalValueTraitsValueType<Traits>>;

/// @brief Checks that comparison method is implemented by the Traits type.
template <typename Traits>
concept HasIntervalValueLess = requires(const IntervalValueTraitsValueType<Traits> & value) {
    { Traits::less(value, value) } noexcept -> std::convertible_to<bool>;
};

/// @brief Checks that equality test method is implemented by the Traits type.
template <typename Traits>
concept HasIntervalValueEqual = requires(const IntervalValueTraitsValueType<Traits> & value) {
    { Traits::equal(value, value) } noexcept -> std::convertible_to<bool>;
};

/// @brief Checks that three-way comparison method is implemented by the Traits type.
template <typename Traits>
concept HasIntervalValueCmp = requires(const IntervalValueTraitsValueType<Traits> & value) {
    { Traits::cmp(value, value) } noexcept -> std::convertible_to<std::strong_ordering>;
};

/// @brief Specifies that a type provides either a less predicate or a three-way comparison (cmp) for ordering interval
/// values.
template <typename Traits>
concept ComparableIntervalValueTraits = HasIntervalValueLess<Traits> || HasIntervalValueCmp<Traits>;

/// @brief Specifies that a type implements all the methods required by the Interval class template.
template <typename Traits, typename T>
concept IntervalValueTraitsFor = ComparableIntervalValueTraits<Traits> && requires(const T & value) {
    { Traits::min() } noexcept -> std::same_as<T>;
    { Traits::max() } noexcept -> std::same_as<T>;
    { Traits::prev(value) } noexcept -> std::same_as<T>;
    { Traits::next(value) } noexcept -> std::same_as<T>;
};

/// @brief Specifies that a type implements all the methods required by the Interval class template including method
/// required by Interval::size().
template <typename Traits, typename T>
concept SizedIntervalValueTraitsFor =
    IntervalValueTraitsFor<Traits, T> && requires(const T & value, const IntervalValueTraitsSizeType<Traits> & size) {
        { size + size } noexcept -> std::same_as<IntervalValueTraitsSizeType<Traits>>;
        { size + 1 } noexcept -> std::same_as<IntervalValueTraitsSizeType<Traits>>;
        { Traits::distance(value, value) } noexcept -> std::same_as<IntervalValueTraitsSizeType<Traits>>;
    };

/// @brief Compile time inderection layer for more convenient Traits usage.
template <typename T, IntervalValueTraitsFor<T> Traits>
struct IntervalValueUtils final
{
    /// @brief Smallest allowed value.
    [[nodiscard]] static constexpr T min() noexcept
    {
        return Traits::min();
    }

    /// @brief Largest allowed value.
    [[nodiscard]] static constexpr T max() noexcept
    {
        return Traits::max();
    }

    // TODO: remove?
    /// @brief Largest value strictly less then the value.
    [[nodiscard]] static constexpr T prev(const T & value) noexcept
    {
        KA_PRE(less(min(), value));
        return Traits::prev(value);
    }

    /// @brief Largest value strictly less then the value, and max if the value is min.
    [[nodiscard]] static constexpr T prev_wrap(const T & value) noexcept
    {
        return less(min(), value) ? Traits::prev(value) : max();
    }

    // TODO: remove?
    /// @brief Smalest value strictly greater then the value.
    [[nodiscard]] static constexpr T next(const T & value) noexcept
    {
        KA_PRE(less(value, max()));
        return Traits::next(value);
    }

    /// @brief Smalest value strictly greater then the value, and min if the value is max.
    [[nodiscard]] static constexpr T next_wrap(const T & value) noexcept
    {
        return less(value, max()) ? Traits::next(value) : min();
    }

    /// @brief Returns the number of increments (applications of next) needed to go from first to last.
    /// @pre first <= last.
    [[nodiscard]] static constexpr IntervalValueTraitsSizeType<Traits> distance(
        const T & first,
        const T & last) noexcept
    {
        KA_PRE(less_or_equal(first, last));
        return Traits::distance(first, last);
    }

    /// @brief Checks if lhs < rhs acording to the Traits.
    [[nodiscard]] static constexpr bool less(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueLess<Traits>)
        {
            return Traits::less(lhs, rhs);
        }
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs) < 0;
        }
    }

    /// @brief Checks if lhs <= rhs acording to the Traits.
    [[nodiscard]] static constexpr bool less_or_equal(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueLess<Traits>)
        {
            return !Traits::less(rhs, lhs);
        }
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs) <= 0;
        }
    }

    /// @brief Returns true if elements are equal acording to the Traits.
    [[nodiscard]] static constexpr bool equal(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueEqual<Traits>)
        {
            return Traits::equal(lhs, rhs);
        }
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs) == 0;
        }
        if constexpr (HasIntervalValueLess<Traits>)
        {
            return !Traits::less(lhs, rhs) && !Traits::less(rhs, lhs);
        }
    }

    /// @brief Checks if lhs >= rhs acording to the Traits.
    [[nodiscard]] static constexpr bool greater_or_equal(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueLess<Traits>)
        {
            return !Traits::less(lhs, rhs);
        }
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs) >= 0;
        }
    }

    /// @brief Checks if lhs > rhs acording to the Traits.
    [[nodiscard]] static constexpr bool greater(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueLess<Traits>)
        {
            return Traits::less(rhs, lhs);
        }
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs) > 0;
        }
    }

    /// @brief Returns the three-way comparison result between lhs and rhs.
    [[nodiscard]] static constexpr std::strong_ordering cmp(const T & lhs, const T & rhs) noexcept
    {
        if constexpr (HasIntervalValueCmp<Traits>)
        {
            return Traits::cmp(lhs, rhs);
        }
        if constexpr (HasIntervalValueLess<Traits>)
        {
            if (Traits::less(lhs, rhs))
            {
                return std::strong_ordering::less;
            }
            if (Traits::less(rhs, lhs))
            {
                return std::strong_ordering::greater;
            }
            return std::strong_ordering::equal;
        }
    }

    /// @brief Returns true if the value is inside allowed range of values for given Traits.
    [[nodiscard]] static constexpr bool value_is_valid(const T & value) noexcept
    {
        return !less(value, min()) && !less(max(), value);
    }
};

template <typename T>
struct IntervalValueTraits final
{
    static_assert(false, "IntervalValueTraits<T> must be specialized for type T");
};

template <std::integral T>
struct IntervalValueTraits<T> final
{
    [[nodiscard]] constexpr static T min() noexcept
    {
        return std::numeric_limits<T>::min();
    }

    [[nodiscard]] constexpr static T max() noexcept
    {
        return std::numeric_limits<T>::max();
    }

    [[nodiscard]] constexpr static T prev(const T & value) noexcept
    {
        KA_PRE(value > min());
        KA_PRE(value <= max());

        return value - 1;
    }

    [[nodiscard]] constexpr static T next(const T & value) noexcept
    {
        KA_PRE(value >= min());
        KA_PRE(value < max());

        return value + 1;
    }

    [[nodiscard]] constexpr static bool less(const T & lhs, const T & rhs) noexcept
    {
        KA_PRE(lhs >= min());
        KA_PRE(lhs <= max());
        KA_PRE(rhs >= min());
        KA_PRE(rhs <= max());

        return lhs < rhs;
    }

    [[nodiscard]] constexpr static size_t distance(const T & lhs, const T & rhs) noexcept
        requires(std::numeric_limits<std::make_unsigned_t<T>>::digits < std::numeric_limits<size_t>::digits)
    {
        KA_PRE(lhs >= min());
        KA_PRE(lhs <= max());
        KA_PRE(rhs >= min());
        KA_PRE(rhs <= max());

        KA_PRE(lhs <= rhs);

        return rhs - lhs;
    }
};

} // namespace ka
