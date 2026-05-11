// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "roah/distb/config/step_def.hpp"

// ---------- コンストラクタ / デストラクタ ----------

roah::distb::config::StepDef::StepDef(std::string cmd)
    : cmd_ { std::move(cmd) }
{}

roah::distb::config::StepDef::StepDef(const StepDef &) = default;
roah::distb::config::StepDef::StepDef(StepDef &&) noexcept = default;

roah::distb::config::StepDef &
roah::distb::config::StepDef::operator=(const StepDef &) = default;

roah::distb::config::StepDef &
roah::distb::config::StepDef::operator=(StepDef &&) noexcept = default;

roah::distb::config::StepDef::~StepDef() noexcept = default;

// ---------- 公開メソッド ----------

const std::string &
roah::distb::config::StepDef::getCmd() const noexcept
{
    return this->cmd_;
}
