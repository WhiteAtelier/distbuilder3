#ifndef ROAH_DISTB_APP_APP_CONFIG_HPP
#define ROAH_DISTB_APP_APP_CONFIG_HPP

#include <filesystem>
#include <string>
#include <vector>

namespace roah::distb::app {

class AppConfig
{
public:
    [[nodiscard]]
    explicit AppConfig();
    ~AppConfig() noexcept;

    AppConfig(const AppConfig &) = delete;
    AppConfig(AppConfig &&)      = delete;
    AppConfig &
    operator=(const AppConfig &)
        = delete;
    AppConfig &
    operator=(AppConfig &&)
        = delete;

    [[nodiscard]]
    const std::filesystem::path &
    getFilePath() const noexcept;

    void
    setFilePath(std::filesystem::path path);

    void
    load();

    const std::filesystem::path &
    getBuildDirectory() const noexcept;

    const std::filesystem::path &
    getInstallDirectory() const noexcept;

    const std::vector<std::filesystem::path> &
    getSearchPaths() const noexcept;

    const std::u8string &
    getCMakeExecutable() const noexcept;

    const std::u8string &
    getGenerator() const noexcept;

    const std::u8string &
    getArchitecture() const noexcept;

private:
    static std::filesystem::path
    _getDefaultFilePath();

    static std::filesystem::path
    _getDefaultBuildDirectory();

    static std::filesystem::path
    _getDefaultInstallDirectory();

    std::filesystem::path              file_path_;
    //
    std::filesystem::path              build_dir_;
    std::filesystem::path              install_dir_;
    std::vector<std::filesystem::path> search_paths_;
    std::u8string                      cmake_executable_;
    std::u8string                      generator_;
    std::u8string                      architecture_;
};

}  // namespace roah::distb::app

#endif
