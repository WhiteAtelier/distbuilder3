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
    Condition(std::string op_name);

public:
    ~Condition() = default;

    Condition(const Condition &)     = delete;
    Condition(Condition &&) noexcept = delete;
    Condition &
    operator=(const Condition &)
        = default;
    Condition &
    operator=(Condition &&) noexcept
        = delete;

    const std::string &
    getOpName() const noexcept;

    virtual void
    loadFromJson(const nlohmann::json & json)
        = 0;

    virtual bool
    eval(const Variables & variables) const
        = 0;

    virtual Condition
    clone() const
        = 0;

private:
    std::string op_name_;
};

struct ConditionGenerator
{
    virtual std::unique_ptr<Condition>
    operator()() const = 0;
};

}  // namespace roah::distb::config

#endif
