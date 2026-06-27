#include "condition_and_or_op_impl.hpp"

#include "roah/distb/errors.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionAndOrOpImpl::ConditionAndOrOpImpl(const std::string_view op_name)
    : Condition{ op_name }
{}

roah::distb::config::impl::ConditionAndOrOpImpl::~ConditionAndOrOpImpl() noexcept = default;

void
roah::distb::config::impl::ConditionAndOrOpImpl::loadFromJson(const nlohmann::json & json)
{
    const auto iter = json.find("values");
    if (iter == json.end())
    {
        throw LibraryConfigError{ "ConditionAndOrOpImpl: 'values' field is missing." };
    }
    if (!iter->is_array())
    {
        throw LibraryConfigError{ "ConditionAndOrOpImpl: 'values' field must be an array." };
    }

    for (const auto & child : *iter)
    {
        if (child.is_object())
        {
            const auto op_iter = child.find("op");
            if (op_iter == child.end())
            {
                throw LibraryConfigError{ "ConditionAndOrOpImpl: Each child in 'values' must have an 'op' field." };
            }
            if (!op_iter->is_string())
            {
                throw LibraryConfigError{ "ConditionAndOrOpImpl: 'op' field must be a string." };
            }
            const auto op_name = op_iter->get<std::string>();
            this->children_.emplace_back(makeConditionFromJson(op_name))->loadFromJson(child);
        }
        else if (child.is_string())
        {
            this->children_.emplace_back(makeConditionFromJson("or"))->loadFromJson(child);
        }
        else
        {
            throw LibraryConfigError{ "ConditionAndOrOpImpl: Each child in 'values' must be an object or a string." };
        }
    }
}
