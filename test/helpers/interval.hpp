#pragma once

#include <limits>

#include <ka/common/assert.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/interval.hpp>

namespace ka
{

struct NoBuiltinOperatorsInt final
{
    s32 value;
};

// Traits specialization for type without built-in operators.
template <bool declare_less, bool declare_equal, bool declare_cmp, bool declare_distance = true>
struct NoBuiltinOperatorsIntIntervalTraits final
{
    [[nodiscard]] constexpr static NoBuiltinOperatorsInt min() noexcept
    {
        return { std::numeric_limits<s32>::min() };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt max() noexcept
    {
        return { std::numeric_limits<s32>::max() };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt prev(const NoBuiltinOperatorsInt & value) noexcept
    {
        KA_PRE(less(min(), value));
        return { value.value - 1 };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt next(const NoBuiltinOperatorsInt & value) noexcept
    {
        KA_PRE(less(value, max()));
        return { value.value + 1 };
    }

    [[nodiscard]] constexpr static size_t distance(
        const NoBuiltinOperatorsInt & lhs,
        const NoBuiltinOperatorsInt & rhs) noexcept
        requires(declare_distance)
    {
        KA_PRE(lhs.value <= rhs.value);
        return rhs.value - lhs.value;
    }

    [[nodiscard]] constexpr static bool less(
        const NoBuiltinOperatorsInt & lhs,
        const NoBuiltinOperatorsInt & rhs) noexcept
        requires(declare_less)
    {
        return lhs.value < rhs.value;
    }

    [[nodiscard]] constexpr static bool equal(
        const NoBuiltinOperatorsInt & lhs,
        const NoBuiltinOperatorsInt & rhs) noexcept
        requires(declare_equal)
    {
        return lhs.value == rhs.value;
    }

    [[nodiscard]] constexpr static auto cmp(
        const NoBuiltinOperatorsInt & lhs,
        const NoBuiltinOperatorsInt & rhs) noexcept
        requires(declare_cmp)
    {
        return lhs.value <=> rhs.value;
    }
};

} // namespace ka
