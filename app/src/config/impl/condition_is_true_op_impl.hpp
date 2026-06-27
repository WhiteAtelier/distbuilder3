#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_TRUE_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_TRUE_OP_IMPL_HPP

#include "condition_is_op_impl.hpp"

#include <memory>
#include <vector>

namespace roah::distb::config::impl {

class ConditionIsTrueOpImpl final : public ConditionIsOpImpl
{
public:
    constexpr static std::string_view kOpName = "is_true";

    ConditionIsTrueOpImpl();
    ~ConditionIsTrueOpImpl() noexcept override;

    bool
    eval(const Variables & variables) const override;

    void
    loadFromJson(const nlohmann::json & json) override;
};

}  // namespace roah::distb::config::impl

#endif