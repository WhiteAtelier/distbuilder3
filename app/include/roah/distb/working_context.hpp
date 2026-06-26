#ifndef ROAH_DISTB_WORKING_CONTEXT_HPP
#define ROAH_DISTB_WORKING_CONTEXT_HPP

#include <filesystem>
#include <unordered_map>

namespace roah::distb {

class WorkingContext
{
protected:
    WorkingContext();

public:
    virtual ~WorkingContext() noexcept;

    virtual const std::filesystem::path &
    getCurrentWorkingDirectory() const
        = 0;

    virtual std::string
    resolveString(const std::string & str) const
        = 0;

    // [libraryName]: [hash]
    virtual const std::unordered_map<std::string, std::string> &
    getDependencies() const
        = 0;
};

}  // namespace roah::distb

#endif
