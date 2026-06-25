#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_DOWNLOAD_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_DOWNLOAD_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

struct StepDownloadImplGenerator final : public StepGenerator
{
    std::unique_ptr<StepDef>
    operator()() const override;
};

class StepDownloadImpl final : public StepDef
{
public:
    using Generator                        = StepDownloadImplGenerator;
    constexpr static std::string_view kCmd = "download";

    StepDownloadImpl();

    StepDownloadImpl(const StepDownloadImpl &)     = default;
    StepDownloadImpl(StepDownloadImpl &&) noexcept = default;
    StepDownloadImpl &
    operator=(const StepDownloadImpl &)
        = default;
    StepDownloadImpl &
    operator=(StepDownloadImpl &&) noexcept
        = default;
    ~StepDownloadImpl() noexcept override;

    void
    operator()(const WorkingContext & context) const override;

    // 自身のディープコピーを生成して返す.
    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

    const std::string &
    getUrl() const noexcept;

    const std::string &
    getOutput() const noexcept;

    const std::string &
    getHash() const noexcept;

private:
    std::string
    _calculateHash(const std::filesystem::path & path) const;

    std::string url_;
    std::string output_;
    std::string hash_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
