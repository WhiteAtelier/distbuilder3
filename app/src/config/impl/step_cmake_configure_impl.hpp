#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_CMAKE_CONFIGURE_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_CMAKE_CONFIGURE_IMPL_HPP

#include "roah/distb/config/step_def.hpp"
#include "roah/distb/utils/delay_copyable_container.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace roah::distb::config {
class Condition;
}  // namespace roah::distb::config

namespace roah::distb::config::impl {

class StepCMakeConfigureImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "cmake-configure";

    StepCMakeConfigureImpl();

    StepCMakeConfigureImpl(const StepCMakeConfigureImpl &);
    StepCMakeConfigureImpl(StepCMakeConfigureImpl &&) noexcept;

    ~StepCMakeConfigureImpl() noexcept override;

    std::unique_ptr<StepDef>
    clone() const override;

private:
    void
    _execute(WorkingContext & context) const override;

    void
    _loadFromJson(const nlohmann::json & json) override;

    using ConditionHolder = utils::DelayCopyableContainer<Condition>;

    struct ArgsSubset
    {
        std::vector<std::string> args;
        ConditionHolder          condition;
    };

    std::string                       source_dir_;
    std::string                       build_dir_;
    std::string                       debug_postfix_;
    std::map<std::string, ArgsSubset> args_;
    std::vector<std::string>          configs_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
