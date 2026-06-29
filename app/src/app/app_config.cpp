#include "app_config.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/string.hpp"

#include <toml.hpp>

#include <cstring>
#include <fstream>
#include <iostream>

roah::distb::app::AppConfig::AppConfig(std::filesystem::path executable_dir)
    : executable_dir_{ std::move(executable_dir) }
    , app_storage_path_{ _getAppStoragePath() }
    , default_search_path_{ this->app_storage_path_ / "libs" }
    , file_path_{ _getDefaultFilePath() }
    , build_dir_{ _getDefaultBuildDirectory() }
    , install_dir_{ _getDefaultInstallDirectory() }
    , search_paths_{ { this->default_search_path_ } }
    , cmake_executable_{ u8"cmake" }
    , generator_{}
    , architecture_{}
    , cmake_presets_default_build_dir_{ u8"${sourceDir}/build" }
    , cmake_presets_default_install_dir_{ u8"${sourceDir}/install" }
{}

roah::distb::app::AppConfig::~AppConfig() noexcept = default;

const std::filesystem::path &
roah::distb::app::AppConfig::getFilePath() const noexcept
{
    return this->file_path_;
}

void
roah::distb::app::AppConfig::setFilePath(std::filesystem::path path)
{
    this->file_path_ = std::move(path);
}

void
roah::distb::app::AppConfig::load()
{
    if (!std::filesystem::exists(this->file_path_))
    {
        logger.trace("Config file does not exist, generating template. {}", this->file_path_.string());
        this->_createTemplate();
    }

    logger.trace("Open config file. {}", this->file_path_.string());

    std::string content;
    {
        auto ifst = std::ifstream{ this->file_path_ };
        AppError::check(ifst, "Failed to open config file: {}", this->file_path_.u8string());
        content = std::string{ (std::istreambuf_iterator<char>(ifst)), std::istreambuf_iterator<char>() };
    }

    logger.trace("-- Opened.");

    this->search_paths_.clear();

    try
    {
        const auto root = toml::parse_str(content);

        if (root.contains("directory"))
        {
            const auto & t_directory = root.at("directory");
            if (t_directory.contains("build"))
            {
                if (auto path = utils::toU8String(t_directory.at("build").as_string()); !path.empty())
                {
                    this->build_dir_ = std::move(path);
                }
            }
            if (t_directory.contains("install"))
            {
                if (auto path = utils::toU8String(t_directory.at("install").as_string()); !path.empty())
                {
                    this->install_dir_ = std::move(path);
                }
            }
            if (t_directory.contains("search_paths"))
            {
                const auto & spaths = t_directory.at("search_paths").as_array();
                for (const auto & t_path : spaths)
                {
                    this->search_paths_.emplace_back(utils::toU8String(t_path.as_string()));
                }
            }
        }

        if (root.contains("cmake"))
        {
            const auto & t_cmake = root.at("cmake");
            if (t_cmake.contains("executable"))
            {
                if (auto path = utils::toU8String(t_cmake.at("executable").as_string()); !path.empty())
                {
                    this->cmake_executable_ = std::move(path);
                }
            }
            if (t_cmake.contains("generator"))
            {
                if (auto path = utils::toU8String(t_cmake.at("generator").as_string()); !path.empty())
                {
                    this->generator_ = std::move(path);
                }
            }
            if (t_cmake.contains("architecture"))
            {
                if (auto path = utils::toU8String(t_cmake.at("architecture").as_string()); !path.empty())
                {
                    this->architecture_ = std::move(path);
                }
            }
        }

        if (root.contains("cmake_presets"))
        {
            const auto & t_cmake_presets = root.at("cmake_presets");
            if (t_cmake_presets.contains("default_build_dir"))
            {
                this->cmake_presets_default_build_dir_  //
                    = utils::toU8String(t_cmake_presets.at("default_build_dir").as_string());
            }
            if (t_cmake_presets.contains("default_install_dir"))
            {
                this->cmake_presets_default_install_dir_
                    = utils::toU8String(t_cmake_presets.at("default_install_dir").as_string());
            }
        }

        if (root.contains("access_token"))
        {
            const auto & t_access_token = root.at("access_token");
            for (const auto & [key, value] : t_access_token.as_table())
            {
                auto & secrets = this->access_tokens_.try_emplace(key, key).first->second;
                if (value.is_string())
                {
                    secrets.addSecret(value.as_string());
                }
                else if (value.is_table())
                {
                    for (const auto & [k, v] : value.as_table())
                    {
                        if (v.is_string())
                        {
                            secrets.addSecret(k, v.as_string());
                        }
                    }
                }
            }
        }
    }
    catch (const toml::exception & e)
    {
        AppError::throw_("Failed to parse config file '{}': {}", this->file_path_.string(), e.what());
    }

    this->build_dir_   = std::filesystem::absolute(this->build_dir_).make_preferred();
    this->install_dir_ = std::filesystem::absolute(this->install_dir_).make_preferred();
    this->search_paths_.emplace_back(this->default_search_path_);

    logger.trace("-- Done.");
    logger.trace("[   Directory   ] Build Directory           = {}", this->build_dir_.u8string());
    logger.trace("[   Directory   ] Install Directory         = {}", this->install_dir_.u8string());
    for (auto & p : this->search_paths_)
    {
        p = std::filesystem::absolute(p).make_preferred();
        logger.trace("[   Directory   ] Search Path               = {}", p.u8string());
    }
    logger.trace("[     CMake     ] Executable                = {}", this->cmake_executable_);
    logger.trace("[     CMake     ] Generator                 = {}", this->generator_);
    logger.trace("[     CMake     ] Architecture              = {}", this->architecture_);
    logger.trace("[ CMake Presets ] Default Build Directory   = {}", this->cmake_presets_default_build_dir_);
    logger.trace("[ CMake Presets ] Default Install Directory = {}", this->cmake_presets_default_install_dir_);
}

void
roah::distb::app::AppConfig::_createTemplate()
{
    const auto create_item = [](auto v, std::vector<std::string> comments) {
        toml::value t_v = std::move(v);
        t_v.comments()  = std::move(comments);
        return t_v;
    };

    toml::value root{
        toml::table{
            {
                "directory",
                toml::table{
                    { "build",
                      create_item("",
                                  { " Path to build directory.",  //
                                    " -- default: " + utils::toString(this->build_dir_.u8string()) }) },
                    { "install",
                      create_item("",
                                  { " Path to install directory.",  //
                                    " -- default: " + utils::toString(this->install_dir_.u8string()) }) },
                    { "search_paths",  //
                      create_item(toml::array{}, { " Extra search paths." }) },
                },
            },
            {
                "cmake",
                toml::table{
                    { "executable",
                      create_item("cmake",
                                  { " Path to cmake executable.",  //
                                    " -- default: \"cmake\"" }) },
                    { "generator",
                      create_item("",
                                  { " CMake generator name.",  //
                                    " -- default: (CMake default generator.)" }) },
                    { "architecture",
                      create_item("",
                                  { " Architecture name.",  //
                                    " -- default: (CMake default architecture.)" }) },
                },
            },
            {
                "cmake_presets",
                toml::table{
                    { "default_build_dir",
                      create_item(
                          "${sourceDir}/build",
                          { " Default \"buildDir\" value of cmake preset.",  //
                            " -- default: ${sourceDir}/build",
                            " When distbuilder creates new cmake configure preset, this settings is used as \"binaryDir\" value.",
                            " \"buildDir\" key is only defined if this value is non-empty." }) },
                    { "default_install_dir",
                      create_item(
                          "${sourceDir}/install",
                          { " Default \"installDir\" value of cmake preset.",  //
                            " -- default: ${sourceDir}/install",
                            " When distbuilder creates new cmake configure preset, this settings is used as \"installDir\" value.",
                            " \"installDir\" key is only defined if this value is non-empty." }) },
                },
            },
            {
                "access_token",
                toml::table{
                    { "github",
                      create_item(
                          "",
                          { " (Optional) GitHub public access token.",  //
                            " -- This token is used to access GitHub API for public or users private repositories.",
                            " -- Get token: https://github.com/settings/personal-access-tokens",
                            " -- Required permissions: Contents: Read-only, Metadata: Read-only",
                            " -- default: (empty)" }) },
                },
            },
        },
    };

    std::ofstream ofst{ this->file_path_ };
    AppError::check(ofst, "Failed to create config file: {}", this->file_path_.u8string());
    ofst << toml::format(root) << std::endl;
}

const std::filesystem::path &
roah::distb::app::AppConfig::getExecutableDirectory() const noexcept
{
    return this->executable_dir_;
}

const std::filesystem::path &
roah::distb::app::AppConfig::getAppStoragePath() const noexcept
{
    return this->app_storage_path_;
}

const std::filesystem::path &
roah::distb::app::AppConfig::getBuildDirectory() const noexcept
{
    return this->build_dir_;
}

const std::filesystem::path &
roah::distb::app::AppConfig::getInstallDirectory() const noexcept
{
    return this->install_dir_;
}

const std::vector<std::filesystem::path> &
roah::distb::app::AppConfig::getSearchPaths() const noexcept
{
    return this->search_paths_;
}

const std::u8string &
roah::distb::app::AppConfig::getCMakeExecutable() const noexcept
{
    return this->cmake_executable_;
}

const std::u8string &
roah::distb::app::AppConfig::getGenerator() const noexcept
{
    return this->generator_;
}

const std::u8string &
roah::distb::app::AppConfig::getArchitecture() const noexcept
{
    return this->architecture_;
}

const std::u8string &
roah::distb::app::AppConfig::getCMakePresetsDefaultBuildDir() const noexcept
{
    return this->cmake_presets_default_build_dir_;
}

const std::u8string &
roah::distb::app::AppConfig::getCMakePresetsDefaultInstallDir() const noexcept
{
    return this->cmake_presets_default_install_dir_;
}

const std::string &
roah::distb::app::AppConfig::getAccessToken(const std::string & site, const std::string & key) const noexcept
{
    const auto it = this->access_tokens_.find(site);
    if (it != this->access_tokens_.end())
    {
        return it->second.getSecret(key);
    }
    static const std::string empty{};
    return empty;
}
