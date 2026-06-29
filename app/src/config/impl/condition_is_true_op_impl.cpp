#include "condition_is_true_op_impl.hpp"

#include "roah/distb/utils/string_expander.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionIsTrueOpImpl::ConditionIsTrueOpImpl()
    : ConditionIsOpImpl{ kOpName }
{}

roah::distb::config::impl::ConditionIsTrueOpImpl::~ConditionIsTrueOpImpl() noexcept = default;

bool
roah::distb::config::impl::ConditionIsTrueOpImpl::eval(const Variables & variables) const
{
    return this->_eval(variables);
}

void
roah::distb::config::impl::ConditionIsTrueOpImpl::loadFromJson(const nlohmann::json & json)
{
    if (!json.is_object())
    {
        ConditionIsOpImpl::loadFromJson(nlohmann::json{ { "value", json } });
    }
    else
    {
        ConditionIsOpImpl::loadFromJson(json);
    }
}

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::impl::ConditionIsTrueOpImpl::clone() const
{
    auto ret = std::make_unique<ConditionIsTrueOpImpl>();
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