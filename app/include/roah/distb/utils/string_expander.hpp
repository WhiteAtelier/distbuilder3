// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_UTILS_STRING_EXPANDER_HPP
#define ROAH_DISTB_UTILS_STRING_EXPANDER_HPP

#include <roah/distb/utils/option_value.hpp>

#include <string>
#include <unordered_map>

namespace roah::distb::utils {

// 文字列テンプレートを展開する.
// tmpl 内の ${name} 形式のプレースホルダーを vars から展開し, 結果を result に格納する.
//
// 構文:
//   ${name}            -- vars["name"] に展開する.
//   ${name:opt1,opt2}  -- 展開後の値にプリプロセスオプションを適用する.
//   $$                 -- $ 1文字に置換される.
//   $X (X が { でも $ でもない場合) -- そのまま残す.
//   変数が見つからない場合 -- ${name} をそのまま残す.
//
// 対応プリプロセスオプション (string_expander.cpp の k_preprocessors に登録):
//   lower   -- string 値を小文字化する.
//   upper   -- string 値を大文字化する.
//   to_str  -- 任意の型の値を string 型の OptionValue に変換する.
// プリプロセスは OptionValue を直接受け取るため, int 値に対する加減算なども実装できる.
// 新しいプリプロセスを追加する場合は k_preprocessors にエントリを追加する.
// オプションは , で区切られ, 左から順に適用される.
//
// すべてのプレースホルダーが展開できた場合は true, 未解決のものが残った場合は false を返す.
bool
expandTemplate(
    const std::string &                                  tmpl,
    const std::unordered_map<std::string, OptionValue> & vars,
    std::string &                                        result);

}  // namespace roah::distb::utils

#endif  // ROAH_DISTB_UTILS_STRING_EXPANDER_HPP
