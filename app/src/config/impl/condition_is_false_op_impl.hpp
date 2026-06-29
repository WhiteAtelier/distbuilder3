#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_FALSE_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_FALSE_OP_IMPL_HPP

#include "condition_is_op_impl.hpp"

#include <memory>
#include <vector>

namespace roah::distb::config::impl {

class ConditionIsFalseOpImpl final : public ConditionIsOpImpl
{
public:
    constexpr static std::string_view kOpName = "is_false";

    ConditionIsFalseOpImpl();
    ~ConditionIsFalseOpImpl() noexcept override;

    bool
    eval(const Variables & variables) const override;

    std::unique_ptr<Condition>
    clone() const override;
};

}  // namespace roah::distb::config::impl

#endif