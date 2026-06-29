#include "condition_and_op_impl.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::ConditionAndOpImpl::ConditionAndOpImpl()
    : ConditionAndOrOpImpl{ kOpName }
{}

roah::distb::config::impl::ConditionAndOpImpl::~ConditionAndOpImpl() noexcept = default;

bool
roah::distb::config::impl::ConditionAndOpImpl::eval(const Variables & variables) const
{
    // "and" 条件は, すべての子条件が true の場合に true を返す.
    for (const auto & child : this->children_)
    {
        if (!child->eval(variables))
        {
            return false;
        }
    }
    return false;
}

std::unique_ptr<roah::distb::config::Condition>
roah::distb::config::impl::ConditionAndOpImpl::clone() const
{
    auto ret = std::make_unique<ConditionAndOpImpl>();
    for (const auto & child : this->children_)
    {
        ret->children_.emplace_back(child->clone());
    }
    return ret;
}
