#pragma once

#include <ka/common/assert.hpp>
#include <ka/common/hash.hpp>
#include <ka/common/interval_traits.hpp>

namespace ka
{

/// @brief Immutable closed interval without any special cases.
/// @note Empty set cannot be represented by this class, but you may use std::optional.
/// @details This class is intended to be used to inspect intervals with more complex internal structure.
/// It intentionally does not provide any set operations.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class ClosedInterval final
{
    using Utils = IntervalValueUtils<T, Traits>;

public:
    /// @brief Creates a valid closed interval.
    /// @pre first <= last and both are valid according to the traits.
    constexpr ClosedInterval(const T & first, const T & last) noexcept
        : first_ { first }
        , last_ { last }
    {
        // More detailed assertions for detailed error reports.
        KA_PRE(Utils::value_is_valid(first_));
        KA_PRE(Utils::value_is_valid(last_));
        KA_PRE(Utils::less_or_equal(first_, last_));
    }

    /// @brief Inclusive start endpoint of the interval.
    [[nodiscard]] constexpr const T & first() const noexcept
    {
        KA_ASSERT(is_valid());
        return first_;
    }

    /// @brief Inclusive finish endpoint of the interval.
    [[nodiscard]] constexpr const T & last() const noexcept
    {
        KA_ASSERT(is_valid());
        return last_;
    }

    [[nodiscard]] constexpr bool operator==(const ClosedInterval &) const noexcept = default;

    /// @brief Feeds the interval endpoints into the hasher.
    void hash(Hasher & hasher) const noexcept
    {
        KA_ASSERT(is_valid());
        hasher.update(first_);
        hasher.update(last_);
    }

private:
    /// @brief Should always be true.
    [[nodiscard]] constexpr bool is_valid() const noexcept
    {
        return Utils::value_is_valid(first_) && Utils::value_is_valid(last_) && Utils::less_or_equal(first_, last_);
    }

private:
    T first_;
    T last_;
};

/// @brief Represents zero, one or two non-overlapping non-adjacent ordered closed intervals.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class MaybeTwoClosedIntervals final
{
    using Utils = IntervalValueUtils<T, Traits>;

public:
    /// @brief Default-constructs an empty interval set.
    MaybeTwoClosedIntervals() noexcept
        : count_ { 0 }
        , intervals_ { default_interval(), default_interval() }
    {
    }

    /// @brief Constructs a single-interval set.
    explicit MaybeTwoClosedIntervals(const ClosedInterval<T, Traits> & interval) noexcept
        : count_ { 1 }
        , intervals_ { interval, default_interval() }
    {
    }

    /// @brief Constructs a two-interval set with the left interval strictly before the right.
    /// @pre left.last() < right.first().
    MaybeTwoClosedIntervals(const ClosedInterval<T, Traits> & left, const ClosedInterval<T, Traits> & right) noexcept
        : count_ { 2 }
        , intervals_ { left, right }
    {
        KA_PRE(Utils::less(left.last(), right.first()));
    }

    /// @brief Returns the number of intervals (0, 1 or 2).
    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return count_;
    }

    /// @brief Iterator to the first interval.
    [[nodiscard]] constexpr const ClosedInterval<T, Traits> * begin() const noexcept
    {
        return intervals_.cbegin();
    }

    /// @brief Past-the-end iterator.
    [[nodiscard]] constexpr const ClosedInterval<T, Traits> * end() const noexcept
    {
        KA_ASSERT(count_ <= intervals_.size());
        return intervals_.cbegin() + count_;
    }

    [[nodiscard]] constexpr bool operator==(const MaybeTwoClosedIntervals &) const noexcept = default;

    /// @brief Feeds the interval set into the hasher.
    void hash(Hasher & hasher) const noexcept
    {
        hasher.update(count_);
        for (const auto & interval : *this)
        {
            hasher.update(interval);
        }
    }

private:
    /// @brief ClosedInterval is not default constructible but we need to fill unused elements of the array with some
    /// values. Easiest way is to pick some valid interval.
    [[nodiscard]] static constexpr ClosedInterval<T, Traits> default_interval() noexcept
    {
        return { Utils::min(), Utils::max() };
    }

private:
    size_t count_ = 0;
    std::array<ClosedInterval<T, Traits>, 2> intervals_;
};

} // namespace ka

namespace std
{

template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_size<::ka::ClosedInterval<T, Traits>> : ::std::tuple_size<std::pair<T, T>>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_element<I, ::ka::ClosedInterval<T, Traits>> : ::std::tuple_element<I, std::pair<T, T>>
{
};

/// @brief Returns the I-th endpoint of the interval (0 = first, 1 = last).
template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
const std::tuple_element_t<I, ::ka::ClosedInterval<T, Traits>> & get(const ::ka::ClosedInterval<T, Traits> & interval)
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
