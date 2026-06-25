#include "roah/distb/config/step_def.hpp"

#include "impl/step_download_impl.hpp"
#include "impl/step_extract_impl.hpp"
#include "roah/distb/errors.hpp"

#include <nlohmann/json.hpp>

#include <unordered_map>

roah::distb::config::StepDef::StepDef(const std::string_view cmd)
    : cmd_{ cmd }
{}

roah::distb::config::StepDef::StepDef(const StepDef &)     = default;
roah::distb::config::StepDef::StepDef(StepDef &&) noexcept = default;

roah::distb::config::StepDef &
roah::distb::config::StepDef::operator=(const StepDef &)
    = default;

roah::distb::config::StepDef &
roah::distb::config::StepDef::operator=(StepDef &&) noexcept
    = default;

roah::distb::config::StepDef::~StepDef() noexcept = default;

std::string_view
roah::distb::config::StepDef::getCmd() const noexcept
{
    return this->cmd_;
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
#define DISTB_STEP(cls)                           \
    {                                             \
        cls::kCmd, _getInstance<cls::Generator>() \
    }

    static std::unordered_map<std::string_view, std::reference_wrapper<const StepGenerator>> _generators{
        DISTB_STEP(impl::StepDownloadImpl),
        DISTB_STEP(impl::StepExtractImpl),
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

    const auto cmd   = json.get<std::string>();
    const auto i_gen = _generators.find(cmd);
    if (i_gen == _generators.end())
    {
        throw LibraryConfigError{ "Unknown step command '{}' for key '{}'.", cmd, key };
    }

    auto ret = (i_gen->second).get()();
    ret->loadFromJson(json);
    return ret;
}
