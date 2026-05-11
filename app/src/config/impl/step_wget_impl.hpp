// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

// StepDef を継承して, url, output, hash フィールドを持つ wget ステップの定義クラス.
// loadFromJson で StepWgetImpl を生成する static ファクトリ関数を持つ.

#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace roah::distb::config::impl {

// wget ステップの実装クラス.
// cmd: "wget" のステップに対応し, url からファイルをダウンロードする.
class StepWgetImpl final : public StepDef
{
public:
    StepWgetImpl(std::string url, std::string output, std::string hash);

    StepWgetImpl(const StepWgetImpl &)     = default;
    StepWgetImpl(StepWgetImpl &&) noexcept = default;
    StepWgetImpl &
    operator=(const StepWgetImpl &)
        = default;
    StepWgetImpl &
    operator=(StepWgetImpl &&) noexcept
        = default;
    ~StepWgetImpl() noexcept override = default;

    // ステップを実行する. curl を使用して url からファイルをダウンロードする.
    void
    operator()() const override;

    // 自身のディープコピーを生成して返す.
    std::unique_ptr<StepDef>
    clone() const override;

    // JSON オブジェクトから StepWgetImpl を生成する static ファクトリ関数.
    // url フィールドが存在しない場合は nullptr を返す.
    static std::unique_ptr<StepWgetImpl>
    loadFromJson(const nlohmann::json & json);

    const std::string &
    getUrl() const noexcept;

    const std::string &
    getOutput() const noexcept;

    const std::string &
    getHash() const noexcept;

private:
    // ダウンロード URL.
    std::string url_;
    // ダウンロード先ファイル名. 空文字の場合は curl のデフォルト動作に従う.
    std::string output_;
    // ダウンロードファイルの整合性検証用ハッシュ値. 空文字の場合は検証しない.
    std::string hash_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
