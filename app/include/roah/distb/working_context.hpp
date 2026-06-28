#ifndef ROAH_DISTB_WORKING_CONTEXT_HPP
#define ROAH_DISTB_WORKING_CONTEXT_HPP

#include <filesystem>
#include <unordered_map>

namespace roah::distb {

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
    getbuild_root_directory() const
        = 0;

    // 変数には絶対にしないこと.
    [[nodiscard]]
    virtual const std::string &
    getGitHubPublicAccessToken() const
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
};

}  // namespace roah::distb

#endif
