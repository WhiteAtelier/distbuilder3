#include "roah/distb/errors.hpp"

roah::distb::DistbuilderException::DistbuilderException(const ReturnCode code, const std::string & msg)
    : runtime_error{ msg }
    , code_{ code }
{}

roah::distb::DistbuilderException::ReturnCode
roah::distb::DistbuilderException::getReturnCode() const noexcept
{
    return this->code_;
}
