#include <gtest/gtest.h>

#include <optional>
#include <string>

#include <fmt/format.h>

#include <ka/common/closed_interval.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/fmt_closed_interval.hpp>

namespace ka
{

// default_spec: formatting with default "{}" specifier.

TEST(FormatClosedInterval, default_spec_s32)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, default_spec_single_value)
{
    const ClosedInterval<s32> interval { 42, 42 };
    EXPECT_EQ("[42, 42]", fmt::format("{}", interval));
    EXPECT_EQ("[42, 42]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[42, 42]", fmt::format("{}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, default_spec_negative)
{
    const ClosedInterval<s32> interval { -3, 7 };
    EXPECT_EQ("[-3, 7]", fmt::format("{}", interval));
    EXPECT_EQ("[-3, 7]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, default_spec_u32)
{
    const ClosedInterval<u32> interval { 0, 100 };
    EXPECT_EQ("[0, 100]", fmt::format("{}", interval));
    EXPECT_EQ("[0, 100]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{}", MaybeTwoClosedIntervals<u32>(interval)));
}

// element_spec: formatting with element specifier "{::element-spec}".

TEST(FormatClosedInterval, element_spec_empty)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{::}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{::}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{::}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_hex)
{
    const ClosedInterval<s32> interval { 10, 255 };
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", interval));
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", std::make_optional(interval)));
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_octal)
{
    const ClosedInterval<s32> interval { 8, 16 };
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", interval));
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", std::make_optional(interval)));
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_binary)
{
    const ClosedInterval<s32> interval { 5, 10 };
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", interval));
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", std::make_optional(interval)));
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_default)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", interval));
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", std::make_optional(interval)));
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_left_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", interval));
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", std::make_optional(interval)));
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_center_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", interval));
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", std::make_optional(interval)));
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_center_align_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", interval));
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", std::make_optional(interval)));
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_zero_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", interval));
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", std::make_optional(interval)));
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_right_align_negative)
{
    const ClosedInterval<s32> interval { -3, -1 };
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", interval));
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", std::make_optional(interval)));
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, element_spec_right_align_wider)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", interval));
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", std::make_optional(interval)));
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", MaybeTwoClosedIntervals<s32>(interval)));
}

// outer_spec: formating with whole value specifier "{:outer-spec}".

TEST(FormatClosedInterval, outer_spec_empty)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{:}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{:}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{:}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, outer_spec_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10}", interval));
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10}", std::make_optional(interval)));
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, outer_spec_left_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]    ", fmt::format("{:<10}", interval));
    EXPECT_EQ("[1, 5]    ", fmt::format("{:<10}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]    ", fmt::format("{:<10}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, outer_spec_center_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("  [1, 5]  ", fmt::format("{:^10}", interval));
    EXPECT_EQ("  [1, 5]  ", fmt::format("{:^10}", std::make_optional(interval)));
    EXPECT_EQ("  [1, 5]  ", fmt::format("{:^10}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, outer_spec_center_align_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("**[1, 5]**", fmt::format("{:*^10}", interval));
    EXPECT_EQ("**[1, 5]**", fmt::format("{:*^10}", std::make_optional(interval)));
    EXPECT_EQ("**[1, 5]**", fmt::format("{:*^10}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, outer_spec_smaller_than_content)
{
    const ClosedInterval<s32> interval { 100, 200 };
    EXPECT_EQ("[100, 200]", fmt::format("{:>5}", interval));
    EXPECT_EQ("[100, 200]", fmt::format("{:>5}", std::make_optional(interval)));
    EXPECT_EQ("[100, 200]", fmt::format("{:>5}", MaybeTwoClosedIntervals<s32>(interval)));
}

// combined_specs: formatting with both whole value and element specifiers "{:outer-spec:element-spec}".

TEST(FormatClosedInterval, combined_specs_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10:d}", interval));
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10:d}", std::make_optional(interval)));
    EXPECT_EQ("    [1, 5]", fmt::format("{:>10:d}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, combined_specs_zero_fill_hex)
{
    const ClosedInterval<s32> interval { 1, 15 };
    EXPECT_EQ("00000000000000[1, f]", fmt::format("{:0>20:x}", interval));
    EXPECT_EQ("00000000000000[1, f]", fmt::format("{:0>20:x}", std::make_optional(interval)));
    EXPECT_EQ("00000000000000[1, f]", fmt::format("{:0>20:x}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, combined_specs_outer_inner_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("     [  1,   5]", fmt::format("{:>15:>3}", interval));
    EXPECT_EQ("     [  1,   5]", fmt::format("{:>15:>3}", std::make_optional(interval)));
    EXPECT_EQ("     [  1,   5]", fmt::format("{:>15:>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, combined_specs_outer_inner_mixed)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ(" [  1  ,   5  ]", fmt::format("{:>15:^5}", interval));
    EXPECT_EQ(" [  1  ,   5  ]", fmt::format("{:>15:^5}", std::make_optional(interval)));
    EXPECT_EQ(" [  1  ,   5  ]", fmt::format("{:>15:^5}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, combined_specs_outer_left_inner_zero_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[001, 005]     ", fmt::format("{:<15:0>3}", interval));
    EXPECT_EQ("[001, 005]     ", fmt::format("{:<15:0>3}", std::make_optional(interval)));
    EXPECT_EQ("[001, 005]     ", fmt::format("{:<15:0>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

// empty_set: formatting an empty set represented as std::nullopt / default-constructed MaybeTwoClosedIntervals.

TEST(FormatClosedInterval, empty_set_default)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{}", opt));
    EXPECT_EQ("{}", fmt::format("{}", MaybeTwoClosedIntervals<s32> {}));
}

TEST(FormatClosedInterval, empty_set_outer_spec_empty)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{:}", opt));
    EXPECT_EQ("{}", fmt::format("{:}", MaybeTwoClosedIntervals<s32> {}));
}

TEST(FormatClosedInterval, empty_set_outer_and_element_spec_empty)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{::}", opt));
    EXPECT_EQ("{}", fmt::format("{::}", MaybeTwoClosedIntervals<s32> {}));
}

TEST(FormatClosedInterval, empty_set_inner_right_align)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("{}", fmt::format("{::>3}", opt));
    EXPECT_EQ("{}", fmt::format("{::>3}", MaybeTwoClosedIntervals<s32> {}));
}

TEST(FormatClosedInterval, empty_set_center_align)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("**{}**", fmt::format("{:*^6}", opt));
    EXPECT_EQ("**{}**", fmt::format("{:*^6}", MaybeTwoClosedIntervals<s32> {}));
}

TEST(FormatClosedInterval, empty_set_outer_and_element_spec)
{
    const std::optional<ClosedInterval<s32>> opt;
    EXPECT_EQ("        {}", fmt::format("{:>10:d}", opt));
    EXPECT_EQ("        {}", fmt::format("{:>10:d}", MaybeTwoClosedIntervals<s32> {}));
}

// two_intervals: formatting MaybeTwoClosedIntervals created via two-argument constructor.

TEST(FormatClosedInterval, two_intervals_default_spec)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{}", mci));
}

TEST(FormatClosedInterval, two_intervals_default_spec_u32)
{
    const MaybeTwoClosedIntervals<u32> mci { { 0, 10 }, { 20, 100 } };
    EXPECT_EQ("[0, 10] U [20, 100]", fmt::format("{}", mci));
}

TEST(FormatClosedInterval, two_intervals_default_spec_negative)
{
    const MaybeTwoClosedIntervals<s32> mci { { -10, -5 }, { 1, 4 } };
    EXPECT_EQ("[-10, -5] U [1, 4]", fmt::format("{}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_hex)
{
    const MaybeTwoClosedIntervals<s32> mci { { 10, 15 }, { 32, 255 } };
    EXPECT_EQ("[0xa, 0xf] U [0x20, 0xff]", fmt::format("{::#x}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_octal)
{
    const MaybeTwoClosedIntervals<s32> mci { { 8, 10 }, { 16, 20 } };
    EXPECT_EQ("[010, 012] U [020, 024]", fmt::format("{::#o}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_binary)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 4, 6 } };
    EXPECT_EQ("[1, 11] U [100, 110]", fmt::format("{::b}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_default)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{::d}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[  1,   3] U [  5,   8]", fmt::format("{::>3}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_left_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1  , 3  ] U [5  , 8  ]", fmt::format("{::<3}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_center_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[ 1 ,  3 ] U [ 5 ,  8 ]", fmt::format("{::^3}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_center_align_fill)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[**1**, **3**] U [**5**, **8**]", fmt::format("{::*^5}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_zero_fill)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[001, 003] U [005, 008]", fmt::format("{::0>3}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align_negative)
{
    const MaybeTwoClosedIntervals<s32> mci { { -5, -3 }, { 1, 4 } };
    EXPECT_EQ("[ -5,  -3] U [  1,   4]", fmt::format("{::>3}", mci));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align_wider)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[   1,    3] U [   5,    8]", fmt::format("{::>4}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_empty)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{:}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_right_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("   [1, 3] U [5, 8]", fmt::format("{:>18}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_left_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]   ", fmt::format("{:<18}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_center_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ(" [1, 3] U [5, 8]  ", fmt::format("{:^18}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_center_align_fill)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("**[1, 3] U [5, 8]**", fmt::format("{:*^19}", mci));
}

TEST(FormatClosedInterval, two_intervals_outer_spec_smaller_than_content)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{:>5}", mci));
}

TEST(FormatClosedInterval, two_intervals_combined_specs_right_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("   [1, 3] U [5, 8]", fmt::format("{:>18:d}", mci));
}

TEST(FormatClosedInterval, two_intervals_combined_specs_zero_fill_hex)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 15 }, { 32, 255 } };
    EXPECT_EQ("00000000[1, f] U [20, ff]", fmt::format("{:0>25:x}", mci));
}

TEST(FormatClosedInterval, two_intervals_combined_specs_outer_inner_right_align)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("   [  1,   3] U [  5,   8]", fmt::format("{:>26:>3}", mci));
}

TEST(FormatClosedInterval, two_intervals_combined_specs_outer_inner_mixed)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[  1  ,   3  ] U [  5  ,   8  ]", fmt::format("{:>29:^5}", mci));
}

TEST(FormatClosedInterval, two_intervals_combined_specs_outer_left_inner_zero_fill)
{
    const MaybeTwoClosedIntervals<s32> mci { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[001, 003] U [005, 008]  ", fmt::format("{:<25:0>3}", mci));
}

} // namespace ka
