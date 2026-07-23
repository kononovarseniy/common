#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <string_view>

#include <fmt/format.h>

#include <ka/common/closed_interval.hpp>

/// @brief Formatter for ClosedInterval.
///
/// @details Format spec syntax: `{[:interval-spec[:element-spec]]}`.
/// Interval-spec is the same as for std::string_view.
/// Element-spec is forwarded to the element formatter.
template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct fmt::formatter<::ka::ClosedInterval<T, Traits>>
{
    friend struct fmt::formatter<std::optional<::ka::ClosedInterval<T, Traits>>>;
    friend struct fmt::formatter<::ka::MaybeTwoClosedIntervals<T, Traits>>;

private:
    fmt::formatter<T> endpoints_formatter;

private:
    // template <typename FormatContext>
    // auto align(FormatContext & ctx, const std::string_view str) const -> decltype(ctx.out())
    // {
    //     return aligning_formatter.format(str, ctx);
    // }

    // template <typename FormatContext>
    // auto format_unaligned(FormatContext & ctx, const ::ka::ClosedInterval<T, Traits> & interval) const
    //     -> decltype(ctx.out())
    // {
    //     auto out_it = ctx.out();
    //     out_it = fmt::format_to(out_it, "[");
    //     out_it = endpoints_formatter.format(interval.first(), ctx);
    //     out_it = fmt::format_to(out_it, ", ");
    //     out_it = endpoints_formatter.format(interval.last(), ctx);
    //     return fmt::format_to(out_it, "]");
    // }

public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> decltype(ctx.begin())
    {
        if (ctx.begin() == ctx.end())
        {
            return ctx.end();
        }

        auto it = ctx.begin();
        if (*it == ':')
        {
            ctx.advance_to(std::next(it));
            it = endpoints_formatter.parse(ctx);
        }
        else
        {
            auto empty_ctx = fmt::format_parse_context { "" };
            endpoints_formatter.parse(empty_ctx);
        }
        if (it != ctx.end() && *it != '}')
        {
            fmt::report_error("Invalid format specifier");
        }
        return it;

        // auto outer_specs_first = ctx.begin();
        // auto outer_specs_last = std::ranges::find_if(
        //     outer_specs_first,
        //     ctx.end(),
        //     [](const char c)
        //     {
        //         return c == ':' || c == '}';
        //     });
        // auto inner_specs_first = *outer_specs_last == ':' ? std::next(outer_specs_last) : outer_specs_last;
        // auto inner_specs_last = std::ranges::find(inner_specs_first, ctx.end(), '}');

        // fmt::format_parse_context outer_specs_ctx { std::string_view { outer_specs_first, outer_specs_last } };
        // const auto outer_last = aligning_formatter.parse(outer_specs_ctx);
        // if (outer_last != outer_specs_last)
        // {
        //     return outer_last;
        // }

        // fmt::format_parse_context inner_specs_ctx { std::string_view { inner_specs_first, inner_specs_last } };
        // const auto inner_last = endpoints_formatter.parse(inner_specs_ctx);
        // KA_ASSERT(inner_last == inner_specs_last);

        // return inner_last;
    }

    template <typename FormatContext>
    auto format(const ::ka::ClosedInterval<T, Traits> & interval, FormatContext & ctx) const -> decltype(ctx.out())
    {
        auto out_it = ctx.out();
        out_it = fmt::format_to(out_it, "[");
        out_it = endpoints_formatter.format(interval.first(), ctx);
        out_it = fmt::format_to(out_it, ", ");
        out_it = endpoints_formatter.format(interval.last(), ctx);
        return fmt::format_to(out_it, "]");
        // fmt::memory_buffer buf;
        // fmt::format_context buf_ctx { buf, fmt::format_args() }; // format_args ???

        // format_unaligned(buf_ctx, interval);

        // return align(ctx, std::string_view { buf.data(), buf.size() });
    }
};

/// @brief Formatter for std::optional<ClosedInterval> which allows empty set.
/// @details Format spec syntax is identical to ClosedInterval.
template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct fmt::formatter<std::optional<::ka::ClosedInterval<T, Traits>>> : fmt::formatter<::ka::ClosedInterval<T, Traits>>
{
    template <typename FormatContext>
    auto format(const std::optional<::ka::ClosedInterval<T, Traits>> & interval, FormatContext & ctx) const
        -> decltype(ctx.out())
    {
        using Super = fmt::formatter<::ka::ClosedInterval<T, Traits>>;

        if (interval.has_value())
        {
            return Super::format(interval.value(), ctx);
        }
        return fmt::format_to(ctx.out(), "{{}}");
    }
};

/// @brief Formatter for MaybeTwoClosedIntervals which allows empty set and two closed intervals.
/// @details Format spec syntax is identical to ClosedInterval.
template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct fmt::formatter<::ka::MaybeTwoClosedIntervals<T, Traits>> : fmt::formatter<::ka::ClosedInterval<T, Traits>>
{
    template <typename FormatContext>
    auto format(const ::ka::MaybeTwoClosedIntervals<T, Traits> & intervals, FormatContext & ctx) const
        -> decltype(ctx.out())
    {
        using Super = fmt::formatter<::ka::ClosedInterval<T, Traits>>;

        if (intervals.size() == 0)
        {
            return fmt::format_to(ctx.out(), "{{}}");
        }

        if (intervals.size() == 1)
        {
            Super::format(intervals.begin()[0], ctx);
        }
        if (intervals.size() == 2)
        {
            Super::format(intervals.begin()[0], ctx);
            fmt::format_to(ctx.out(), " U ");
            Super::format(intervals.begin()[1], ctx);
        }

        return ctx.out();
    }
};
