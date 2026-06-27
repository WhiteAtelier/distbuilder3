// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_UTILS_OPTION_VALUE_HPP
#define ROAH_DISTB_UTILS_OPTION_VALUE_HPP

#include <compare>
#include <cstdint>
#include <string>
#include <variant>

namespace roah::distb::utils {

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @brief オプションの値を表すクラス.
///
/// null は持てない.
/// string, bool, int, double のいずれかを保持する.
///
/// それぞれを string で取得できるようにする.
/// true は 1, false は 0 という文字列にする.
///
class OptionValue
{
public:
    /// @brief 空文字列を保持する OptionValue を作成する.
    OptionValue();

    /// @brief 指定した文字列を保持する OptionValue を作成する.
    OptionValue(std::string value);

    /// @brief 指定した bool 値を保持する OptionValue を作成する.
    OptionValue(const bool value);

    /// @brief 指定した整数値を保持する OptionValue を作成する.
    OptionValue(const std::int64_t value);

    /// @brief 指定した小数値を保持する OptionValue を作成する.
    OptionValue(const double value);

    OptionValue(const OptionValue &);      ///< Copy constructor
    OptionValue(OptionValue &&) noexcept;  ///< Move constructor
    OptionValue &
    operator=(const OptionValue &);  ///< Copy assignment operator
    OptionValue &
    operator=(OptionValue &&) noexcept;  ///< Move assignment operator
    ~OptionValue() noexcept;             ///< Destructor

    /// @brief ふたつの OptionValue が等しいかどうかを比較する.
    ///
    /// 同じ型同士では, それぞれの型の == 演算子で比較する.
    /// 異なる型に関して, 次のルールで比較する.
    ///
    /// - bool は, true=1, false=0 と int として扱う.
    /// - int と double の比較は, int を double に変換して比較する.
    /// - string は, 比較対象の型によってキャストを行う.
    ///     - bool との比較は, operator bool() にしたがって bool に変換する.
    ///     - 文字 '.' を含む場合は double に変換する.
    ///     - 文字 '.' を含まない場合は int に変換する.
    ///     - 変換したあと, 前述した2つのルールに従って比較する.
    [[nodiscard]]
    bool
    operator==(const OptionValue & rhs) const;

    /// @brief ふたつの OptionValue を比較する.
    ///
    /// 同じ型同士では, それぞれの型の <=> 演算子で比較する.
    /// 異なる型に関しては, operator==() のルールを参照すること.
    ///
    /// @sa
    ///     operator==()
    [[nodiscard]]
    std::partial_ordering
    operator<=>(const OptionValue & rhs) const;

    /// @brief bool 値を保持しているかどうかを返す.
    [[nodiscard]]
    bool
    hasBool() const noexcept;

    /// @brief int 値を保持しているかどうかを返す.
    [[nodiscard]]
    bool
    hasInt() const noexcept;

    /// @brief double 値を保持しているかどうかを返す.
    [[nodiscard]]
    bool
    hasDouble() const noexcept;

    /// @brief string 値を保持しているかどうかを返す.
    [[nodiscard]]
    bool
    hasString() const noexcept;

    /// @brief 保持している値を文字列に変換する.
    ///
    /// bool は true="ON", false="OFF" に変換する.
    /// そのほかは std::to_string() に従う.
    explicit
    operator std::string() const;

    /// @brief 保持している値を bool に変換する.
    ///
    /// 保持している値を, 以下のルールにしたがって bool に変換する.
    ///
    /// - bool はそのまま返す.
    /// - int は 0 なら false, それ以外なら true とする.
    /// - double は 0 (全 bits が 0) の場合は false, それ以外なら true とする.
    /// - string は全体を小文字に変換し, "" (空文字), "0", "false", "off", "no" のとき false, それ以外を true とする.
    [[nodiscard]]
    explicit
    operator bool() const;

    /// @brief int64 に変換する.
    ///
    /// 保持している値を, 以下のルールにしたがって int64_t に変換する.
    ///
    /// - bool は, true=1, false=0 とする.
    /// - int はそのまま返す.
    /// - double は, int64_t に変換する (小数点以下は切り捨てる).
    /// - string は std::stoll() に従うが, 変換できない場合は例外を投げる.
    [[nodiscard]]
    explicit
    operator std::int64_t() const;

    /// @brief double に変換する.
    ///
    /// 保持している値を, 以下のルールにしたがって double に変換する.
    ///
    /// - bool は, true=1.0, false=0.0 とする.
    /// - int は, double に変換する.
    /// - double はそのまま返す.
    /// - string は std::stod() に従うが, 変換できない場合は例外を投げる.
    [[nodiscard]]
    explicit
    operator double() const;

private:
    template <typename Comp>
    static typename Comp::ReturnType
    _compare(const OptionValue & lhs, const OptionValue & rhs);

    using ValueType = std::variant<std::string, bool, std::int64_t, double>;
    ValueType value_;
};
}  // namespace roah::distb::utils

#endif  // ROAH_DISTB_UTILS_OPTION_VALUE_HPP
