// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <gtest/gtest.h>
#include <roah/distb/utils/string_expander.hpp>

#include <cstdint>
#include <string>
#include <unordered_map>

// ============================================================
// expandTemplate テスト
// ============================================================

// 変数なしのテンプレートはそのまま返す.
TEST(ExpandTemplateTest, NoPlaceholder)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("Hello, World!", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "Hello, World!");
}

// 基本的な変数展開.
TEST(ExpandTemplateTest, BasicExpand)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "name", roah::distb::utils::OptionValue{ std::string{ "Alice" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("Hello, ${name}!", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "Hello, Alice!");
}

// 複数の変数を展開する.
TEST(ExpandTemplateTest, MultipleVars)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "first", roah::distb::utils::OptionValue{ std::string{ "foo" } } },
        { "second", roah::distb::utils::OptionValue{ std::string{ "bar" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${first}-${second}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "foo-bar");
}

// 変数が見つからない場合はプレースホルダーをそのまま残し false を返す.
TEST(ExpandTemplateTest, MissingVar)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("Hello, ${name}!", vars, result);
    EXPECT_FALSE(ok);
    EXPECT_EQ(result, "Hello, ${name}!");
}

// 一部の変数が見つからない場合.
TEST(ExpandTemplateTest, PartialMissing)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "a", roah::distb::utils::OptionValue{ std::string{ "A" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${a}-${b}", vars, result);
    EXPECT_FALSE(ok);
    EXPECT_EQ(result, "A-${b}");
}

// $$ はエスケープとして $ 1文字に置換される.
TEST(ExpandTemplateTest, DollarEscape)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("Price: $$100", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "Price: $100");
}

// $X (X は { でも $ でもない) はそのまま残す.
TEST(ExpandTemplateTest, DollarOtherChar)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("$X $Y", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "$X $Y");
}

// 文字列末尾の単独 $ はそのまま残す.
TEST(ExpandTemplateTest, TrailingDollar)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("end$", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "end$");
}

// 閉じ括弧がない場合はプレースホルダーをそのまま残す.
TEST(ExpandTemplateTest, UnclosedBrace)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "name", roah::distb::utils::OptionValue{ std::string{ "Alice" } } },
    };
    std::string result;
    // ${name は閉じ括弧がないのでそのまま残る.
    const bool  ok = roah::distb::utils::expandTemplate("${name", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "${name");
}

// ${ の直後が } の場合 (空変数名) は見つからない扱いになる.
TEST(ExpandTemplateTest, EmptyVarName)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars;
    std::string                                                            result;
    const bool ok = roah::distb::utils::expandTemplate("${}", vars, result);
    EXPECT_FALSE(ok);
    EXPECT_EQ(result, "${}");
}

// ---------- プリプロセスオプション ----------

// lower オプションで小文字化する.
TEST(ExpandTemplateTest, OptionLower)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "val", roah::distb::utils::OptionValue{ std::string{ "Hello WORLD" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${val:lower}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "hello world");
}

// upper オプションで大文字化する.
TEST(ExpandTemplateTest, OptionUpper)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "val", roah::distb::utils::OptionValue{ std::string{ "Hello World" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${val:upper}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "HELLO WORLD");
}

// lower, upper を両方指定すると左から順に適用される (結果は upper が最後).
TEST(ExpandTemplateTest, OptionLowerThenUpper)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "val", roah::distb::utils::OptionValue{ std::string{ "MixedCase" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${val:lower,upper}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "MIXEDCASE");
}

// to_str オプションで int64_t 型の OptionValue を文字列として展開する.
TEST(ExpandTemplateTest, OptionToStr_FromInt)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "count", roah::distb::utils::OptionValue{ std::int64_t{ 42 } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("count=${count:to_str}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "count=42");
}

// to_str の後に lower を適用する (int -> string -> lower).
TEST(ExpandTemplateTest, OptionToStrThenLower)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "val", roah::distb::utils::OptionValue{ std::string{ "HELLO" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${val:to_str,lower}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "hello");
}

// 未知のオプションは無視して展開される.
TEST(ExpandTemplateTest, UnknownOptionIgnored)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "val", roah::distb::utils::OptionValue{ std::string{ "hello" } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("${val:unknown}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "hello");
}

// ---------- 数値型の展開 ----------

// int64_t 型の OptionValue はオプションなしで string として展開される.
TEST(ExpandTemplateTest, IntValue)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "count", roah::distb::utils::OptionValue{ std::int64_t{ 42 } } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("count=${count}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "count=42");
}

// bool 型の OptionValue は "1" / "0" として展開される.
TEST(ExpandTemplateTest, BoolValue_True)
{
    const std::unordered_map<std::string, roah::distb::utils::OptionValue> vars{
        { "flag", roah::distb::utils::OptionValue{ true } },
    };
    std::string result;
    const bool  ok = roah::distb::utils::expandTemplate("flag=${flag}", vars, result);
    EXPECT_TRUE(ok);
    EXPECT_EQ(result, "flag=ON");
}
