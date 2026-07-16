#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <string_view>

#include <fmt/format.h>

#include <ka/common/closed_interval.hpp>

namespace ka
{

namespace __fmt_detail
{

struct FillAlign
{
    char fill;
    fmt::align align;
};

[[nodiscard]] constexpr FillAlign parse_fill_align(std::string_view spec) noexcept
{
    char fill = ' ';
    fmt::align align = fmt::align::none;

    if (!spec.empty())
    {
        if (spec[0] == '<' || spec[0] == '>' || spec[0] == '^')
        {
            align = (spec[0] == '<') ? fmt::align::left : (spec[0] == '>') ? fmt::align::right : fmt::align::center;
        }
        else if (spec.size() >= 2 && (spec[1] == '<' || spec[1] == '>' || spec[1] == '^'))
        {
            fill = spec[0];
            align = (spec[1] == '<') ? fmt::align::left : (spec[1] == '>') ? fmt::align::right : fmt::align::center;
        }
        else if (spec[0] == '0' && spec.size() >= 2 && spec[1] >= '1' && spec[1] <= '9')
        {
            fill = '0';
            align = fmt::align::right;
        }
    }

    return { fill, align };
}

[[nodiscard]] constexpr size_t parse_width(std::string_view spec, char fill, fmt::align align) noexcept
{
    size_t start = 0;

    if (align != fmt::align::none)
    {
        start = (fill == ' ' || fill == '0') ? 1 : 2;
    }

    if (start < spec.size() && spec[start] >= '0' && spec[start] <= '9')
    {
        size_t w = 0;
        for (size_t i = start; i < spec.size(); ++i)
        {
            if (spec[i] < '0' || spec[i] > '9')
            {
                break;
            }
            w = w * 10 + static_cast<size_t>(spec[i] - '0');
        }
        return w;
    }

    return 0;
}

template <typename OutputIt>
[[nodiscard]] OutputIt write_padded(OutputIt out, std::string_view buf, char fill, fmt::align align, size_t w)
{
    if (buf.size() >= w)
    {
        return std::copy(buf.begin(), buf.end(), out);
    }

    const size_t pad = w - buf.size();

    if (align == fmt::align::right || align == fmt::align::none)
    {
        out = std::fill_n(out, pad, fill);
        return std::copy(buf.begin(), buf.end(), out);
    }
    if (align == fmt::align::left)
    {
        out = std::copy(buf.begin(), buf.end(), out);
        return std::fill_n(out, pad, fill);
    }
    // center
    const size_t left = pad / 2;
    const size_t right = pad - left;
    out = std::fill_n(out, left, fill);
    out = std::copy(buf.begin(), buf.end(), out);
    return std::fill_n(out, right, fill);
}

template <typename OutputIt, typename T>
[[nodiscard]] OutputIt format_interval(OutputIt out, const T & first, const T & last, std::string_view elem_spec)
{
    *out++ = '[';
    if (elem_spec.empty())
    {
        out = fmt::format_to(out, "{}", first);
    }
    else
    {
        out = fmt::format_to(out, fmt::runtime("{:" + std::string(elem_spec) + "}"), first);
    }
    *out++ = ',';
    *out++ = ' ';
    if (elem_spec.empty())
    {
        out = fmt::format_to(out, "{}", last);
    }
    else
    {
        out = fmt::format_to(out, fmt::runtime("{:" + std::string(elem_spec) + "}"), last);
    }
    *out++ = ']';
    return out;
}

} // namespace __fmt_detail

} // namespace ka

/// @brief Formatter for ClosedInterval.
///
/// @details Format spec syntax: `{:interval-spec:element-spec}`.
/// The `:` separator divides interval-spec and element-spec.
/// Interval-spec controls fill, align, width.
/// Element-spec is forwarded to the element formatter.
/// Without `:`, the entire spec is interval-spec (no element forwarding).
///
/// @details Examples:
/// - `"{}"` — default `[first, last]` format.
/// - `"{::d}"` — element spec `d` forwarded to int formatter.
/// - `"{:>10}"` — right-align interval in width 10.
/// - `"{:>10:d}"` — right-align + element decimal.
/// - `"{:020:x}"` — zero-fill width 20 + element hex.
/// - `"{::#x}"` — element hex with `0x` prefix.
/// - `"{::>3}"` — element right-align width 3.
/// - `"{::*^5}"` — element center-align width 5 fill `*`.
/// - `"{::0>3}"` — element zero-fill right-align width 3.
template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct fmt::formatter<::ka::ClosedInterval<T, Traits>>
{
private:
    std::string_view inner_format_;
    char fill_ = ' ';
    fmt::align align_ = fmt::align::none;
    size_t width_ = 0;

public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        auto spec_end = ctx.end();
        if (spec_end != it)
        {
            --spec_end;
        }

        auto colon = std::find(it, spec_end, ':');

        std::string_view outer_format;
        if (colon != spec_end)
        {
            outer_format = std::string_view(it, static_cast<size_t>(colon - it));
            inner_format_ = std::string_view(colon + 1, static_cast<size_t>(spec_end - colon - 1));
        }
        else
        {
            outer_format = std::string_view(it, static_cast<size_t>(spec_end - it));
            inner_format_ = std::string_view();
        }

        const auto [fill, align] = ::ka::__fmt_detail::parse_fill_align(outer_format);
        fill_ = fill;
        align_ = align;
        width_ = ::ka::__fmt_detail::parse_width(outer_format, fill, align);

        return spec_end;
    }

    template <typename FormatContext>
    auto format(const ::ka::ClosedInterval<T, Traits> & interval, FormatContext & ctx) const -> decltype(ctx.out())
    {
        if (width_ == 0)
        {
            return ::ka::__fmt_detail::format_interval(ctx.out(), interval.first(), interval.last(), inner_format_);
        }

        std::string buf;
        static_cast<void>(::ka::__fmt_detail::format_interval(
            std::back_inserter(buf),
            interval.first(),
            interval.last(),
            inner_format_));

        return ::ka::__fmt_detail::write_padded(ctx.out(), buf, fill_, align_, width_);
    }
};

/// @brief Formatter for std::optional<ClosedInterval> (empty set representation).
///
/// @details Format spec syntax is identical to ClosedInterval: `{:outer:inner}`.
/// The same spec that formats a ClosedInterval also works on optional<ClosedInterval>.
/// When engaged, the full spec is forwarded to the ClosedInterval formatter.
/// When empty, `{}` is produced with outer padding applied.
///
/// @details Examples:
/// - `"{}"` — empty: `{}`, engaged: `[1, 5]`.
/// - `"{::d}"` — element spec `d` forwarded to int formatter.
/// - `"{::#x}"` — element hex with `0x` prefix.
/// - `"{::>3}"` — element right-align width 3.
/// - `"{:>10}"` — right-align whole output in width 10.
/// - `"{:>10:d}"` — right-align + element decimal.
template <typename T, ::ka::IntervalValueTraitsFor<T> Traits>
struct fmt::formatter<std::optional<::ka::ClosedInterval<T, Traits>>>
{
private:
    std::string_view outer_format_;
    std::string_view inner_format_;
    char fill_ = ' ';
    fmt::align align_ = fmt::align::none;
    size_t width_ = 0;

public:
    constexpr auto parse(fmt::format_parse_context & ctx) -> decltype(ctx.begin())
    {
        auto it = ctx.begin();
        auto spec_end = ctx.end();
        if (spec_end != it)
        {
            --spec_end;
        }

        auto colon = std::find(it, spec_end, ':');

        if (colon != spec_end)
        {
            outer_format_ = std::string_view(it, static_cast<size_t>(colon - it));
            inner_format_ = std::string_view(colon + 1, static_cast<size_t>(spec_end - colon - 1));
        }
        else
        {
            outer_format_ = std::string_view(it, static_cast<size_t>(spec_end - it));
            inner_format_ = std::string_view();
        }

        const auto [fill, align] = ::ka::__fmt_detail::parse_fill_align(outer_format_);
        fill_ = fill;
        align_ = align;
        width_ = ::ka::__fmt_detail::parse_width(outer_format_, fill, align);

        return spec_end;
    }

    template <typename FormatContext>
    auto format(const std::optional<::ka::ClosedInterval<T, Traits>> & opt, FormatContext & ctx) const
        -> decltype(ctx.out())
    {
        if (!opt.has_value())
        {
            if (width_ == 0)
            {
                return fmt::format_to(ctx.out(), "{{}}");
            }

            std::string buf = "{}";
            return ::ka::__fmt_detail::write_padded(ctx.out(), buf, fill_, align_, width_);
        }

        if (!inner_format_.empty())
        {
            std::string spec(outer_format_);
            spec += ':';
            spec += inner_format_;
            return fmt::vformat_to(ctx.out(), "{:" + spec + "}", fmt::make_format_args(*opt));
        }

        if (!outer_format_.empty())
        {
            return fmt::vformat_to(ctx.out(), "{:" + std::string(outer_format_) + "}", fmt::make_format_args(*opt));
        }

        return fmt::format_to(ctx.out(), "{}", *opt);
    }
};
