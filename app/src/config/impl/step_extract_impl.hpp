#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_EXTRACT_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_EXTRACT_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

class StepExtractImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "extract";

    StepExtractImpl();
    StepExtractImpl(const std::string_view cmd_name_driven_by,
                    std::string            input,
                    std::string            output,
                    const bool             verbosity,
                    const bool             error_ok);

    StepExtractImpl(const StepExtractImpl &);
    StepExtractImpl(StepExtractImpl &&) noexcept;

    ~StepExtractImpl() noexcept override;

    void
    operator()(WorkingContext & context) const override;

    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

private:
    std::string input_;
    std::string output_;
    bool        verbosity_;
    bool        error_ok_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
