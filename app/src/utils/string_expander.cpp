// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/string_expander.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace {

// プリプロセッサの基底インターフェース.
struct Preprocessor
{
    virtual void
    operator()(roah::distb::utils::OptionValue & val) const = 0;
    virtual ~Preprocessor()                                 = default;
};

// 文字列値を小文字化するプリプロセッサ.
struct LowerPreprocessor : Preprocessor
{
    void
    operator()(roah::distb::utils::OptionValue & val) const override
    {
        auto s = static_cast<std::string>(val);
        std::transform(
            s.begin(), s.end(), s.begin(),
            [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
        val = roah::distb::utils::OptionValue{ std::move(s) };
    }
};

// 文字列値を大文字化するプリプロセッサ.
struct UpperPreprocessor : Preprocessor
{
    void
    operator()(roah::distb::utils::OptionValue & val) const override
    {
        auto s = static_cast<std::string>(val);
        std::transform(
            s.begin(), s.end(), s.begin(),
            [](const unsigned char c) { return static_cast<char>(std::toupper(c)); });
        val = roah::distb::utils::OptionValue{ std::move(s) };
    }
};

// 任意の型の値を string 型の OptionValue に変換するプリプロセッサ.
struct ToStrPreprocessor : Preprocessor
{
    void
    operator()(roah::distb::utils::OptionValue & val) const override
    {
        val = roah::distb::utils::OptionValue{ static_cast<std::string>(val) };
    }
};

static const LowerPreprocessor k_lower;
static const UpperPreprocessor k_upper;
static const ToStrPreprocessor k_to_str;

// プリプロセス名とプリプロセッサを対応させるレジストリ.
// 新しいプリプロセスを追加する場合は, 上に struct を追加し static instance を定義してここに登録する.
static const std::unordered_map<std::string, const Preprocessor *> k_preprocessors{
    { "lower", &k_lower },
    { "upper", &k_upper },
    { "to_str", &k_to_str },
};

}  // namespace

bool
roah::distb::utils::expandTemplate(
    const std::string &                                  tmpl,
    const std::unordered_map<std::string, OptionValue> & vars,
    std::string &                                        result)
{
    result.clear();
    bool              all_expanded = true;
    const std::size_t len          = tmpl.size();
    std::size_t       i            = 0;

    while (i < len)
    {
        if (tmpl[i] != '$')
        {
            result += tmpl[i];
            ++i;
            continue;
        }

        // $ が見つかった.
        if (i + 1 >= len)
        {
            // 文字列末尾の $ はそのまま残す.
            result += '$';
            ++i;
            continue;
        }

        if (tmpl[i + 1] == '$')
        {
            // $$ -> $ に置換する.
            result += '$';
            i += 2;
            continue;
        }

        if (tmpl[i + 1] != '{')
        {
            // $X (X は { でも $ でもない) -> そのまま残す.
            result += '$';
            ++i;
            continue;
        }

        // ${ が見つかった: 変数展開を試みる.
        const auto var_start = i + 2;
        const auto close     = tmpl.find('}', var_start);
        if (close == std::string::npos)
        {
            // 閉じ括弧がない -> そのまま残す.
            result += '$';
            ++i;
            continue;
        }

        const auto inner = tmpl.substr(var_start, close - var_start);

        // : で変数名とオプションに分割する.
        std::string              name;
        std::vector<std::string> options;
        const auto               colon_pos = inner.find(':');
        if (colon_pos == std::string::npos)
        {
            name = inner;
        }
        else
        {
            name = inner.substr(0, colon_pos);
            // オプションを , で分割する.
            std::istringstream ss{ inner.substr(colon_pos + 1) };
            std::string        opt;
            while (std::getline(ss, opt, ','))
            {
                if (!opt.empty())
                {
                    options.push_back(std::move(opt));
                }
            }
        }

        const auto it = vars.find(name);
        if (it == vars.end())
        {
            // 変数が見つからない -> そのまま残し, 失敗フラグを立てる.
            result += tmpl.substr(i, close - i + 1);
            all_expanded = false;
        }
        else
        {
            auto val = it->second;

            // プリプロセスオプションを左から順に適用する.
            for (const auto & opt : options)
            {
                const auto pp_it = k_preprocessors.find(opt);
                if (pp_it != k_preprocessors.end())
                {
                    (*pp_it->second)(val);
                }
                // 未知のオプションは無視する.
            }

            result += static_cast<std::string>(val);
        }

        i = close + 1;
    }

    return all_expanded;
}
