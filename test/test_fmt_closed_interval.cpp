#include <gtest/gtest.h>

#include <optional>
#include <string>

#include <fmt/format.h>

#include <ka/common/closed_interval.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/fmt_closed_interval.hpp>

namespace ka
{

// ClosedInterval default formatting

TEST(FmtClosedIntervalTest, default_format_s32)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{}", interval));
}

TEST(FmtClosedIntervalTest, default_format_single_value)
{
    const ClosedInterval<s32> interval { 42, 42 };
    EXPECT_EQ("[42, 42]", fmt::format("{}", interval));
}

TEST(FmtClosedIntervalTest, default_format_negative)
{
    const ClosedInterval<s32> interval { -3, 7 };
    EXPECT_EQ("[-3, 7]", fmt::format("{}", interval));
}

TEST(FmtClosedIntervalTest, default_format_u32)
{
    const ClosedInterval<u32> interval { 0, 100 };
    EXPECT_EQ("[0, 100]", fmt::format("{}", interval));
}

// Element spec forwarding via ::

TEST(FmtClosedIntervalTest, element_spec_hex)
{
    const ClosedInterval<s32> interval { 10, 255 };
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", interval));
}

TEST(FmtClosedIntervalTest, element_spec_octal)
{
    const ClosedInterval<s32> interval { 8, 16 };
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", interval));
}

TEST(FmtClosedIntervalTest, element_spec_binary)
{
    const ClosedInterval<s32> interval { 5, 10 };
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", interval));
}

// Width and alignment (interval-spec only)

TEST(FmtClosedIntervalTest, width_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10}", interval));
}

TEST(FmtClosedIntervalTest, width_left_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]    ", fmt::format("{:<10}", interval));
}

TEST(FmtClosedIntervalTest, width_center_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("  [1, 5]  ", fmt::format("{:^10}", interval));
}

TEST(FmtClosedIntervalTest, width_center_align_odd_pad)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("**[1, 5]**", fmt::format("{:*^10}", interval));
}

// Combined: outer width + inner element spec via : separator

TEST(FmtClosedIntervalTest, combined_outer_inner_default)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", interval));
}

TEST(FmtClosedIntervalTest, combined_width_and_element)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10:d}", interval));
}

TEST(FmtClosedIntervalTest, combined_zero_fill_and_hex_element)
{
    const ClosedInterval<s32> interval { 1, 15 };
    EXPECT_EQ("00000000000000[1, f]", fmt::format("{:020:x}", interval));
}

// Inner element alignment via : separator
// Use explicit outer+inner when inner starts with alignment char

TEST(FmtClosedIntervalTest, inner_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", interval));
}

TEST(FmtClosedIntervalTest, inner_left_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", interval));
}

TEST(FmtClosedIntervalTest, inner_center_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", interval));
}

TEST(FmtClosedIntervalTest, inner_center_align_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", interval));
}

TEST(FmtClosedIntervalTest, inner_zero_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", interval));
}

TEST(FmtClosedIntervalTest, inner_right_align_negative)
{
    const ClosedInterval<s32> interval { -3, -1 };
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", interval));
}

TEST(FmtClosedIntervalTest, inner_right_align_wider)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", interval));
}

// Combined outer + inner alignment

TEST(FmtClosedIntervalTest, outer_inner_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("     [  1,   5]", fmt::format("{:>15:>3}", interval));
}

TEST(FmtClosedIntervalTest, outer_inner_mixed)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(" [  1  ,   5  ]", fmt::format("{:>15:^5}", interval));
}

TEST(FmtClosedIntervalTest, outer_left_inner_zero_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[001, 005]     ", fmt::format("{:<15:0>3}", interval));
}

// Width smaller than content

TEST(FmtClosedIntervalTest, width_smaller_than_content)
{
    const ClosedInterval<s32> interval { 100, 200 };
    EXPECT_EQ("[100, 200]", fmt::format("{:>5}", interval));
}

// std::optional<ClosedInterval> — empty set

TEST(FmtOptionalClosedIntervalTest, empty_default)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_default)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("[1, 5]", fmt::format("{}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_element_spec)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 10, 255 } };
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_width_right_align)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_width_left_align)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("[1, 5]    ", fmt::format("{:<10}", opt));
}

// Outer width + element spec via : separator

TEST(FmtOptionalClosedIntervalTest, engaged_outer_and_element_spec)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10:d}", opt));
}

TEST(FmtOptionalClosedIntervalTest, empty_outer_and_element_spec)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("        {}", fmt::format("{:>10:d}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_outer_center_align)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("  [1, 5]  ", fmt::format("{:^10:d}", opt));
}

TEST(FmtOptionalClosedIntervalTest, engaged_element_only_spec)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", opt));
}

TEST(FmtOptionalClosedIntervalTest, empty_default_element_spec)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{::d}", opt));
}

// Inner alignment

TEST(FmtOptionalClosedIntervalTest, engaged_inner_right_align)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { 1, 5 } };
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", opt));
}

TEST(FmtOptionalClosedIntervalTest, empty_inner_right_align)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{::>3}", opt));
}

// Negative values

TEST(FmtOptionalClosedIntervalTest, negative_values)
{
    const std::optional<ClosedInterval<s32>> opt { ClosedInterval<s32> { -10, -1 } };
    EXPECT_EQ("[-10, -1]", fmt::format("{}", opt));
}

} // namespace ka
