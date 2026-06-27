#ifndef ROAH_DISTB_CONFIG_IMPL_CONDITION_AND_OR_OP_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_CONDITION_AND_OR_OP_IMPL_HPP

#include "roah/distb/config/condition.hpp"

#include <memory>
#include <vector>

namespace roah::distb::config::impl {

class ConditionAndOrOpImpl : public Condition
{
protected:
    ConditionAndOrOpImpl(const std::string_view op_name);

public:
    ~ConditionAndOrOpImpl() noexcept override;

    void
    loadFromJson(const nlohmann::json & json) override final;

protected:
    std::vector<std::unique_ptr<Condition>> children_;
};

}  // namespace roah::distb::config::impl

#endif