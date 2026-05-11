// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_CONFIG_STEP_DEF_HPP
#define ROAH_DISTB_CONFIG_STEP_DEF_HPP

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace roah::distb::config {

// ライブラリのビルドステップの定義を表すクラス.
// cmd フィールドによってステップの種別が決まり, 種別ごとに有効なフィールドが異なる.
class StepDef {
    protected:
    StepDef(std::string cmd) ;
public:

    StepDef(const StepDef&) ;
    StepDef(StepDef&&) noexcept;
    StepDef& operator=(const StepDef&);
    StepDef& operator=(StepDef&&) noexcept;
    virtual  ~StepDef() noexcept;

    virtual void operator()() const = 0;

    // 自身のディープコピーを生成して返す.
    virtual std::unique_ptr<StepDef> clone() const = 0;

    const std::string& getCmd() const noexcept;

    private:
    // ステップのコマンド種別 (例: "subp", "wget", "cmake-configure", "cmake-build").
   std::string cmd_;
};



} // namespace roah::distb::config

#endif // ROAH_DISTB_CONFIG_STEP_DEF_HPP
