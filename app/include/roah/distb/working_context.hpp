#ifndef ROAH_DISTB_WORKING_CONTEXT_HPP
#define ROAH_DISTB_WORKING_CONTEXT_HPP

#include <filesystem>
#include <unordered_map>

namespace roah::distb {

namespace utils {
class OptionValue;
}  // namespace utils
namespace config {
class Condition;
}  // namespace config

class WorkingContext
{
protected:
    WorkingContext();

public:
    virtual ~WorkingContext() noexcept;

    [[nodiscard]]
    virtual const std::filesystem::path &
    getBuildRootDirectory() const
        = 0;

    // 変数には絶対にしないこと.
    [[nodiscard]]
    virtual const std::string &
    getAccessToken(const std::string & site, const std::string & key) const
        = 0;

    [[nodiscard]]
    virtual const std::filesystem::path &
    getCurrentWorkingDirectory() const
        = 0;

    [[nodiscard]]
    virtual std::string
    resolveString(const std::string & str) const
        = 0;

    // [libraryName]: [hash]
    [[nodiscard]]
    virtual const std::unordered_map<std::string, std::string> &
    getDependencies() const
        = 0;

    [[nodiscard]]
    virtual bool
    evalCondition(const config::Condition & condition) const
        = 0;

    virtual void
    registRuntimeVariable(std::string key, utils::OptionValue value, const bool in_step_ns = true)
        = 0;
};

}  // namespace roah::distb

#endif
