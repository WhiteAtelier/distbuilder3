#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_IS_OP_IMPL_HPP

#include "roah/distb/config/condition.hpp"
#include "roah/distb/utils/option_value.hpp"

#include <memory>
#include <variant>
#include <vector>

namespace roah::distb::config::impl {

class ConditionIsOpImpl : public Condition
{
protected:
    ConditionIsOpImpl(const std::string_view op_name);

public:
    ~ConditionIsOpImpl() noexcept override;

    void
    loadFromJson(const nlohmann::json & json) override;

protected:
    bool
    _eval(const Variables & variables) const;

    std::variant<std::unique_ptr<Condition>, utils::OptionValue> value_;
};

}  // namespace roah::distb::config::impl

#endif