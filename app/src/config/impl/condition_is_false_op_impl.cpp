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

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::impl::ConditionIsFalseOpImpl::clone() const
{
    auto ret = std::make_unique<ConditionIsFalseOpImpl>();
    if (std::holds_alternative<std::unique_ptr<Condition>>(this->value_))
    {
        ret->value_ = std::get<std::unique_ptr<Condition>>(this->value_)->clone();
    }
    else
    {
        ret->value_ = std::get<utils::OptionValue>(this->value_);
    }
    return ret;
}
