#ifndef ROAH_DISTB_APP_DEPENDENCY_HPP
#define ROAH_DISTB_APP_DEPENDENCY_HPP

#include "roah/distb/config/library.hpp"
#include "roah/distb/utils/option_value.hpp"

#include <filesystem>
#include <map>
#include <string>
#include <unordered_map>

namespace roah::distb::config {
class Library;
class LibraryEntry;
}  // namespace roah::distb::config

namespace roah::distb::app {

class AppConfig;

class Dependency
{
public:
    Dependency(std::string name);
    Dependency(std::string author, std::string repo);
    Dependency(Dependency &&) noexcept;
    ~Dependency() noexcept;

    Dependency &
    operator=(Dependency &&) noexcept;

    Dependency(const Dependency &) = delete;
    Dependency &
    operator=(const Dependency &)
        = delete;

    const std::string &
    getAuthor() const noexcept;

    const std::string &
    getRepo() const noexcept;

    void
    setVersion(std::string version);

    const std::string &
    getVersion() const noexcept;

    bool
    checkVersionRange(const std::vector<std::string> & version_range);

    template <typename T>
    utils::OptionValue &
    setOption(std::string key, T value);

    const std::map<std::string, utils::OptionValue> &
    getOptions() const noexcept;

    void
    loadLibraryConfig(const AppConfig & app_config);

    const config::Library &
    getLibraryConfig() const noexcept;

    const config::LibraryEntry &
    getLibraryEntityConfigOfSelectedVersion() const;

    const std::string &
    getStateHash() const noexcept;

    void
    build(const AppConfig & app_config, const std::unordered_map<std::string, Dependency> & all_dependencies);

private:
    bool
    _loadLibraryConfig(const std::filesystem::path & path);

    void
    _calculateStateHash(const std::unordered_map<std::string, Dependency> & all_dependencies);

    std::string                               version_;
    std::map<std::string, utils::OptionValue> options_;
    config::Library                           library_conf_;
    bool                                      library_conf_loaded_;
    std::string                               state_hash_;
};

}  // namespace roah::distb::app

#endif