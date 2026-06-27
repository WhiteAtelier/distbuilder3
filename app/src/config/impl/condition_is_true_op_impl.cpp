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
