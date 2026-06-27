// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <gtest/gtest.h>
#include <roah/distb/utils/hash.hpp>

#include <string>

// ============================================================
// SHA256Hash テスト
//
// 各テストの期待値は PowerShell の Get-FileHash コマンド (SHA256) で確認した.
// ============================================================

// 空文字列の SHA-256.
// SHA-256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
TEST(SHA256HashTest, EmptyString)
{
    roah::distb::utils::SHA256Hash hash;
    EXPECT_EQ(hash.getHashAsHexString(), "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

// "abc" の SHA-256.
// SHA-256("abc") = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
TEST(SHA256HashTest, SimpleString_abc)
{
    roah::distb::utils::SHA256Hash hash;
    const std::string              src = "abc";
    hash.addData(src.data(), src.size());
    EXPECT_EQ(hash.getHashAsHexString(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// "The quick brown fox jumps over the lazy dog" の SHA-256.
// SHA-256("The quick brown fox jumps over the lazy dog") =
// d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592
TEST(SHA256HashTest, SimpleString_quickBrownFox)
{
    roah::distb::utils::SHA256Hash hash;
    const std::string              src = "The quick brown fox jumps over the lazy dog";
    hash.addData(src.data(), src.size());
    EXPECT_EQ(hash.getHashAsHexString(), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592");
}

// null バイト 1 つの SHA-256.
// SHA-256("\x00") = 6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d
TEST(SHA256HashTest, BinaryData_NullByte)
{
    roah::distb::utils::SHA256Hash hash;
    const unsigned char            data = 0x00;
    hash.addData(&data, 1);
    EXPECT_EQ(hash.getHashAsHexString(), "6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d");
}

// addData() を複数回に分けて呼び出しても, 一度に渡した場合と同じ結果になることを確認する.
TEST(SHA256HashTest, IncrementalAddData)
{
    const std::string src = "The quick brown fox jumps over the lazy dog";

    // 一度にすべてのデータを渡す.
    roah::distb::utils::SHA256Hash hash_all;
    hash_all.addData(src.data(), src.size());
    const auto expected = hash_all.getHashAsHexString();

    // 1 バイトずつ追加する.
    roah::distb::utils::SHA256Hash hash_incremental;
    for (const char c : src)
    {
        hash_incremental.addData(&c, 1);
    }
    EXPECT_EQ(hash_incremental.getHashAsHexString(), expected);

    // 前半・後半の 2 回に分けて追加する.
    const std::size_t              half = src.size() / 2;
    roah::distb::utils::SHA256Hash hash_split;
    hash_split.addData(src.data(), half);
    hash_split.addData(src.data() + half, src.size() - half);
    EXPECT_EQ(hash_split.getHashAsHexString(), expected);
}

// getHash() を呼び出しても内部状態が変化しないことを確認する.
TEST(SHA256HashTest, GetHashDoesNotChangeState)
{
    roah::distb::utils::SHA256Hash hash;
    const std::string              part1 = "abc";
    hash.addData(part1.data(), part1.size());

    // getHash() を 2 回呼んで同じ値になるか.
    const auto hash1 = hash.getHashAsHexString();
    const auto hash2 = hash.getHashAsHexString();
    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// ムーブコンストラクタ後のオブジェクトが正しく動作することを確認する.
TEST(SHA256HashTest, MoveConstructor)
{
    roah::distb::utils::SHA256Hash hash1;
    const std::string              src = "abc";
    hash1.addData(src.data(), src.size());

    roah::distb::utils::SHA256Hash hash2{ std::move(hash1) };
    EXPECT_EQ(hash2.getHashAsHexString(), "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// ============================================================
// MD5Hash テスト
//
// 各テストの期待値は PowerShell の Get-FileHash コマンド (SHA256) で確認した.
// ============================================================

// 空文字列の MD5.
// MD5("") = d41d8cd98f00b204e9800998ecf8427e
TEST(MD5HashTest, EmptyString)
{
    roah::distb::utils::MD5Hash hash;
    EXPECT_EQ(hash.getHashAsHexString(), "d41d8cd98f00b204e9800998ecf8427e");
}

// "abc" の MD5.
// MD5("abc") = 900150983cd24fb0d6963f7d28e17f72
TEST(MD5HashTest, SimpleString_abc)
{
    roah::distb::utils::MD5Hash hash;
    const std::string           src = "abc";
    hash.addData(src.data(), src.size());
    EXPECT_EQ(hash.getHashAsHexString(), "900150983cd24fb0d6963f7d28e17f72");
}

// "The quick brown fox jumps over the lazy dog" の MD5.
// MD5("The quick brown fox jumps over the lazy dog") =
// 9e107d9d372bb6826bd81d3542a419d6
TEST(MD5HashTest, SimpleString_quickBrownFox)
{
    roah::distb::utils::MD5Hash hash;
    const std::string           src = "The quick brown fox jumps over the lazy dog";
    hash.addData(src.data(), src.size());
    EXPECT_EQ(hash.getHashAsHexString(), "9e107d9d372bb6826bd81d3542a419d6");
}

// null バイト 1 つの MD5.
// MD5("\x00") = 93b885adfe0da089cdf634904fd59f71
TEST(MD5HashTest, BinaryData_NullByte)
{
    roah::distb::utils::MD5Hash hash;
    const unsigned char         data = 0x00;
    hash.addData(&data, 1);
    EXPECT_EQ(hash.getHashAsHexString(), "93b885adfe0da089cdf634904fd59f71");
}

// addData() を複数回に分けて呼び出しても, 一度に渡した場合と同じ結果になることを確認する.
TEST(MD5HashTest, IncrementalAddData)
{
    const std::string src = "The quick brown fox jumps over the lazy dog";

    // 一度にすべてのデータを渡す.
    roah::distb::utils::MD5Hash hash_all;
    hash_all.addData(src.data(), src.size());
    const auto expected = hash_all.getHashAsHexString();

    // 1 バイトずつ追加する.
    roah::distb::utils::MD5Hash hash_incremental;
    for (const char c : src)
    {
        hash_incremental.addData(&c, 1);
    }
    EXPECT_EQ(hash_incremental.getHashAsHexString(), expected);

    // 前半・後半の 2 回に分けて追加する.
    const std::size_t           half = src.size() / 2;
    roah::distb::utils::MD5Hash hash_split;
    hash_split.addData(src.data(), half);
    hash_split.addData(src.data() + half, src.size() - half);
    EXPECT_EQ(hash_split.getHashAsHexString(), expected);
}

// getHash() を呼び出しても内部状態が変化しないことを確認する.
TEST(MD5HashTest, GetHashDoesNotChangeState)
{
    roah::distb::utils::MD5Hash hash;
    const std::string           part1 = "abc";
    hash.addData(part1.data(), part1.size());

    // getHash() を 2 回呼んで同じ値になるか.
    const auto hash1 = hash.getHashAsHexString();
    const auto hash2 = hash.getHashAsHexString();
    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash1, "900150983cd24fb0d6963f7d28e17f72");
}

// ムーブコンストラクタ後のオブジェクトが正しく動作することを確認する.
TEST(MD5HashTest, MoveConstructor)
{
    roah::distb::utils::MD5Hash hash1;
    const std::string           src = "abc";
    hash1.addData(src.data(), src.size());

    roah::distb::utils::MD5Hash hash2{ std::move(hash1) };
    EXPECT_EQ(hash2.getHashAsHexString(), "900150983cd24fb0d6963f7d28e17f72");
}
