// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_CONFIG_DEPENDENCY_SPEC_HPP
#define ROAH_DISTB_CONFIG_DEPENDENCY_SPEC_HPP

#include "option_value.hpp"

#include <nlohmann/json_fwd.hpp>

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace roah::distb::config {

/// @brief ライブラリのビルドに必要な依存関係を表すクラス.
class DependencySpec
{
public:
    DependencySpec(std::string name);
    DependencySpec(const DependencySpec &);
    DependencySpec(DependencySpec &&) noexcept;
    DependencySpec &
    operator=(const DependencySpec &);
    DependencySpec &
    operator=(DependencySpec &&) noexcept;
    ~DependencySpec();

    /// @brief JSON から依存関係の情報を更新する.
    ///
    /// 現状持っている情報にマージする形
    bool
    updateFromJson(const nlohmann::json & json);

    const std::string &
    getName() const noexcept;

    const std::vector<std::string> &
    getRequiredVersionRange() const noexcept;

    const std::unordered_map<std::string, OptionValue> &
    getOptions() const noexcept;

private:
    std::string name_;  ///< 依存するライブラリの名前.

    // 対応バージョンのリスト.
    // "..." は特殊なバージョン範囲マーカーで, 直前から直後のバージョンまでの全バージョンを示す.
    // 末尾の "..." は直前のバージョン以降の全バージョンを示す.
    std::vector<std::string> required_version_range_;

    // 強制オプション制約のマップ.
    // std::nullopt の値は, 継承元のオプション制約を削除することを示す.
    std::unordered_map<std::string, OptionValue> options_;
};

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFJG_DEPENDENCY_SPEC_HPP
