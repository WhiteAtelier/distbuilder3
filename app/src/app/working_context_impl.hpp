#ifndef ROAH_DISTB_APP_WORKING_CONTEXT_IMPL_HPP
#define ROAH_DISTB_APP_WORKING_CONTEXT_IMPL_HPP

#include "roah/distb/config/variables.hpp"
#include "roah/distb/utils/option_value.hpp"
#include "roah/distb/working_context.hpp"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace roah::distb::app {

class AppConfig;

class WorkingContextImpl final : public WorkingContext
{
public:
    WorkingContextImpl(const AppConfig &                                    app_config,
                       std::filesystem::path                                current_working_directory,
                       const config::Variables &                            variables,
                       const std::unordered_map<std::string, std::string> & dependencies);
    ~WorkingContextImpl() noexcept override;

    const std::filesystem::path &
    getBuildRootDirectory() const override;

    const std::string &
    getAccessToken(const std::string & site, const std::string & key) const override;

    const std::filesystem::path &
    getCurrentWorkingDirectory() const override;

    std::string
    resolveString(const std::string & str) const override;

    const std::unordered_map<std::string, std::string> &
    getDependencies() const override;

    bool
    evalCondition(const config::Condition & condition) const override;

    void
    registRuntimeVariable(std::string key, utils::OptionValue value, const bool in_step_ns) override;

private:
    const AppConfig &                                    app_config_;
    const std::filesystem::path                          current_working_directory_;
    config::Variables                                    variables_;
    const std::unordered_map<std::string, std::string> & dependencies_;
};

}  // namespace roah::distb::app

#endif
