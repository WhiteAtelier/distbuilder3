#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_DOWNLOAD_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_DOWNLOAD_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace roah::distb::config::impl {

class StepDownloadImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "download";

    StepDownloadImpl();
    StepDownloadImpl(const std::string_view     cmd_name_driven_by,
                     std::string                url,
                     std::string                output,
                     std::string                hash,
                     std::vector<std::u8string> curl_extra_args = {});

    StepDownloadImpl(const StepDownloadImpl &);
    StepDownloadImpl(StepDownloadImpl &&) noexcept;

    ~StepDownloadImpl() noexcept override;

    void
    operator()(WorkingContext & context) const override;

    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

    const std::string &
    getUrl() const noexcept;

    const std::string &
    getOutput() const noexcept;

    const std::string &
    getHashAsHexString() const noexcept;

private:
    std::string
    _calculateHash(const std::filesystem::path & path) const;

    std::string                url_;
    std::string                output_;
    std::string                hash_;
    //
    std::vector<std::u8string> curl_extra_args_;
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
