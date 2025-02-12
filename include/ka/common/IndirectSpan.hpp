#pragma once

#include <ranges>

namespace
{

template <std::ranges::contiguous_range R>
class IndirectSpan final : std::ranges::view_interface<IndirectSpan<R>>
{
public:
    IndirectSpan(R & range)
        : range_ { std::addressof(range) }
        , offset_ { 0 }
        , size_ { std::size(range) }
    {
    }

    IndirectSpan(R & range, std::ranges::iterator_t<R> first, std::sentinel_for<std::ranges::iterator_t<R>> last)
        : IndirectSpan(range, std::distance(std::begin(range), first), std::distance(first, last))
    {
    }

    [[nodiscard]] auto begin() const
    {
        return std::begin(ranges_);
    }

    [[nodiscard]] auto end() const
    {
        return begin() + size_;
    }

private:
    R * range_;
    size_t offset_;
    size_t size_;
};

} // namespace
