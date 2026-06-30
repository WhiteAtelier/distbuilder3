#include "condition_equal_op_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/utils/string_expander.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionEqualOpImpl::ConditionEqualOpImpl()
    : Condition{ kOpName }
{}

roah::distb::config::impl::ConditionEqualOpImpl::~ConditionEqualOpImpl() noexcept = default;

void
roah::distb::config::impl::ConditionEqualOpImpl::loadFromJson(const nlohmann::json & json)
{
    const auto get_value_fn = [&](const std::string & key, utils::OptionValue & dst) {
        if (const auto iter = json.find(key); iter != json.end())
        {
            if (iter->is_boolean())
            {
                dst = utils::OptionValue{ iter->get<bool>() };
            }
            else if (iter->is_number_integer())
            {
                dst = utils::OptionValue{ iter->get<std::int64_t>() };
            }
            else if (iter->is_number_float())
            {
                dst = utils::OptionValue{ iter->get<double>() };
            }
            else if (iter->is_string())
            {
                dst = utils::OptionValue{ iter->get<std::string>() };
            }
            else
            {
                throw LibraryConfigError{ "Condition '{}' Invalid value type for key: {}", this->getOpName(), key };
            }
        }
    };

    get_value_fn("lhs", this->lhs_);
    get_value_fn("rhs", this->rhs_);
}

bool
roah::distb::config::impl::ConditionEqualOpImpl::eval(const Variables & variables) const
{
    auto lhs = this->lhs_;
    auto rhs = this->rhs_;

    const auto expand_fn = [&](auto & dst) {
        if (dst.hasString())
        {
            std::string result;
            if (!utils::expandTemplate(static_cast<std::string>(dst), variables, result))
            {
                throw LibraryConfigError{ "Condition '{}', Failed to expand template: {}",
                                          this->getOpName(),
                                          static_cast<std::string>(dst) };
            }
            dst = result;
        }
    };
    expand_fn(lhs);
    expand_fn(rhs);
    return lhs == rhs;
}

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::impl::ConditionEqualOpImpl::clone() const
{
    auto ret  = std::make_unique<ConditionEqualOpImpl>();
    ret->lhs_ = this->lhs_;
    ret->rhs_ = this->rhs_;
    return ret;
}
