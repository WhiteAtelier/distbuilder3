#include "working_context_impl.hpp"

#include "roah/distb/utils/string_expander.hpp"

roah::distb::app::WorkingContextImpl::WorkingContextImpl(
    std::filesystem::path                                current_working_path,
    const config::Variables &                            variables,
    const std::unordered_map<std::string, std::string> & dependencies)
    : current_working_path_{ std::move(current_working_path) }
    , variables_{ variables }
    , dependencies_{ dependencies }
{}

roah::distb::app::WorkingContextImpl::~WorkingContextImpl() noexcept = default;

const std::filesystem::path &
roah::distb::app::WorkingContextImpl::getCurrentWorkingDirectory() const
{
    return this->current_working_path_;
}

std::string
roah::distb::app::WorkingContextImpl::resolveString(const std::string & str) const
{
    std::string ret;
    utils::expandTemplate(str, this->variables_, ret);
    return ret;
}

const std::unordered_map<std::string, std::string> &
roah::distb::app::WorkingContextImpl::getDependencies() const
{
    return this->dependencies_;
}
