#pragma once

#include <algorithm>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>

namespace ka
{

template <typename T>
struct RangeSetValueTraits final
{
};

/// @brief Concept of RangeSet value traits.
template <typename Traits, typename T>
concept RangeSetValueTraitsFor = requires(const T & value) {
    { Traits::min() } noexcept -> std::same_as<T>;
    { Traits::max() } noexcept -> std::same_as<T>;
    { Traits::prev(value) } noexcept -> std::same_as<T>;
    { Traits::next(value) } noexcept -> std::same_as<T>;
    { Traits::less(value, value) } noexcept -> std::convertible_to<bool>;
    { Traits::distance(value, value) } noexcept -> std::same_as<size_t>;
};

template <typename T, RangeSetValueTraitsFor<T> Traits>
class RangeSetElementsIterator;

template <typename T, RangeSetValueTraitsFor<T> Traits>
class RangeSetIntervalsIterator;

/// @brief A set of ordered values ​​of type T, represented as a collection of intervals.
/// @tparam T The type of the set's elements.
/// @tparam Traits Traits that determine the order of the elements and the allowed range.
template <typename T, RangeSetValueTraitsFor<T> Traits = RangeSetValueTraits<T>>
class RangeSet final
{
public:
    /// @brief Constructs empty set.
    RangeSet()
        : RangeSet(std::vector<T> {})
    {
    }

    /// @brief Returns true if the set is empty.
    [[nodiscard]] bool empty() const noexcept
    {
        return endpoints_.empty();
    }

    /// @brief Returns true if the set contains all possible values.
    [[nodiscard]] bool all() const noexcept
    {
        return endpoints_.size() == 1 && equal(endpoints_.front(), Traits::min());
    }

    /// @brief Returns true if the set contains given value.
    [[nodiscard]] bool contains(const T & value) const noexcept
    {
        AR_PRE(value_inside_allowed_range(value));

        const auto it = std::ranges::upper_bound(endpoints_, value, Traits::less);
        return std::distance(endpoints_.begin(), it) % 2 == 1;
    }

    /// @brief Returns the minimal value contained in the set.
    /// @pre The set is not empty.
    [[nodiscard]] T min() const noexcept
    {
        AR_PRE(!empty());
        return endpoints_.front();
    }

    /// @brief Returns the maximal value contained in the set.
    /// @pre The set is not empty.
    [[nodiscard]] T max() const noexcept
    {
        AR_PRE(!empty());
        if (endpoints_.size() % 2 == 0)
        {
            return Traits::prev(endpoints_.back());
        }
        return Traits::max();
    }

    using Iterator = RangeSetElementsIterator<T, Traits>;

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
        using ResultT = std::invoke_result_t<decltype(Traits::distance), const T &, const T &>;
        ResultT result {};
        for (size_t i = 0; i + 1 < endpoints_.size(); i += 2)
        {
            result += Traits::distance(endpoints_[i], endpoints_[i + 1]);
        }
        if (endpoints_.size() % 2 == 1)
        {
            result += Traits::distance(endpoints_.back(), Traits::max());
            ++result;
        }
        return result;
    }

    /// @brief All continuous intervals in the set.
    class Intervals final
    {
        friend class RangeSet;

    public:
        using Iterator = RangeSetIntervalsIterator<T, Traits>;

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
    [[nodiscard]] bool operator==(const RangeSet & other) const noexcept
    {
        if (endpoints_.size() != other.endpoints_.size())
        {
            return false;
        }
        for (size_t i = 0; i < endpoints_.size(); ++i)
        {
            if (!equal(endpoints_[i], other.endpoints_[i]))
            {
                return false;
            }
        }
        return true;
    }

    /// @brief Constructs an empty set.
    [[nodiscard]] static RangeSet make_empty()
    {
        return RangeSet {};
    }

    /// @brief Constructs a set containing all possible values.
    [[nodiscard]] static RangeSet make_all()
    {
        return RangeSet { { Traits::min() } };
    }

    /// @brief Constructs a set containing all values greater or equal to the given one.
    [[nodiscard]] static RangeSet make_greater_equal(const T & value)
    {
        AR_PRE(value_inside_allowed_range(value));

        return RangeSet { { value } };
    }

    /// @brief Constructs a set containing all values greater than the given one.
    [[nodiscard]] static RangeSet make_greater(const T & value)
    {
        AR_PRE(value_inside_allowed_range(value));

        if (Traits::less(value, Traits::max()))
        {
            return RangeSet { { Traits::next(value) } };
        }
        return make_empty();
    }

    /// @brief Constructs a set containing single value.
    [[nodiscard]] static RangeSet make_single_value(const T & value)
    {
        AR_PRE(value_inside_allowed_range(value));

        if (Traits::less(value, Traits::max()))
        {
            return RangeSet { { value, Traits::next(value) } };
        }
        return make_greater_equal(value);
    }

    /// @brief Constructs a set containing all values less or equal to the given one.
    [[nodiscard]] static RangeSet make_less_equal(const T & value)
    {
        AR_PRE(value_inside_allowed_range(value));

        if (Traits::less(value, Traits::max()))
        {
            return RangeSet { { Traits::min(), Traits::next(value) } };
        }
        return make_all();
    }

    /// @brief Constructs a set containing all values less then the given one.
    [[nodiscard]] static RangeSet make_less(const T & value)
    {
        AR_PRE(value_inside_allowed_range(value));

        if (Traits::less(Traits::min(), value))
        {
            return RangeSet { { Traits::min(), value } };
        }
        return make_empty();
    }

    /// @brief Constructs a set containing all values between the given endpoints, including or excluding the endpoints.
    [[nodiscard]] static RangeSet make_interval(
        const T & infinum,
        const bool infinum_included,
        const T & supremum,
        const bool supremum_included)
    {
        AR_PRE(value_inside_allowed_range(infinum));
        AR_PRE(value_inside_allowed_range(supremum));
        AR_PRE(!Traits::less(supremum, infinum));

        // When supremum is included and it is the maximum allowed value, we should omit the endpoint.
        if (supremum_included && !Traits::less(supremum, Traits::max()))
        {
            return RangeSet { { infinum_included ? infinum : Traits::next(infinum) } };
        }

        const auto first_endpoint = infinum_included ? infinum : Traits::next(infinum);
        const auto second_endpoint = supremum_included ? Traits::next(supremum) : supremum;
        if (!Traits::less(first_endpoint, second_endpoint))
        {
            return make_empty();
        }

        return RangeSet { { first_endpoint, second_endpoint } };
    }

    /// @brief Constructs a complement of the set.
    [[nodiscard]] RangeSet complement() const
    {
        std::vector<T> result_endpoints;
        if (endpoints_.empty() || !equal(endpoints_.front(), Traits::min()))
        {
            result_endpoints.reserve(endpoints_.size() + 1);
            result_endpoints.push_back(Traits::min());
            result_endpoints.insert(result_endpoints.end(), endpoints_.begin(), endpoints_.end());
        }
        else
        {
            result_endpoints.reserve(endpoints_.size() - 1);
            result_endpoints.insert(result_endpoints.end(), std::next(endpoints_.begin()), endpoints_.end());
        }
        return RangeSet { std::move(result_endpoints) };
    }

    /// @brief Constructs the set of all values for which op(lhs.contains(value), rhs.contains(value)) is true.
    template <std::regular_invocable<bool, bool> Op>
        requires std::convertible_to<std::invoke_result_t<Op, bool, bool>, bool>
    [[nodiscard]] static RangeSet make_boolean(const RangeSet & lhs, const RangeSet & rhs, Op op)
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
                if (rhs_it == rhs_end || (lhs_it != lhs_end && Traits::less(*lhs_it, *rhs_it)))
                {
                    inside_lhs = !inside_lhs;
                    return *lhs_it++;
                }
                if (lhs_it == lhs_end || (rhs_it != rhs_end && Traits::less(*rhs_it, *lhs_it)))
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
        return RangeSet { std::move(endpoints) };
    };

private:
    /// @brief Constructs a set from raw endpoints.
    RangeSet(std::vector<T> && endpoints) noexcept
        : endpoints_ { std::move(endpoints) }
    {
        AR_PRE(std::ranges::all_of(endpoints, value_inside_allowed_range));
        AR_PRE(all_values_are_ordered(endpoints));
    }

    /// @brief Returns true if endpoints are sorted in ascending order according to Traits.
    [[nodiscard]] constexpr static bool all_values_are_ordered(const std::vector<T> & endpoints) noexcept
    {
        constexpr auto not_ordered = [](const auto & lhs, const auto & rhs)
        {
            return !Traits::less(lhs, rhs);
        };
        return std::ranges::adjacent_find(endpoints, not_ordered) == endpoints.end();
    }

    /// @brief Returns true if the value is inside allowed range of values for given Traits.
    [[nodiscard]] constexpr static bool value_inside_allowed_range(const T & value) noexcept
    {
        return !Traits::less(value, Traits::min()) && !Traits::less(Traits::max(), value);
    }

    /// @brief Returns true if elements are equal acording to the Traits.
    [[nodiscard]] constexpr static bool equal(const T & lhs, const T & rhs) noexcept
    {
        return !Traits::less(lhs, rhs) && !Traits::less(rhs, lhs);
    }

private:
    /// @brief Strictly ascending sequence of endpoints of half-open subintervals.
    ///
    /// @details Endpoints at even indices are inclusive subinterval infima, endpoints at odd indices are exclusive
    /// subinterval suprema. If a supremum is greater than Traits::max(), it is omitted.
    std::vector<T> endpoints_;
};

/// @brief Constructs the union of two given sets.
template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] static RangeSet<T, Traits> make_union(const RangeSet<T, Traits> & lhs, const RangeSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a || b;
    };
    return RangeSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the intersection of two given sets.
template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] static RangeSet<T, Traits> make_intersection(
    const RangeSet<T, Traits> & lhs,
    const RangeSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a && b;
    };
    return RangeSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the difference of two given sets.
template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] static RangeSet<T, Traits> make_difference(
    const RangeSet<T, Traits> & lhs,
    const RangeSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a && !b;
    };
    return RangeSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

/// @brief Constructs the symmetric difference of two given sets.
template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] static RangeSet<T, Traits> make_symetric_difference(
    const RangeSet<T, Traits> & lhs,
    const RangeSet<T, Traits> & rhs)
{
    const auto operation = [](const bool a, const bool b)
    {
        return a ^ b;
    };
    return RangeSet<T, Traits>::make_boolean(lhs, rhs, operation);
}

template <std::integral T>
struct RangeSetValueTraits<T> final
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
        AR_PRE(value > min());
        AR_PRE(value <= max());

        return value - 1;
    }

    [[nodiscard]] constexpr static T next(const T & value) noexcept
    {
        AR_PRE(value >= min());
        AR_PRE(value < max());

        return value + 1;
    }

    [[nodiscard]] constexpr static bool less(const T & lhs, const T & rhs) noexcept
    {
        AR_PRE(lhs >= min());
        AR_PRE(lhs <= max());
        AR_PRE(rhs >= min());
        AR_PRE(rhs <= max());

        return lhs < rhs;
    }

    [[nodiscard]] constexpr static size_t distance(const T & lhs, const T & rhs) noexcept
    {
        AR_PRE(lhs >= min());
        AR_PRE(lhs <= max());
        AR_PRE(rhs >= min());
        AR_PRE(rhs <= max());

        AR_PRE(lhs <= rhs);

        return rhs - lhs;
    }
};

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

template <typename T, RangeSetValueTraitsFor<T> Traits>
class RangeSetElementsIterator final
{
    friend class RangeSet<T, Traits>;

public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;

    RangeSetElementsIterator() noexcept
        : RangeSetElementsIterator(RangeSetIntervalsIterator<T, Traits> {})
    {
    }

    [[nodiscard]] T operator*() const noexcept
    {
        AR_PRE(interval_.valid());
        return item_;
    }

    [[nodiscard]] detail::TemporaryPointer<T> operator->() const noexcept
    {
        AR_PRE(interval_.valid());
        return &item_;
    }

    [[nodiscard]] bool operator==(const RangeSetElementsIterator & other) const noexcept
    {
        if (interval_ != other.interval_)
        {
            return false;
        }
        return !Traits::less(item_, other.item_) && !Traits::less(other.item_, item_);
    }

    RangeSetElementsIterator & operator++() noexcept
    {
        AR_PRE(interval_.valid());

        if (Traits::less(item_, interval_->second))
        {
            item_ = Traits::next(item_);
        }
        else
        {
            ++interval_;
            item_ = interval_start_or_invalid(interval_);
        }
        return *this;
    }

    RangeSetElementsIterator & operator--() noexcept
    {
        if (interval_.valid() && Traits::less(interval_->first, item_))
        {
            item_ = Traits::prev(item_);
        }
        else
        {
            --interval_;
            item_ = interval_->second;
        }
        return *this;
    }

    RangeSetElementsIterator operator++(int) noexcept
    {
        RangeSetElementsIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    RangeSetElementsIterator operator--(int) noexcept
    {
        RangeSetElementsIterator tmp = *this;
        --(*this);
        return tmp;
    }

private:
    explicit RangeSetElementsIterator(RangeSetIntervalsIterator<T, Traits> interval) noexcept
        : interval_ { interval }
        , item_ { interval_start_or_invalid(interval_) }
    {
    }

    [[nodiscard]] static T interval_start_or_invalid(RangeSetIntervalsIterator<T, Traits> interval)
    {
        return interval.valid() ? interval->first : Traits::max();
    }

private:
    /// @brief Iterator to the current interval of the parent set.
    RangeSetIntervalsIterator<T, Traits> interval_;
    /// @brief The current element, or Traits::max() if interval_ is not valid.
    T item_;
};

template <typename T, RangeSetValueTraitsFor<T> Traits>
class RangeSetIntervalsIterator final
{
    friend class RangeSet<T, Traits>::Intervals;

public:
    using value_type = std::pair<T, T>;
    using difference_type = std::ptrdiff_t;

    RangeSetIntervalsIterator() noexcept
        : RangeSetIntervalsIterator { {}, 0 }
    {
    }

private:
    RangeSetIntervalsIterator(const std::span<const T> endpoints, const bool end) noexcept
        : RangeSetIntervalsIterator { endpoints, end ? ((endpoints.size() + 1) / 2) * 2 : 0 }
    {
    }

    RangeSetIntervalsIterator(const std::span<const T> endpoints, size_t index) noexcept
        : endpoints_ { endpoints }
        , index_ { index }
    {
        AR_PRE(index_ % 2 == 0);
        AR_PRE(index_ <= ((endpoints_.size() + 1) / 2) * 2);
    }

public:
    /// @brief Returns true is the iterator points to valid interval.
    [[nodiscard]] bool valid() const noexcept
    {
        return index_ < endpoints_.size();
    }

    [[nodiscard]] value_type operator*() const noexcept
    {
        AR_PRE(index_ < endpoints_.size());
        if (index_ == endpoints_.size() - 1)
        {
            return { endpoints_[index_], Traits::max() };
        }
        return { endpoints_[index_], Traits::prev(endpoints_[index_ + 1]) };
    }

    [[nodiscard]] auto operator->() const noexcept
    {
        return detail::TemporaryPointer<value_type> { **this };
    }

    [[nodiscard]] auto operator<=>(const RangeSetIntervalsIterator & other) const noexcept
    {
        AR_PRE(same_set(*this, other));
        return index_ <=> other.index_;
    }

    [[nodiscard]] auto operator==(const RangeSetIntervalsIterator & other) const noexcept
    {
        AR_PRE(same_set(*this, other));
        return index_ == other.index_;
    }

    [[nodiscard]] difference_type operator-(const RangeSetIntervalsIterator & other) const noexcept
    {
        AR_PRE(same_set(*this, other));
        return (exact_cast<difference_type>(index_) - exact_cast<difference_type>(other.index_)) / 2;
    }

    [[nodiscard]] value_type operator[](const size_t n) const noexcept
    {
        return *(*this + n);
    }

    RangeSetIntervalsIterator & operator+=(const size_t n) noexcept
    {
        AR_PRE(index_ + n * 2 < endpoints_.size() + 2);
        index_ += 2 * n;
        return *this;
    }

    RangeSetIntervalsIterator & operator-=(const size_t n) noexcept
    {
        AR_PRE(index_ >= 2 * n);
        index_ -= 2 * n;
        return *this;
    }

    RangeSetIntervalsIterator & operator++() noexcept
    {
        return *this += 1;
    }

    RangeSetIntervalsIterator operator++(int) noexcept
    {
        RangeSetIntervalsIterator tmp = *this;
        *this += 1;
        return tmp;
    }

    RangeSetIntervalsIterator & operator--() noexcept
    {
        return *this -= 1;
    }

    RangeSetIntervalsIterator operator--(int) noexcept
    {
        RangeSetIntervalsIterator tmp = *this;
        *this -= 1;
        return tmp;
    }

private:
    [[nodiscard]] static bool same_set(
        const RangeSetIntervalsIterator & lhs,
        const RangeSetIntervalsIterator & rhs) noexcept
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

template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] RangeSetIntervalsIterator<T, Traits> operator+(
    RangeSetIntervalsIterator<T, Traits> it,
    const size_t n) noexcept
{
    it += n;
    return it;
}

template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] RangeSetIntervalsIterator<T, Traits> operator+(
    const size_t n,
    RangeSetIntervalsIterator<T, Traits> it) noexcept
{
    it += n;
    return it;
}

template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] RangeSetIntervalsIterator<T, Traits> operator-(
    RangeSetIntervalsIterator<T, Traits> it,
    const size_t n) noexcept
{
    it -= n;
    return it;
}

template <typename T, RangeSetValueTraitsFor<T> Traits>
[[nodiscard]] RangeSetIntervalsIterator<T, Traits> operator-(
    const size_t n,
    RangeSetIntervalsIterator<T, Traits> it) noexcept
{
    it -= n;
    return it;
}

} // namespace ka
