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
class ClosedInterval
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

} // namespace ka

namespace std
{

template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_size<::ka::ClosedInterval<T, Traits>> : ::std::integral_constant<size_t, 2>
{
};

template <size_t I, typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct tuple_element<I, ::ka::ClosedInterval<T, Traits>> : ::std::tuple_element<I, ::std::pair<T, T>>
{
};

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
