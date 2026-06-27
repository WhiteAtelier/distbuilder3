#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_AND_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_AND_OP_IMPL_HPP

#include "condition_and_or_op_impl.hpp"

#include <memory>
#include <vector>

namespace roah::distb::config::impl {

class ConditionAndOpImpl final : public ConditionAndOrOpImpl
{
public:
    constexpr static std::string_view kOpName = "and";

    ConditionAndOpImpl();
    ~ConditionAndOpImpl() noexcept override;

    bool
    eval(const Variables & variables) const override;
};

}  // namespace roah::distb::config::impl

#endif