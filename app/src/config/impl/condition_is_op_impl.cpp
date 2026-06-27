#include "condition_is_op_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/utils/option_value.hpp"
#include "roah/distb/utils/string_expander.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionIsOpImpl::ConditionIsOpImpl(const std::string_view op_name)
    : Condition{ op_name }
{}

roah::distb::config::impl::ConditionIsOpImpl::~ConditionIsOpImpl() noexcept = default;

void
roah::distb::config::impl::ConditionIsOpImpl::loadFromJson(const nlohmann::json & json)
{
    const auto iter = json.find("value");
    if (iter == json.end())
    {
        throw LibraryConfigError{ "ConditionIsOpImpl: 'value' field is missing." };
    }

    if (iter->is_boolean())
    {
        this->value_ = iter->get<bool>();
    }
    else if (iter->is_number_integer())
    {
        this->value_ = iter->get<std::int64_t>();
    }
    else if (iter->is_number_float())
    {
        this->value_ = iter->get<double>();
    }
    else if (iter->is_string())
    {
        this->value_ = iter->get<std::string>();
    }
    else if (iter->is_object())
    {
        const auto op_iter = iter->find("op");
        if (op_iter == iter->end())
        {
            throw LibraryConfigError{ "ConditionIsOpImpl: 'op' field is missing in 'value' object." };
        }
        if (!op_iter->is_string())
        {
            throw LibraryConfigError{ "ConditionIsOpImpl: 'op' field must be a string in 'value' object." };
        }
        auto child = makeConditionFromJson(op_iter->get<std::string>());
        child->loadFromJson(*iter);
        this->value_ = std::move(child);
    }
    else
    {
        throw LibraryConfigError{ "ConditionIsOpImpl: 'value' field must be a boolean, integer, float, or string." };
    }
}

bool
roah::distb::config::impl::ConditionIsOpImpl::_eval(const Variables & variables) const
{
    if (std::holds_alternative<utils::OptionValue>(this->value_))
    {
        const auto & value = std::get<utils::OptionValue>(this->value_);
        if (value.hasString())
        {
            std::string ret;
            utils::expandTemplate(static_cast<std::string>(value), variables, ret);
            return static_cast<bool>(utils::OptionValue{ ret });
        }
        return static_cast<bool>(value);
    }
    else
    {
        const auto & child_condition = std::get<std::unique_ptr<Condition>>(this->value_);
        return child_condition->eval(variables);
    }
}
