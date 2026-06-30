#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_EQUAL_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_EQUAL_OP_IMPL_HPP

#include "roah/distb/config/condition.hpp"
#include "roah/distb/utils/option_value.hpp"

#include <memory>
#include <vector>

namespace roah::distb::config::impl {

class ConditionEqualOpImpl final : public Condition
{
public:
    constexpr static std::string_view kOpName = "equal";

    ConditionEqualOpImpl();
    ~ConditionEqualOpImpl() noexcept override;

    void
    loadFromJson(const nlohmann::json & json) override final;

    bool
    eval(const Variables & variables) const override;

    std::unique_ptr<Condition>
    clone() const override;

protected:
    utils::OptionValue lhs_;
    utils::OptionValue rhs_;
};

}  // namespace roah::distb::config::impl

#endif