// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#if 0
#    include <gtest/gtest.h>
#    include <roah/distb/utils/option_value.hpp>

#    include <cstdint>
#    include <stdexcept>
#    include <string>

// ============================================================
// OptionValue テスト
// ============================================================

// ---------- コンストラクタ ----------

// デフォルトコンストラクタは空文字列を保持する.
TEST(OptionValueTest, DefaultConstructor)
{
    const roah::distb::utils::OptionValue v;
    EXPECT_EQ(static_cast<std::string>(v), "");
}

// string コンストラクタ.
TEST(OptionValueTest, StringConstructor)
{
    const roah::distb::utils::OptionValue v{ std::string{ "hello" } };
    EXPECT_EQ(static_cast<std::string>(v), "hello");
}

// bool コンストラクタ: true は "1", false は "0" に変換される.
TEST(OptionValueTest, BoolConstructor_True)
{
    const roah::distb::utils::OptionValue v{ true };
    EXPECT_EQ(static_cast<std::string>(v), "1");
}

TEST(OptionValueTest, BoolConstructor_False)
{
    const roah::distb::utils::OptionValue v{ false };
    EXPECT_EQ(static_cast<std::string>(v), "0");
}

// int64_t コンストラクタ.
TEST(OptionValueTest, Int64Constructor)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ 42 } };
    EXPECT_EQ(static_cast<std::string>(v), "42");
}

TEST(OptionValueTest, Int64Constructor_Negative)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ -7 } };
    EXPECT_EQ(static_cast<std::string>(v), "-7");
}

// double コンストラクタ.
TEST(OptionValueTest, DoubleConstructor)
{
    const roah::distb::utils::OptionValue v{ 3.14 };
    // std::to_string の精度に依存するため前方一致で確認する.
    EXPECT_EQ(static_cast<std::string>(v).substr(0, 4), "3.14");
}

// ---------- コピー / ムーブ ----------

TEST(OptionValueTest, CopyConstructor)
{
    const roah::distb::utils::OptionValue src{ std::string{ "copy" } };
    const roah::distb::utils::OptionValue dst{ src };
    EXPECT_EQ(static_cast<std::string>(dst), "copy");
}

TEST(OptionValueTest, MoveConstructor)
{
    roah::distb::utils::OptionValue       src{ std::string{ "move" } };
    const roah::distb::utils::OptionValue dst{ std::move(src) };
    EXPECT_EQ(static_cast<std::string>(dst), "move");
}

TEST(OptionValueTest, CopyAssignment)
{
    const roah::distb::utils::OptionValue src{ std::string{ "assigned" } };
    roah::distb::utils::OptionValue       dst;
    dst = src;
    EXPECT_EQ(static_cast<std::string>(dst), "assigned");
}

TEST(OptionValueTest, MoveAssignment)
{
    roah::distb::utils::OptionValue src{ std::string{ "moved" } };
    roah::distb::utils::OptionValue dst;
    dst = std::move(src);
    EXPECT_EQ(static_cast<std::string>(dst), "moved");
}

// ---------- 比較演算子 ----------

TEST(OptionValueTest, EqualityOperator_SameString)
{
    const roah::distb::utils::OptionValue a{ std::string{ "abc" } };
    const roah::distb::utils::OptionValue b{ std::string{ "abc" } };
    EXPECT_EQ(a, b);
}

TEST(OptionValueTest, EqualityOperator_DifferentString)
{
    const roah::distb::utils::OptionValue a{ std::string{ "abc" } };
    const roah::distb::utils::OptionValue b{ std::string{ "xyz" } };
    EXPECT_NE(a, b);
}

TEST(OptionValueTest, EqualityOperator_DifferentTypes)
{
    // 同じ文字列表現でも内部型が異なれば不一致.
    const roah::distb::utils::OptionValue str_val{ std::string{ "1" } };
    const roah::distb::utils::OptionValue bool_val{ true };
    EXPECT_NE(str_val, bool_val);
}

// ---------- operator bool() ----------

// bool 型はそのまま返す.
TEST(OptionValueTest, ToBool_FromBool_True)
{
    const roah::distb::utils::OptionValue v{ true };
    EXPECT_TRUE(static_cast<bool>(v));
}

TEST(OptionValueTest, ToBool_FromBool_False)
{
    const roah::distb::utils::OptionValue v{ false };
    EXPECT_FALSE(static_cast<bool>(v));
}

// int64_t: 0 以外は true.
TEST(OptionValueTest, ToBool_FromInt_NonZero)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ 1 } };
    EXPECT_TRUE(static_cast<bool>(v));
}

TEST(OptionValueTest, ToBool_FromInt_Zero)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ 0 } };
    EXPECT_FALSE(static_cast<bool>(v));
}

// string: 各種 truthy / falsy 値の確認.
TEST(OptionValueTest, ToBool_FromString_Truthy)
{
    for (const auto * s : { "1", "true", "True", "TRUE", "on", "On", "yes", "Yes" })
    {
        const roah::distb::utils::OptionValue v{ std::string{ s } };
        EXPECT_TRUE(static_cast<bool>(v)) << "failed for: " << s;
    }
}

TEST(OptionValueTest, ToBool_FromString_Falsy)
{
    for (const auto * s : { "0", "false", "False", "FALSE", "off", "Off", "no", "No" })
    {
        const roah::distb::utils::OptionValue v{ std::string{ s } };
        EXPECT_FALSE(static_cast<bool>(v)) << "failed for: " << s;
    }
}

// 認識できない文字列は例外を投げる.
TEST(OptionValueTest, ToBool_FromString_Invalid)
{
    const roah::distb::utils::OptionValue v{ std::string{ "maybe" } };
    EXPECT_THROW(static_cast<bool>(v), std::runtime_error);
}

// double は bool 変換不可.
TEST(OptionValueTest, ToBool_FromDouble_Throws)
{
    const roah::distb::utils::OptionValue v{ 1.0 };
    EXPECT_THROW(static_cast<bool>(v), std::runtime_error);
}

// ---------- operator int64_t() ----------

TEST(OptionValueTest, ToInt64_Success)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ 100 } };
    EXPECT_EQ(static_cast<std::int64_t>(v), 100);
}

// 型が違う場合は例外.
TEST(OptionValueTest, ToInt64_WrongType_Throws)
{
    const roah::distb::utils::OptionValue v{ std::string{ "100" } };
    EXPECT_THROW(static_cast<std::int64_t>(v), std::runtime_error);
}

// ---------- operator double() ----------

TEST(OptionValueTest, ToDouble_Success)
{
    const roah::distb::utils::OptionValue v{ 2.71828 };
    EXPECT_DOUBLE_EQ(static_cast<double>(v), 2.71828);
}

// 型が違う場合は例外.
TEST(OptionValueTest, ToDouble_WrongType_Throws)
{
    const roah::distb::utils::OptionValue v{ std::int64_t{ 1 } };
    EXPECT_THROW(static_cast<double>(v), std::runtime_error);
}

// ---------- operator<=>() ----------

// int 同士: 大小比較.
TEST(OptionValueTest, Spaceship_IntInt_Less)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 1 } };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 2 } };
    EXPECT_LT(a, b);
    EXPECT_LE(a, b);
    EXPECT_GT(b, a);
    EXPECT_GE(b, a);
}

TEST(OptionValueTest, Spaceship_IntInt_Greater)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 5 } };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 3 } };
    EXPECT_GT(a, b);
    EXPECT_GE(a, b);
    EXPECT_LT(b, a);
    EXPECT_LE(b, a);
}

TEST(OptionValueTest, Spaceship_IntInt_Equal)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 7 } };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 7 } };
    EXPECT_LE(a, b);
    EXPECT_GE(a, b);
}

// double 同士: 大小比較.
TEST(OptionValueTest, Spaceship_DoubleDouble_Less)
{
    const roah::distb::utils::OptionValue a{ 1.5 };
    const roah::distb::utils::OptionValue b{ 2.5 };
    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
}

TEST(OptionValueTest, Spaceship_DoubleDouble_Equal)
{
    const roah::distb::utils::OptionValue a{ 3.14 };
    const roah::distb::utils::OptionValue b{ 3.14 };
    EXPECT_LE(a, b);
    EXPECT_GE(a, b);
}

// int と double の混合: double に変換して比較する.
TEST(OptionValueTest, Spaceship_IntDouble_Less)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 1 } };
    const roah::distb::utils::OptionValue b{ 1.5 };
    EXPECT_LT(a, b);
    EXPECT_GT(b, a);
}

TEST(OptionValueTest, Spaceship_DoubleInt_Greater)
{
    const roah::distb::utils::OptionValue a{ 2.5 };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 2 } };
    EXPECT_GT(a, b);
    EXPECT_LT(b, a);
}

TEST(OptionValueTest, Spaceship_IntDouble_Equal)
{
    // 2 と 2.0 は数値として等しい.
    const roah::distb::utils::OptionValue a{ std::int64_t{ 2 } };
    const roah::distb::utils::OptionValue b{ 2.0 };
    EXPECT_LE(a, b);
    EXPECT_GE(a, b);
}

// string が片側にある場合は例外を投げる.
TEST(OptionValueTest, Spaceship_StringLeft_Throws)
{
    const roah::distb::utils::OptionValue a{ std::string{ "hello" } };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 1 } };
    EXPECT_THROW(a <=> b, std::runtime_error);
}

TEST(OptionValueTest, Spaceship_StringRight_Throws)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 1 } };
    const roah::distb::utils::OptionValue b{ std::string{ "hello" } };
    EXPECT_THROW(a <=> b, std::runtime_error);
}

TEST(OptionValueTest, Spaceship_StringBoth_Throws)
{
    const roah::distb::utils::OptionValue a{ std::string{ "abc" } };
    const roah::distb::utils::OptionValue b{ std::string{ "xyz" } };
    EXPECT_THROW(a <=> b, std::runtime_error);
}

// bool が片側にある場合は例外を投げる.
TEST(OptionValueTest, Spaceship_BoolLeft_Throws)
{
    const roah::distb::utils::OptionValue a{ true };
    const roah::distb::utils::OptionValue b{ std::int64_t{ 1 } };
    EXPECT_THROW(a <=> b, std::runtime_error);
}

TEST(OptionValueTest, Spaceship_BoolRight_Throws)
{
    const roah::distb::utils::OptionValue a{ std::int64_t{ 1 } };
    const roah::distb::utils::OptionValue b{ false };
    EXPECT_THROW(a <=> b, std::runtime_error);
}

TEST(OptionValueTest, Spaceship_BoolBoth_Throws)
{
    const roah::distb::utils::OptionValue a{ true };
    const roah::distb::utils::OptionValue b{ false };
    EXPECT_THROW(a <=> b, std::runtime_error);
}
#endif