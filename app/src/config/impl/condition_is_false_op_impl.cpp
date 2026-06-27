#include "condition_is_false_op_impl.hpp"

#include "roah/distb/utils/string_expander.hpp"

roah::distb::config::impl::ConditionIsFalseOpImpl::ConditionIsFalseOpImpl()
    : ConditionIsOpImpl{ kOpName }
{}

roah::distb::config::impl::ConditionIsFalseOpImpl::~ConditionIsFalseOpImpl() noexcept = default;

bool
roah::distb::config::impl::ConditionIsFalseOpImpl::eval(const Variables & variables) const
{
    return !this->_eval(variables);
}
