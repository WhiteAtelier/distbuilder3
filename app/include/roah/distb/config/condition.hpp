#ifndef ROAH_DISTB_CONFIG_CONDITION_HPP
#define ROAH_DISTB_CONFIG_CONDITION_HPP

#include "variables.hpp"

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <string>

namespace roah::distb::config {

class Condition
{
protected:
    Condition(const std::string_view op_name);

public:
    virtual ~Condition() noexcept;

    Condition(const Condition &) = delete;
    Condition(Condition &&)      = delete;
    Condition &
    operator=(const Condition &)
        = delete;
    Condition &
    operator=(Condition &&) noexcept
        = delete;

    std::string_view
    getOpName() const noexcept;

    virtual void
    loadFromJson(const nlohmann::json & json)
        = 0;

    virtual bool
    eval(const Variables & variables) const
        = 0;

private:
    std::string_view op_name_;
};

std::unique_ptr<Condition>
makeConditionFromJson(const nlohmann::json & json);

}  // namespace roah::distb::config

#endif
