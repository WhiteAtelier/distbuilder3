#ifndef ROAH_DISTB_WORKING_CONTEXT_HPP
#define ROAH_DISTB_WORKING_CONTEXT_HPP

#include <filesystem>

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
};

}  // namespace roah::distb

#endif
