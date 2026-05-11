// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_CONFIG_LIBRARY_ENTRY_HPP
#define ROAH_DISTB_CONFIG_LIBRARY_ENTRY_HPP

#include "dependency_spec.hpp"
#include "option_value.hpp"
#include "step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace roah::distb::config {

// ライブラリの単一バージョンエントリを表すクラス.
// base フィールドを使用することで, 継承による差分定義が可能.
// 継承エントリでは, 明記されたフィールドは上書き, null 値は削除, 省略は継承となる.
class LibraryEntry
{
public:
    LibraryEntry(std::string version);

    LibraryEntry(const LibraryEntry &) = delete;
    LibraryEntry(LibraryEntry &&) noexcept;
    LibraryEntry &
    operator=(const LibraryEntry &)
        = delete;
    LibraryEntry &
    operator=(LibraryEntry &&) noexcept;
    ~LibraryEntry() noexcept;

    void
    setBase(const LibraryEntry & base_entry);

    void
    updateFromJson(const nlohmann::json & json);

    const std::string &
    getVersion() const noexcept;

    const std::unordered_map<std::string, OptionValue> &
    getOptions() const noexcept;

    const std::unordered_map<std::string, DependencySpec> &
    getDependencies() const noexcept;

    const std::vector<std::string> &
    getRecipes() const noexcept;

    const std::unordered_map<std::string, std::unique_ptr<StepDef>> &
    getSteps() const noexcept;

private:
    // バージョン文字列 (例: "v1.2.3").
    std::string version_;

    // ライブラリのビルドオプションのデフォルト値マップ.
    // std::nullopt の値は, 継承元のオプションを削除することを示す.
    std::unordered_map<std::string, OptionValue> options_;

    // 依存関係のマップ. キーは依存関係の識別名.
    // std::nullopt の値は, 継承元の依存関係を削除することを示す.
    std::unordered_map<std::string, DependencySpec> dependencies_;

    // ビルドレシピの実行順ステップ名リスト.
    // 省略時は継承元の recipes を引き継ぐ.
    std::vector<std::string> recipes_;

    // ビルドステップ定義のマップ. キーはステップ名.
    // nullptr の値は, 継承元のステップを削除することを示す.
    std::unordered_map<std::string, std::unique_ptr<StepDef>> steps_;
};

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFJG_LIBRARY_ENTRY_HPP
