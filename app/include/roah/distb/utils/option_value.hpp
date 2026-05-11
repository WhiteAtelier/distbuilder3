// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_UTILS_OPTION_VALUE_HPP
#define ROAH_DISTB_UTILS_OPTION_VALUE_HPP

#include <cstdint>
#include <string>
#include <variant>

namespace roah::distb::utils {
/// @brief オプションの値を表すクラス.
///
/// null は持てない.
/// string, bool, int, double のいずれかを保持する.
///
/// それぞれを string で取得できるようにする.
/// true は 1, false は 0 という文字列にする.
class OptionValue
{
public:
    OptionValue();  // 空文字
    OptionValue(std::string value);
    OptionValue(const bool value);
    OptionValue(const std::int64_t value);
    OptionValue(const double value);
    OptionValue(const OptionValue &);
    OptionValue(OptionValue &&) noexcept;
    OptionValue &
    operator=(const OptionValue &);
    OptionValue &
    operator=(OptionValue &&) noexcept;
    ~OptionValue();

    bool
    operator==(const OptionValue & other) const;
    bool
    operator!=(const OptionValue & other) const;

    explicit
    operator std::string() const;

    // bool はそのまま返す.
    // string は, 小文字にして 1, true, on, yes なら true / 0, false, off, no なら false とする.
    // int は 0 なら false, それ以外なら true とする.
    // 上の条件に当てはまらない場合は例外を投げる.
    explicit
    operator bool() const;

    // 以下, 型マッチしない場合は例外を投げる.
    explicit
    operator std::int64_t() const;
    explicit
    operator double() const;

private:
    std::variant<std::string, bool, std::int64_t, double> value_;
};
}  // namespace roah::distb::utils

#endif  // ROAH_DISTB_UTILS_OPTION_VALUE_HPP
