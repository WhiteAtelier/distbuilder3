#include "roah/distb/utils/string.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <unordered_map>

TEST(StringUtilsTest, EmptyHexString)
{
    constexpr std::string_view hex_str         = "";
    const auto                 converted_bytes = roah::distb::utils::toBinaryFromHexString(hex_str);
    EXPECT_TRUE(converted_bytes.empty());
}

TEST(StringUtilsTest, FromHexString1)
{
    constexpr std::string_view hex_str        = "0123456789AbcDeF";
    std::vector<unsigned char> expected_bytes = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

    const auto converted_bytes = roah::distb::utils::toBinaryFromHexString(hex_str);

    EXPECT_EQ(converted_bytes.size(), expected_bytes.size());
    for (size_t i = 0; i < converted_bytes.size(); ++i)
    {
        EXPECT_EQ(converted_bytes[i], static_cast<std::byte>(expected_bytes[i]));
    }
}

TEST(StringUtilsTest, FromHexString2)
{
    constexpr std::string_view hex_str = "fe6ca052b5bdd2c6e0616b360c9b0dcbcc46e01bbd0aa8fd0517c17fc58931db";
    std::vector<unsigned char> expected_bytes
        = { 0xfe, 0x6c, 0xa0, 0x52, 0xb5, 0xbd, 0xd2, 0xc6, 0xe0, 0x61, 0x6b, 0x36, 0x0c, 0x9b, 0x0d, 0xcb,
            0xcc, 0x46, 0xe0, 0x1b, 0xbd, 0x0a, 0xa8, 0xfd, 0x05, 0x17, 0xc1, 0x7f, 0xc5, 0x89, 0x31, 0xdb };

    const auto converted_bytes = roah::distb::utils::toBinaryFromHexString(hex_str);

    EXPECT_EQ(converted_bytes.size(), expected_bytes.size());
    for (size_t i = 0; i < converted_bytes.size(); ++i)
    {
        EXPECT_EQ(converted_bytes[i], static_cast<std::byte>(expected_bytes[i]));
    }
}
