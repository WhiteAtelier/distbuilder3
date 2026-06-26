#include "roah/distb/utils/base32.hpp"

#include <gtest/gtest.h>

#include <string>

namespace {
std::string
toLower(const std::string & str)
{
    std::string ret;
    ret.reserve(str.size());
    for (const auto c : str)
    {
        ret.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return ret;
}
}  // namespace

TEST(Base32Test, EncodeTest)
{
    using namespace roah::distb::utils;

    const auto check_fn = [](const std::string & input, const std::string & expected) {
        const auto encoded_lg = encodeBase32(reinterpret_cast<const std::byte *>(input.data()),  //
                                             input.size(),
                                             false,
                                             true);
        EXPECT_EQ(encoded_lg, expected);

        const auto expected_sm = toLower(expected);
        const auto encoded_sm  = encodeBase32(reinterpret_cast<const std::byte *>(input.data()),  //
                                             input.size(),
                                             true,
                                             true);
        EXPECT_EQ(encoded_sm, expected_sm);
    };

    check_fn("", "");  // 空文字列の Base32 エンコードは空文字列になる.
    check_fn("Hello, Base32 World.", "JBSWY3DPFQQEEYLTMUZTEICXN5ZGYZBO");
    check_fn("The quick plan fox", "KRUGKIDROVUWG2ZAOBWGC3RAMZXXQ===");
    check_fn("Hoge", "JBXWOZI=");
    check_fn(
        "Target 340, Dr.Jones observed wild animals in Africa. I went to the British Museum last August. ",
        "KRQXEZ3FOQQDGNBQFQQEI4ROJJXW4ZLTEBXWE43FOJ3GKZBAO5UWYZBAMFXGS3LBNRZSA2LOEBAWM4TJMNQS4ICJEB3WK3TUEB2G6IDUNBSSAQTSNF2GS43IEBGXK43FOVWSA3DBON2CAQLVM52XG5BOEA======");
}
