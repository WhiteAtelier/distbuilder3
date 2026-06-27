#ifndef ROAH_DISTB_CONFIG_STEP_DEF_HPP
#define ROAH_DISTB_CONFIG_STEP_DEF_HPP

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace roah::distb {
class WorkingContext;
}  // namespace roah::distb

namespace roah::distb::config {

// ライブラリのビルドステップの定義を表すクラス.
// cmd フィールドによってステップの種別が決まり, 種別ごとに有効なフィールドが異なる.
// todo: Step class に名前変える
class StepDef
{
protected:
    StepDef(const std::string_view cmd);

public:
    StepDef(const StepDef &);
    StepDef(StepDef &&) noexcept;
    StepDef &
    operator=(const StepDef &);
    StepDef &
    operator=(StepDef &&) noexcept;
    virtual ~StepDef() noexcept;

    virtual void
    loadFromJson(const nlohmann::json & json)
        = 0;

    virtual void
    operator()(const WorkingContext & context) const
        = 0;

    virtual std::unique_ptr<StepDef>
    clone() const = 0;

    std::string_view
    getCmd() const noexcept;

protected:
    static bool
    _getStringFromJson(const std::string_view cmd,
                       const nlohmann::json & json,
                       const std::string &    key,
                       std::string &          out);

    static bool
    _getBoolFromJson(const std::string_view cmd,  //
                     const nlohmann::json & json,
                     const std::string &    key,
                     bool &                 out);

private:
    std::string_view cmd_;
};

std::unique_ptr<StepDef>
makeStepDefFromJson(const nlohmann::json & json);

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFIG_STEP_DEF_HPP
