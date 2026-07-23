/// @brief Utilites burried in fmt::detail for no reason.

[[nodiscard]] constexpr int code_point_size(const char8_t * utf8) noexcept
{
    const auto c = *utf8;
    // 0b0xxxx_xxx -> 1;
    // 0b10xxx_xxx -> 1; (invalid first byte)
    // 0b110xx_xxx -> 2;
    // 0b1110x_xxx -> 3;
    // 0b11110_xxx -> 4;
    //  >|   |<- other
    //   |   |
    //   |   |>||<- 0b11110_xxx
    //   |   | ||
    //   |   | ||>|   |<- 0b1110x_xxx
    //   |   | || |   |
    //   |   | || |   |>|         |<- 0b110xx_xxx
    //   |   | || |   | |         |
    //   |   | || |   | |         | |<--- 0b10xxx_xxx --->|
    //   |   | || |   | |         | |                     | | <-------------- 0b0xxxx_xxx --------------> |
    //   32 31 30 29 28 27 26 25 24 23 22 21 i0 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
    // 0b00_00_11_10_10_01_01_01_01_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00_00
    return static_cast<int>((0x03a55000000000000ull >> (2 * (c >> 3))) & 3) + 1;
}
