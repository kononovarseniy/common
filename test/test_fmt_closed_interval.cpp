#include <gtest/gtest.h>

#include <optional>
#include <string>

#include <fmt/format.h>

#include <ka/common/closed_interval.hpp>
#include <ka/common/fixed.hpp>
#include <ka/common/fmt_closed_interval.hpp>

namespace ka
{

// The name of the test has the form <set_kind>_<spec_type>[_<test_name>].
// All supported combinations must be present.
// set_kind:
// * empty_set - tests for std::optional and MaybeTwoClosedIntervals representing empty set,
// * single_interval - tests for all three types representing single interval,
// * two_intervals - tests for MaybeTwoClosedIntervals representing two intervals.
// spec_type:
// * default_spec - tests for default specifiers "{}" "{:}" "{::}",
// * element_spec - tests for element specifiers "{::*}",
// * outer_spec (not supported yet) - tests for outer specifiers "{:*}" "{:*:}",
// * both_spec (not supported yet) - tests for both specifiers "{:*:*}".

// single_interval, default_spec.

TEST(FormatClosedInterval, single_interval_default_spec_s32)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{}", MaybeTwoClosedIntervals<s32>(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{:}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{:}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{:}", MaybeTwoClosedIntervals<s32>(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{::}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{::}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{::}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_default_spec_negative)
{
    const ClosedInterval<s32> interval { -3, 7 };
    EXPECT_EQ("[-3, 7]", fmt::format("{}", interval));
    EXPECT_EQ("[-3, 7]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{}", MaybeTwoClosedIntervals<s32>(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{:}", interval));
    EXPECT_EQ("[-3, 7]", fmt::format("{:}", std::make_optional(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{:}", MaybeTwoClosedIntervals<s32>(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{::}", interval));
    EXPECT_EQ("[-3, 7]", fmt::format("{::}", std::make_optional(interval)));
    EXPECT_EQ("[-3, 7]", fmt::format("{::}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_default_spec_u32)
{
    const ClosedInterval<u32> interval { 0, 100 };
    EXPECT_EQ("[0, 100]", fmt::format("{}", interval));
    EXPECT_EQ("[0, 100]", fmt::format("{}", std::make_optional(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{}", MaybeTwoClosedIntervals<u32>(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{:}", interval));
    EXPECT_EQ("[0, 100]", fmt::format("{:}", std::make_optional(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{:}", MaybeTwoClosedIntervals<u32>(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{::}", interval));
    EXPECT_EQ("[0, 100]", fmt::format("{::}", std::make_optional(interval)));
    EXPECT_EQ("[0, 100]", fmt::format("{::}", MaybeTwoClosedIntervals<u32>(interval)));
}

// single_interval, element_spec.

TEST(FormatClosedInterval, single_interval_element_spec_hex)
{
    const ClosedInterval<s32> interval { 10, 255 };
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", interval));
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", std::make_optional(interval)));
    EXPECT_EQ("[0xa, 0xff]", fmt::format("{::#x}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_octal)
{
    const ClosedInterval<s32> interval { 8, 16 };
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", interval));
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", std::make_optional(interval)));
    EXPECT_EQ("[010, 020]", fmt::format("{::#o}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_binary)
{
    const ClosedInterval<s32> interval { 5, 10 };
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", interval));
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", std::make_optional(interval)));
    EXPECT_EQ("[101, 1010]", fmt::format("{::b}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_default)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", interval));
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", std::make_optional(interval)));
    EXPECT_EQ("[1, 5]", fmt::format("{::d}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_right_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", interval));
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", std::make_optional(interval)));
    EXPECT_EQ("[  1,   5]", fmt::format("{::>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_left_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", interval));
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", std::make_optional(interval)));
    EXPECT_EQ("[1  , 5  ]", fmt::format("{::<3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_center_align)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", interval));
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", std::make_optional(interval)));
    EXPECT_EQ("[ 1 ,  5 ]", fmt::format("{::^3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_center_align_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", interval));
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", std::make_optional(interval)));
    EXPECT_EQ("[**1**, **5**]", fmt::format("{::*^5}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_zero_fill)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", interval));
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", std::make_optional(interval)));
    EXPECT_EQ("[001, 005]", fmt::format("{::0>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_right_align_negative)
{
    const ClosedInterval<s32> interval { -3, -1 };
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", interval));
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", std::make_optional(interval)));
    EXPECT_EQ("[ -3,  -1]", fmt::format("{::>3}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_right_align_wider)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", interval));
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", std::make_optional(interval)));
    EXPECT_EQ("[   1,    5]", fmt::format("{::>4}", MaybeTwoClosedIntervals<s32>(interval)));
}

TEST(FormatClosedInterval, single_interval_element_spec_dynamic_width)
{
    const ClosedInterval<s32> interval { 1, 5 };
    EXPECT_EQ("[    1,     5]", fmt::format("{::{}}", interval, 5));
    EXPECT_EQ("[    1,     5]", fmt::format("{::{}}", std::make_optional(interval), 5));
    EXPECT_EQ("[    1,     5]", fmt::format("{::{}}", MaybeTwoClosedIntervals<s32>(interval), 5));
}

// empty_set, default_spec.

TEST(FormatClosedInterval, empty_set_default_spec)
{
    EXPECT_EQ("{}", fmt::format("{}", std::optional<ClosedInterval<s32>> {}));
    EXPECT_EQ("{}", fmt::format("{}", MaybeTwoClosedIntervals<s32>()));
    EXPECT_EQ("{}", fmt::format("{:}", std::optional<ClosedInterval<s32>> {}));
    EXPECT_EQ("{}", fmt::format("{:}", MaybeTwoClosedIntervals<s32>()));
    EXPECT_EQ("{}", fmt::format("{::}", std::optional<ClosedInterval<s32>> {}));
    EXPECT_EQ("{}", fmt::format("{::}", MaybeTwoClosedIntervals<s32>()));
}

// empty_set, element_spec.

TEST(FormatClosedInterval, empty_set_element_spec)
{
    EXPECT_EQ("{}", fmt::format("{::>5}", std::optional<ClosedInterval<s32>> {}));
    EXPECT_EQ("{}", fmt::format("{::>5}", MaybeTwoClosedIntervals<s32>()));
}

// two_intervals, default_spec.

TEST(FormatClosedInterval, two_intervals_default_spec)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{}", intervals));
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{:}", intervals));
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{::}", intervals));
}

TEST(FormatClosedInterval, two_intervals_default_spec_u32)
{
    const MaybeTwoClosedIntervals<u32> intervals { { 0, 10 }, { 20, 100 } };
    EXPECT_EQ("[0, 10] U [20, 100]", fmt::format("{}", intervals));
    EXPECT_EQ("[0, 10] U [20, 100]", fmt::format("{:}", intervals));
    EXPECT_EQ("[0, 10] U [20, 100]", fmt::format("{::}", intervals));
}

TEST(FormatClosedInterval, two_intervals_default_spec_negative)
{
    const MaybeTwoClosedIntervals<s32> intervals { { -10, -5 }, { 1, 4 } };
    EXPECT_EQ("[-10, -5] U [1, 4]", fmt::format("{}", intervals));
    EXPECT_EQ("[-10, -5] U [1, 4]", fmt::format("{:}", intervals));
    EXPECT_EQ("[-10, -5] U [1, 4]", fmt::format("{::}", intervals));
}

// two_intervals, element_spec.

TEST(FormatClosedInterval, two_intervals_element_spec_hex)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 10, 15 }, { 32, 255 } };
    EXPECT_EQ("[0xa, 0xf] U [0x20, 0xff]", fmt::format("{::#x}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_octal)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 8, 10 }, { 16, 20 } };
    EXPECT_EQ("[010, 012] U [020, 024]", fmt::format("{::#o}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_binary)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 4, 6 } };
    EXPECT_EQ("[1, 11] U [100, 110]", fmt::format("{::b}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_default)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1, 3] U [5, 8]", fmt::format("{::d}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[  1,   3] U [  5,   8]", fmt::format("{::>3}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_left_align)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[1  , 3  ] U [5  , 8  ]", fmt::format("{::<3}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_center_align)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[ 1 ,  3 ] U [ 5 ,  8 ]", fmt::format("{::^3}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_center_align_fill)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[**1**, **3**] U [**5**, **8**]", fmt::format("{::*^5}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_zero_fill)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[001, 003] U [005, 008]", fmt::format("{::0>3}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align_negative)
{
    const MaybeTwoClosedIntervals<s32> intervals { { -5, -3 }, { 1, 4 } };
    EXPECT_EQ("[ -5,  -3] U [  1,   4]", fmt::format("{::>3}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_right_align_wider)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[   1,    3] U [   5,    8]", fmt::format("{::>4}", intervals));
}

TEST(FormatClosedInterval, two_intervals_element_spec_dynamic_width)
{
    const MaybeTwoClosedIntervals<s32> intervals { { 1, 3 }, { 5, 8 } };
    EXPECT_EQ("[    1,     3] U [    5,     8]", fmt::format("{::{}}", intervals, 5));
}

} // namespace ka
