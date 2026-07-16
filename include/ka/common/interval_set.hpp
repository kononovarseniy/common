#pragma once

#include <algorithm>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/interval.hpp>
#include <ka/common/interval_traits.hpp>

namespace ka
{

template <typename T, IntervalValueTraitsFor<T> Traits>
class IntervalSetElementsIterator;

template <typename T, IntervalValueTraitsFor<T> Traits>
class IntervalSetIntervalsIterator;

/// @brief A set of ordered values ​​of type T, represented as a collection of intervals.
/// @tparam T The type of the set's elements.
/// @tparam Traits Traits that determine the order of the elements and the allowed range.
template <typename T, IntervalValueTraitsFor<T> Traits = IntervalValueTraits<T>>
class IntervalSet final
{
    using Utils = IntervalValueUtils<T, Traits>;

public:
    /// @brief Constructs empty set.
    IntervalSet()
        : IntervalSet(std::vector<T> {})
    {
    }

    /// @brief Returns true if the set is empty.
    [[nodiscard]] bool empty() const noexcept
    {
        return endpoints_.empty();
    }

    /// @brief Returns true if the set contains all possible values.
    [[nodiscard]] bool full() const noexcept
    {
        return endpoints_.size() == 1 && Utils::equal(endpoints_.front(), Utils::min());
    }

    /// @brief Returns true if the set contains given value.
    [[nodiscard]] bool contains(const T & value) const noexcept
    {
        KA_PRE(Utils::value_is_valid(value));

        const auto it = std::ranges::upper_bound(endpoints_, value, Utils::less);
        return std::distance(endpoints_.begin(), it) % 2 == 1;
    }

    /// @brief Returns the minimal value contained in the set.
    /// @pre The set is not empty.
    [[nodiscard]] T min() const noexcept
    {
        KA_PRE(!empty());
        return endpoints_.front();
    }

    /// @brief Returns the maximal value contained in the set.
    /// @pre The set is not empty.
    [[nodiscard]] T max() const noexcept
    {
        KA_PRE(!empty());
        if (endpoints_.size() % 2 == 0)
        {
            return Utils::prev(endpoints_.back());
        }
        return Utils::max();
    }

    using Iterator = IntervalSetElementsIterator<T, Traits>;

    /// @brief Returns an iterator to the beginning of all values contained in the set.
    [[nodiscard]] Iterator begin() const noexcept
    {
        return Iterator { intervals().begin() };
    }

    /// @brief Returns the past end interator of all values contained in the set.
    [[nodiscard]] Iterator end() const noexcept
    {
        return Iterator { intervals().end() };
    }

    /// @brief Returns the size of the set.
    [[nodiscard]] auto size() const noexcept
    {
        using ResultT = std::invoke_result_t<decltype(Utils::distance), const T &, const T &>;
        ResultT result {};
        for (size_t i = 0; i + 1 < endpoints_.size(); i += 2)
        {
            result += Utils::distance(endpoints_[i], endpoints_[i + 1]);
        }
        if (endpoints_.size() % 2 == 1)
        {
            result += Utils::distance(endpoints_.back(), Utils::max());
            ++result;
        }
        return result;
    }

    /// @brief All continuous intervals in the set.
    class Intervals final
    {
        friend class IntervalSet;

    public:
        using Iterator = IntervalSetIntervalsIterator<T, Traits>;

        /// @brief Returns an iterator to the beginning of all continuous intervals in the set.
        [[nodiscard]] Iterator begin() const noexcept
        {
            return { endpoints_, false };
        }

        /// @brief Returns the past end interator of all continuous intervals in the set.
        [[nodiscard]] Iterator end() const noexcept
        {
            return { endpoints_, true };
        }

        /// @brief Number of continuous intervals in the set.
        [[nodiscard]] size_t size() const noexcept
        {
            return (endpoints_.size() + 1) / 2;
        }

    private:
        Intervals(const std::span<const T> endpoints) noexcept
            : endpoints_ { endpoints }
        {
        }

    private:
        std::span<const T> endpoints_;
    };

    /// @brief All continuous intervals in the set.
    [[nodiscard]] Intervals intervals() const noexcept
    {
        return { endpoints_ };
    }

    /// @brief Returns true if both set contain same elements.
    [[nodiscard]] bool operator==(const IntervalSet & other) const noexcept
    {
        if (endpoints_.size() != other.endpoints_.size())
        {
            return false;
        }
        for (size_t i = 0; i < endpoints_.size(); ++i)
        {
            if (!Utils::equal(endpoints_[i], other.endpoints_[i]))
            {
                return false;
            }
        }
        return true;
    }

    /// @brief Constructs an empty set.
    [[nodiscard]] static IntervalSet make_empty()
    {
        return IntervalSet {};
    }

    /// @brief Constructs a set containing all possible values.
    [[nodiscard]] static IntervalSet make_full()
    {
        return IntervalSet { { Utils::min() } };
    }

    /// @brief Constructs a set containing all values greater or equal to the given one.
    [[nodiscard]] static IntervalSet make_greater_equal(const T & value)
    {
        KA_PRE(Utils::value_is_valid(value));

        return IntervalSet { { value } };
    }

    /// @brief Constructs a set containing all values greater than the given one.
    [[nodiscard]] static IntervalSet make_greater(const T & value)
    {
        KA_PRE(Utils::value_is_valid(value));

        if (Utils::less(value, Utils::max()))
        {
            return IntervalSet { { Utils::next(value) } };
        }
        return make_empty();
    }

    /// @brief Constructs a set containing single value.
    [[nodiscard]] static IntervalSet make_single_value(const T & value)
    {
        KA_PRE(Utils::value_is_valid(value));

        if (Utils::less(value, Utils::max()))
        {
            return IntervalSet { { value, Utils::next(value) } };
        }
        return make_greater_equal(value);
    }

    /// @brief Constructs a set containing all values less or equal to the given one.
    [[nodiscard]] static IntervalSet make_less_equal(const T & value)
    {
        KA_PRE(Utils::value_is_valid(value));

        if (Utils::less(value, Utils::max()))
        {
            return IntervalSet { { Utils::min(), Utils::next(value) } };
        }
        return make_full();
    }

    /// @brief Constructs a set containing all values less then the given one.
    [[nodiscard]] static IntervalSet make_less(const T & value)
    {
        KA_PRE(Utils::value_is_valid(value));

        if (Utils::less(Utils::min(), value))
        {
            return IntervalSet { { Utils::min(), value } };
        }
        return make_empty();
    }

    /// @brief Constructs a set containing all values between the given endpoints, including or excluding the endpoints.
    [[nodiscard]] static IntervalSet make_interval(
        const T & infinum,
        const bool infinum_included,
        const T & supremum,
        const bool supremum_included)
    {
        KA_PRE(Utils::value_is_valid(infinum));
        KA_PRE(Utils::value_is_valid(supremum));
        KA_PRE(!Utils::less(supremum, infinum));

        // When supremum is included and it is the maximum allowed value, we should omit the endpoint.
        if (supremum_included && !Utils::less(supremum, Utils::max()))
        {
            return IntervalSet { { infinum_included ? infinum : Utils::next(infinum) } };
        }

        const auto first_endpoint = infinum_included ? infinum : Utils::next(infinum);
        const auto second_endpoint = supremum_included ? Utils::next(supremum) : supremum;
        if (!Utils::less(first_endpoint, second_endpoint))
        {
            return make_empty();
        }

        return IntervalSet { { first_endpoint, second_endpoint } };
    }

    /// @brief Constructs a complement of the set.
    [[nodiscard]] IntervalSet complement() const
    {
        std::vector<T> result_endpoints;
        if (endpoints_.empty() || !Utils::equal(endpoints_.front(), Utils::min()))
        {
            result_endpoints.reserve(endpoints_.size() + 1);
            result_endpoints.push_back(Utils::min());
            result_endpoints.insert(result_endpoints.end(), endpoints_.begin(), endpoints_.end());
        }
        else
        {
            result_endpoints.reserve(endpoints_.size() - 1);
            result_endpoints.insert(result_endpoints.end(), std::next(endpoints_.begin()), endpoints_.end());
        }
        return IntervalSet { std::move(result_endpoints) };
    }

    /// @brief Constructs the set of all values for which op(lhs.contains(value), rhs.contains(value)) is true.
    template <std::regular_invocable<bool, bool> Op>
        requires std::convertible_to<std::invoke_result_t<Op, bool, bool>, bool>
    [[nodiscard]] static IntervalSet make_boolean(const IntervalSet & lhs, const IntervalSet & rhs, Op op)
    {
        std::vector<T> endpoints;
        endpoints.reserve(lhs.endpoints_.size() + rhs.endpoints_.size());
        auto inside = false;

        auto lhs_it = lhs.endpoints_.begin();
        const auto lhs_end = lhs.endpoints_.end();
        auto inside_lhs = false;

        auto rhs_it = rhs.endpoints_.begin();
        const auto rhs_end = rhs.endpoints_.end();
        auto inside_rhs = false;

        while (lhs_it != lhs_end || rhs_it != rhs_end)
        {
            auto value = [&]
            {
                if (rhs_it == rhs_end || (lhs_it != lhs_end && Utils::less(*lhs_it, *rhs_it)))
                {
                    inside_lhs = !inside_lhs;
                    return *lhs_it++;
                }
                if (lhs_it == lhs_end || (rhs_it != rhs_end && Utils::less(*rhs_it, *lhs_it)))
                {
                    inside_rhs = !inside_rhs;
                    return *rhs_it++;
                }
                inside_lhs = !inside_lhs;
                inside_rhs = !inside_rhs;
                ++lhs_it;
                return *rhs_it++;
            }();

            const bool new_inside = op(inside_lhs, inside_rhs);
            if (inside != new_inside)
            {
                inside = new_inside;
                endpoints.push_back(value);
            }
        }
        return IntervalSet { std::move(endpoints) };
    };

private:
    /// @brief Constructs a set from raw endpoints.
    IntervalSet(std::vector<T> && endpoints) noexcept
        : endpoints_ { std::move(endpoints) }
    {
        KA_PRE(std::ranges::all_of(endpoints, Utils::value_is_valid));
        KA_PRE(all_values_are_ordered(endpoints));
    }

    /// @brief Returns true if endpoints are sorted in ascending order according to Traits.
    [[nodiscard]] constexpr static bool all_values_are_ordered(const std::vector<T> & endpoints) noexcept
    {
        constexpr auto not_ordered = [](const auto & lhs, const auto & rhs)
        {
            return !Utils::less(lhs, rhs);
        };
        return std::ranges::adjacent_find(endpoints, not_ordered) == endpoints.end();
    }

private:
    /// @brief Strictly ascending sequence of endpoints of half-open subintervals.
    ///
    /// @details Endpoints at even indices are inclusive subinterval infima, endpoints at odd indices are exclusive
    /// subinterval suprema. If a supremum is greater than Utils::max(), it is omitted.
    std::vector<T> endpoints_;
};

/// @brief Constructs the union of two given sets.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] static IntervalSet<T, Traits> make_union(
    const IntervalSet<T, Traits> & lhs,
    const IntervalSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a || b;
    };
    return IntervalSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the intersection of two given sets.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] static IntervalSet<T, Traits> make_intersection(
    const IntervalSet<T, Traits> & lhs,
    const IntervalSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a && b;
    };
    return IntervalSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the difference of two given sets.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] static IntervalSet<T, Traits> make_difference(
    const IntervalSet<T, Traits> & lhs,
    const IntervalSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a && !b;
    };
    return IntervalSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the symmetric difference of two given sets.
template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] static IntervalSet<T, Traits> make_symetric_difference(
    const IntervalSet<T, Traits> & lhs,
    const IntervalSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a ^ b;
    };
    return IntervalSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

namespace detail
{

/// @brief Helper class for implementing operator -> for temporary iterator dereferencing results.
template <typename T>
class TemporaryPointer final
{
public:
    explicit TemporaryPointer(const T & value) noexcept
        : value_(value)
    {
    }

    const T * operator->() const noexcept
    {
        return &value_;
    }

private:
    T value_;
};

} // namespace detail

template <typename T, IntervalValueTraitsFor<T> Traits>
class IntervalSetElementsIterator final
{
    friend class IntervalSet<T, Traits>;

    using Utils = IntervalValueUtils<T, Traits>;

public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    IntervalSetElementsIterator() noexcept
        : IntervalSetElementsIterator(IntervalSetIntervalsIterator<T, Traits> {})
    {
    }

    [[nodiscard]] T operator*() const noexcept
    {
        KA_PRE(interval_.valid());
        return item_;
    }

    [[nodiscard]] detail::TemporaryPointer<T> operator->() const noexcept
    {
        KA_PRE(interval_.valid());
        return &item_;
    }

    [[nodiscard]] bool operator==(const IntervalSetElementsIterator & other) const noexcept
    {
        if (interval_ != other.interval_)
        {
            return false;
        }
        return Utils::equal(item_, other.item_);
    }

    IntervalSetElementsIterator & operator++() noexcept
    {
        KA_PRE(interval_.valid());

        if (Utils::less(item_, interval_->second))
        {
            item_ = Utils::next(item_);
        }
        else
        {
            ++interval_;
            item_ = interval_start_or_invalid(interval_);
        }
        return *this;
    }

    IntervalSetElementsIterator & operator--() noexcept
    {
        if (interval_.valid() && Utils::less(interval_->first, item_))
        {
            item_ = Utils::prev(item_);
        }
        else
        {
            --interval_;
            item_ = interval_->second;
        }
        return *this;
    }

    IntervalSetElementsIterator operator++(int) noexcept
    {
        IntervalSetElementsIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    IntervalSetElementsIterator operator--(int) noexcept
    {
        IntervalSetElementsIterator tmp = *this;
        --(*this);
        return tmp;
    }

private:
    explicit IntervalSetElementsIterator(IntervalSetIntervalsIterator<T, Traits> interval) noexcept
        : interval_ { interval }
        , item_ { interval_start_or_invalid(interval_) }
    {
    }

    [[nodiscard]] static T interval_start_or_invalid(IntervalSetIntervalsIterator<T, Traits> interval)
    {
        return interval.valid() ? interval->first : Utils::max();
    }

private:
    /// @brief Iterator to the current interval of the parent set.
    IntervalSetIntervalsIterator<T, Traits> interval_;
    /// @brief The current element, or Utils::max() if interval_ is not valid.
    T item_;
};

template <typename T, IntervalValueTraitsFor<T> Traits>
class IntervalSetIntervalsIterator final
{
    friend class IntervalSet<T, Traits>::Intervals;

    using Utils = IntervalValueUtils<T, Traits>;

public:
    using value_type = std::pair<T, T>;
    using difference_type = std::ptrdiff_t;

    IntervalSetIntervalsIterator() noexcept
        : IntervalSetIntervalsIterator { {}, 0 }
    {
    }

private:
    IntervalSetIntervalsIterator(const std::span<const T> endpoints, const bool end) noexcept
        : IntervalSetIntervalsIterator { endpoints, end ? ((endpoints.size() + 1) / 2) * 2 : 0 }
    {
    }

    IntervalSetIntervalsIterator(const std::span<const T> endpoints, size_t index) noexcept
        : endpoints_ { endpoints }
        , index_ { index }
    {
        KA_PRE(index_ % 2 == 0);
        KA_PRE(index_ <= ((endpoints_.size() + 1) / 2) * 2);
    }

public:
    /// @brief Returns true is the iterator points to valid interval.
    [[nodiscard]] bool valid() const noexcept
    {
        return index_ < endpoints_.size();
    }

    [[nodiscard]] value_type operator*() const noexcept
    {
        KA_PRE(index_ < endpoints_.size());
        if (index_ == endpoints_.size() - 1)
        {
            return { endpoints_[index_], Utils::max() };
        }
        return { endpoints_[index_], Utils::prev(endpoints_[index_ + 1]) };
    }

    [[nodiscard]] auto operator->() const noexcept
    {
        return detail::TemporaryPointer<value_type> { **this };
    }

    [[nodiscard]] auto operator<=>(const IntervalSetIntervalsIterator & other) const noexcept
    {
        KA_PRE(same_set(*this, other));
        return index_ <=> other.index_;
    }

    [[nodiscard]] auto operator==(const IntervalSetIntervalsIterator & other) const noexcept
    {
        KA_PRE(same_set(*this, other));
        return index_ == other.index_;
    }

    [[nodiscard]] difference_type operator-(const IntervalSetIntervalsIterator & other) const noexcept
    {
        KA_PRE(same_set(*this, other));
        return (exact_cast<difference_type>(index_) - exact_cast<difference_type>(other.index_)) / 2;
    }

    [[nodiscard]] value_type operator[](const size_t n) const noexcept
    {
        return *(*this + n);
    }

    IntervalSetIntervalsIterator & operator+=(const size_t n) noexcept
    {
        KA_PRE(index_ + n * 2 < endpoints_.size() + 2);
        index_ += 2 * n;
        return *this;
    }

    IntervalSetIntervalsIterator & operator-=(const size_t n) noexcept
    {
        KA_PRE(index_ >= 2 * n);
        index_ -= 2 * n;
        return *this;
    }

    IntervalSetIntervalsIterator & operator++() noexcept
    {
        return *this += 1;
    }

    IntervalSetIntervalsIterator operator++(int) noexcept
    {
        IntervalSetIntervalsIterator tmp = *this;
        *this += 1;
        return tmp;
    }

    IntervalSetIntervalsIterator & operator--() noexcept
    {
        return *this -= 1;
    }

    IntervalSetIntervalsIterator operator--(int) noexcept
    {
        IntervalSetIntervalsIterator tmp = *this;
        *this -= 1;
        return tmp;
    }

private:
    [[nodiscard]] static bool same_set(
        const IntervalSetIntervalsIterator & lhs,
        const IntervalSetIntervalsIterator & rhs) noexcept
    {
        return lhs.endpoints_.data() == rhs.endpoints_.data() && lhs.endpoints_.size() == rhs.endpoints_.size();
    }

private:
    /// @brief The endpoints of the parent set.
    std::span<const T> endpoints_;
    /// @brief The index of the current interval infinum endpoint.
    /// The index of the current interval multiplied by two.
    size_t index_;
};

template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] IntervalSetIntervalsIterator<T, Traits> operator+(
    IntervalSetIntervalsIterator<T, Traits> it,
    const size_t n) noexcept
{
    it += n;
    return it;
}

template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] IntervalSetIntervalsIterator<T, Traits> operator+(
    const size_t n,
    IntervalSetIntervalsIterator<T, Traits> it) noexcept
{
    it += n;
    return it;
}

template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] IntervalSetIntervalsIterator<T, Traits> operator-(
    IntervalSetIntervalsIterator<T, Traits> it,
    const size_t n) noexcept
{
    it -= n;
    return it;
}

template <typename T, IntervalValueTraitsFor<T> Traits>
[[nodiscard]] IntervalSetIntervalsIterator<T, Traits> operator-(
    const size_t n,
    IntervalSetIntervalsIterator<T, Traits> it) noexcept
{
    it -= n;
    return it;
}

} // namespace ka
