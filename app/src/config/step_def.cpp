#include "roah/distb/config/step_def.hpp"

#include "impl/step_cmake_configure_impl.hpp"
#include "impl/step_cmake_install_all_impl.hpp"
#include "impl/step_download_impl.hpp"
#include "impl/step_extract_impl.hpp"
#include "impl/step_github_download_impl.hpp"
#include "impl/step_install_file_impl.hpp"
#include "roah/distb/config/condition.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <functional>
#include <string>
#include <unordered_map>

roah::distb::config::StepDef::StepDef(const std::string_view cmd)
    : cmd_{ cmd }
{}

roah::distb::config::StepDef::StepDef(const StepDef & cp)
    : cmd_{ cp.cmd_ }
    , condition_{ cp.condition_ ? cp.condition_->clone() : nullptr }
{}
roah::distb::config::StepDef::StepDef(StepDef &&) noexcept = default;

roah::distb::config::StepDef::~StepDef() noexcept = default;

void
roah::distb::config::StepDef::loadFromJson(const nlohmann::json & json)
{
    // condition
    if (const auto i_condition = json.find("condition"); i_condition != json.end())
    {
        this->condition_ = makeConditionFromJson(*i_condition);
        this->condition_->loadFromJson(*i_condition);
    }

    // 派生へ
    this->_loadFromJson(json);
}

void
roah::distb::config::StepDef::operator()(WorkingContext & context) const
{
    if (!this->condition_ || context.evalCondition(*this->condition_))
    {
        this->_execute(context);
    }
    else
    {
        // Skipped.
        logger.trace("Step '{}' skipped due to condition.", this->cmd_);
    }
}

std::string_view
roah::distb::config::StepDef::getCmd() const noexcept
{
    return this->cmd_;
}

bool
roah::distb::config::StepDef::_getStringFromJson(const std::string_view cmd,
                                                 const nlohmann::json & json,
                                                 const std::string &    key,
                                                 std::string &          out)
{
    if (auto iter = json.find(key); iter != json.end())
    {
        if (!iter->is_string())
        {
            throw LibraryConfigError{ "Invalid '{}' step definition: field '{}' must be a string.", cmd, key };
        }
        iter->get_to(out);
        return true;
    }
    return false;
}

bool
roah::distb::config::StepDef::_getBoolFromJson(const std::string_view cmd,
                                               const nlohmann::json & json,
                                               const std::string &    key,
                                               bool &                 out)
{
    if (auto iter = json.find(key); iter != json.end())
    {
        if (!iter->is_boolean())
        {
            throw LibraryConfigError{ "Invalid '{}' step definition: field '{}' must be a boolean.", cmd, key };
        }
        iter->get_to(out);
        return true;
    }
    return false;
}

namespace {
template <typename T>
T &
_getInstance()
{
    static T instance;
    return instance;
}
}  // namespace

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::makeStepDefFromJson(const nlohmann::json & json)
{
#define DISTB_STEP(cls)                                   \
    {                                                     \
        cls::kCmd, [] { return std::make_unique<cls>(); } \
    }

    static std::unordered_map<std::string_view, std::function<std::unique_ptr<StepDef>()>> _generators{
        DISTB_STEP(impl::StepDownloadImpl),       DISTB_STEP(impl::StepExtractImpl),
        DISTB_STEP(impl::StepCMakeConfigureImpl), DISTB_STEP(impl::StepCMakeInstallAllImpl),
        DISTB_STEP(impl::StepInstallFileImpl),    DISTB_STEP(impl::StepGithubDownloadImpl),
    };

    const auto i_cmd = json.find("cmd");
    if (i_cmd == json.cend())
    {
        throw LibraryConfigError{ "Missing 'cmd' field in step definition." };
    }
    if (!i_cmd->is_string())
    {
        throw LibraryConfigError{ "Invalid 'cmd' field type in step definition: expected a string." };
    }

    const auto cmd   = i_cmd->get<std::string>();
    const auto i_gen = _generators.find(cmd);
    if (i_gen == _generators.end())
    {
        throw LibraryConfigError{ "Unknown step command '{}'.", cmd };
    }

    return (i_gen->second)();
}
