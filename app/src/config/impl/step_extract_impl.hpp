#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_EXTRACT_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_EXTRACT_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

struct StepExtractImplGenerator final : public StepGenerator
{
    std::unique_ptr<StepDef>
    operator()() const override;
};

class StepExtractImpl final : public StepDef
{
public:
    using Generator                        = StepExtractImplGenerator;
    constexpr static std::string_view kCmd = "extract";

    StepExtractImpl();

    StepExtractImpl(const StepExtractImpl &);
    StepExtractImpl(StepExtractImpl &&) noexcept;
    StepExtractImpl &
    operator=(const StepExtractImpl &);
    StepExtractImpl &
    operator=(StepExtractImpl &&) noexcept;
    ~StepExtractImpl() noexcept override;

    void
    operator()(const WorkingContext & context) const override;

    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

private:
    std::string input_;
    std::string output_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
