#pragma once

#include <concepts>
#include <limits>

#include <ka/common/assert.hpp>
#include <ka/common/cast.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/interval.hpp>

namespace ka
{

struct NoBuiltinOperatorsInt final
{
    s16 value;
};

// Traits specialization for type without built-in operators.
template <bool declare_less, bool declare_equal, bool declare_cmp, typename SizeT>
struct NoBuiltinOperatorsIntIntervalTraits final
{
    [[nodiscard]] constexpr static NoBuiltinOperatorsInt min() noexcept
    {
        return { std::numeric_limits<s16>::min() };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt max() noexcept
    {
        return { std::numeric_limits<s16>::max() };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt prev(const NoBuiltinOperatorsInt & value) noexcept
    {
        KA_PRE(less(min(), value));
        return { exact_cast<s16>(value.value - 1) };
    }

    [[nodiscard]] constexpr static NoBuiltinOperatorsInt next(const NoBuiltinOperatorsInt & value) noexcept
    {
        KA_PRE(less(value, max()));
        return { exact_cast<s16>(value.value + 1) };
    }

    [[nodiscard]] constexpr static SizeT distance(
        const NoBuiltinOperatorsInt & lhs,
        const NoBuiltinOperatorsInt & rhs) noexcept
        requires(!std::same_as<SizeT, void>)
    {
        KA_PRE(lhs.value <= rhs.value);
        return exact_cast<SizeT>(rhs.value - lhs.value);
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
