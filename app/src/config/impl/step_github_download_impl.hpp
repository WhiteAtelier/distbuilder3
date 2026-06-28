#ifndef ROAH_DISTB_CONFIG_IMPL_STEP_GITHUB_DOWNLOAD_IMPL_HPP
#define ROAH_DISTB_CONFIG_IMPL_STEP_GITHUB_DOWNLOAD_IMPL_HPP

#include "roah/distb/config/step_def.hpp"

#include <nlohmann/json_fwd.hpp>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

namespace roah::distb::config::impl {

class StepDownloadImpl;
class StepExtractImpl;

class StepGithubDownloadImpl final : public StepDef
{
public:
    constexpr static std::string_view kCmd = "github-download";

    StepGithubDownloadImpl();

    StepGithubDownloadImpl(const StepGithubDownloadImpl &);
    StepGithubDownloadImpl(StepGithubDownloadImpl &&) noexcept;

    ~StepGithubDownloadImpl() noexcept override;

    void
    operator()(WorkingContext & context) const override;

    std::unique_ptr<StepDef>
    clone() const override;

    void
    loadFromJson(const nlohmann::json & json) override;

private:
    std::string output_dir_;    // 省略可能, 展開先, "src"
    std::string ref_;           // ダウンロードする GitHub の ref
    std::string hash_;          // SHA256 hash
    std::string author_;        // 省略可能, ${author}
    std::string repo_;          // 省略可能, ${repo}
    std::string archive_path_;  // 省略可能, "src.zip"
    bool        error_ok_;      // 省略可能, デフォルト false
};

}  // namespace roah::distb::config::impl

#endif  // ROAH_DISTB_CONFIG_IMPL_STEP_WGET_IMPL_HPP
