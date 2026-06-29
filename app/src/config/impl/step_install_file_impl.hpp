#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_INSTALL_FILE_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_INSTALL_FILE_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

class StepInstallFileImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "install-file";

    StepInstallFileImpl();

    StepInstallFileImpl(const StepInstallFileImpl &);
    StepInstallFileImpl(StepInstallFileImpl &&) noexcept;

    ~StepInstallFileImpl() noexcept override;

    std::unique_ptr<StepDef>
    clone() const override;

private:
    void
    _execute(WorkingContext & context) const override;

    void
    _loadFromJson(const nlohmann::json & json) override;

    std::set<std::string> source_file_;
    std::string           destination_dir_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
