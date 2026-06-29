#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_CMAKE_INSTALL_ALL_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_CMAKE_INSTALL_ALL_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

class StepCMakeInstallAllImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "cmake-install-all";

    StepCMakeInstallAllImpl();

    StepCMakeInstallAllImpl(const StepCMakeInstallAllImpl &);
    StepCMakeInstallAllImpl(StepCMakeInstallAllImpl &&) noexcept;

    ~StepCMakeInstallAllImpl() noexcept override;

    std::unique_ptr<StepDef>
    clone() const override;

private:
    void
    _execute(WorkingContext & context) const override;

    void
    _loadFromJson(const nlohmann::json & json) override;

    std::string              build_dir_;
    std::vector<std::string> configs_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
