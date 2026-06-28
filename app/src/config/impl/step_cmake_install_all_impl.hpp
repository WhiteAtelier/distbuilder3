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

    void
    operator()(WorkingContext & context) const override;

    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

private:
    std::string build_dir_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
