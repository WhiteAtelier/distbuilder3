#include "condition_or_op_impl.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionOrOpImpl::ConditionOrOpImpl()
    : ConditionAndOrOpImpl{ kOpName }
{}

roah::distb::config::impl::ConditionOrOpImpl::~ConditionOrOpImpl() noexcept = default;

bool
roah::distb::config::impl::ConditionOrOpImpl::eval(const Variables & variables) const
{
    // "or" 条件は, いずれかの子条件が true の場合に true を返す.
    for (const auto & child : this->children_)
    {
        if (child->eval(variables))
        {
            return true;
        }
    }
    return false;
}

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::impl::ConditionOrOpImpl::clone() const
{
    auto ret = std::make_unique<ConditionOrOpImpl>();
    for (const auto & child : this->children_)
    {
        ret->children_.emplace_back(child->clone());
    }
    return ret;
}
