#include "roah/distb/config/condition.hpp"

#include "impl/condition_and_op_impl.hpp"
#include "impl/condition_equal_op_impl.hpp"
#include "impl/condition_is_false_op_impl.hpp"
#include "impl/condition_is_true_op_impl.hpp"
#include "impl/condition_or_op_impl.hpp"
#include "roah/distb/errors.hpp"

#include <nlohmann/json.hpp>

#include <functional>

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Condition class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
roah::distb::config::Condition::Condition(const std::string_view op_name)
    : op_name_{ op_name }
{}

roah::distb::config::Condition::~Condition() noexcept = default;

std::string_view
roah::distb::config::Condition::getOpName() const noexcept
{
    return this->op_name_;
}

namespace {
template <typename T>
T &
_getInstance()
{
    static T instance;
    return instance;
}
}  // namespace

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::makeConditionFromJson(const nlohmann::json & json)
{
#define DISTB_CONDITION(cls)                                 \
    {                                                        \
        cls::kOpName, [] { return std::make_unique<cls>(); } \
    }

    static std::unordered_map<std::string_view, std::function<std::unique_ptr<Condition>()>> _generators{
        DISTB_CONDITION(impl::ConditionAndOpImpl),    DISTB_CONDITION(impl::ConditionOrOpImpl),
        DISTB_CONDITION(impl::ConditionIsTrueOpImpl), DISTB_CONDITION(impl::ConditionIsFalseOpImpl),
        DISTB_CONDITION(impl::ConditionEqualOpImpl),
    };

    std::string op_name;

    if (json.is_object())
    {
        const auto op_iter = json.find("op");
        if (op_iter == json.end())
        {
            throw LibraryConfigError{ "Missing 'op' field in 'condition' object." };
        }
        if (!op_iter->is_string())
        {
            throw LibraryConfigError{ "Invalid 'op' field type in 'condition' object: expected a string." };
        }
        op_name = op_iter->get<std::string>();
    }
    else if (json.is_string())
    {
        op_name = impl::ConditionIsTrueOpImpl::kOpName;
    }

    const auto i_gen = _generators.find(op_name);
    if (i_gen == _generators.end())
    {
        throw LibraryConfigError{ "Unknown condition op '{}'.", op_name };
    }

    return (i_gen->second)();
}