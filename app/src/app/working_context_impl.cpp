#include "working_context_impl.hpp"

#include "app_config.hpp"
//
#include "roah/distb/config/condition.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/utils/string_expander.hpp"

roah::distb::app::WorkingContextImpl::WorkingContextImpl(
    const AppConfig &                                    app_config,
    std::filesystem::path                                current_working_directory,
    const config::Variables &                            variables,
    const std::unordered_map<std::string, std::string> & dependencies)
    : app_config_{ app_config }
    , current_working_directory_{ std::move(current_working_directory) }
    , variables_{ variables }
    , dependencies_{ dependencies }
{}

roah::distb::app::WorkingContextImpl::~WorkingContextImpl() noexcept = default;

const std::filesystem::path &
roah::distb::app::WorkingContextImpl::getBuildRootDirectory() const
{
    return this->app_config_.getBuildDirectory();
}

const std::string &
roah::distb::app::WorkingContextImpl::getAccessToken(const std::string & site, const std::string & key) const
{
    return this->app_config_.getAccessToken(site, key);
}

const std::filesystem::path &
roah::distb::app::WorkingContextImpl::getCurrentWorkingDirectory() const
{
    return this->current_working_directory_;
}

std::string
roah::distb::app::WorkingContextImpl::resolveString(const std::string & str) const
{
    std::string ret = str;
    // option を展開した後 option 文字列になる可能性がある
    while (ret.find('$') != std::string::npos)
    {
        std::string retn;
        if (!utils::expandTemplate(ret, this->variables_, retn))
        {
            // "$" 単体文字の可能性がある.
            break;
        }
        ret = std::move(retn);
    }
    return ret;
}

const std::unordered_map<std::string, std::string> &
roah::distb::app::WorkingContextImpl::getDependencies() const
{
    return this->dependencies_;
}

bool
roah::distb::app::WorkingContextImpl::evalCondition(const config::Condition & condition) const
{
    return condition.eval(this->variables_);
}

void
roah::distb::app::WorkingContextImpl::registRuntimeVariable(std::string        key,
                                                            utils::OptionValue value,
                                                            const bool         in_step_ns)
{
    if (in_step_ns)
    {
        this->variables_[this->resolveString("step.${current_step_name}." + key)] = std::move(value);
    }
    else
    {
        this->variables_[std::move(key)] = std::move(value);
    }
}
