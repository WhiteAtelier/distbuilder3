#include "app_config.hpp"

#include "errors.hpp"
#include "logger.hpp"
//
#include "roah/distb/utils/string.hpp"

#include <toml.hpp>

#include <cstring>
#include <fstream>
#include <iostream>

roah::distb::app::AppConfig::AppConfig()
    : file_path_{ _getDefaultFilePath() }
    , build_dir_{ _getDefaultBuildDirectory() }
    , install_dir_{ _getDefaultInstallDirectory() }
    , cmake_executable_{ u8"cmake" }
    , generator_{}
    , architecture_{}
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
    logger.trace("Open config file. {}", this->file_path_.string());

    std::ifstream ifst{ this->file_path_ };
    AppError::check(ifst, "Failed to open config file: {}", this->file_path_.string());

    logger.trace("-- Opened.");

    try
    {
        const auto root = toml::parse(ifst);

        if (root.contains("directory"))
        {
            logger.trace("-- [directory] section");
            const auto & t_directory = root.at("directory");
            if (t_directory.contains("build"))
            {
                this->build_dir_ = utils::toU8String(t_directory.at("build").as_string());
                logger.trace("---- buildDir: {}", this->build_dir_.u8string());
            }
            if (t_directory.contains("install"))
            {
                this->install_dir_ = utils::toU8String(t_directory.at("install").as_string());
                logger.trace("---- installDir: {}", this->install_dir_.u8string());
            }
            if (t_directory.contains("search_paths"))
            {
                const auto & spaths = t_directory.at("search_paths").as_array();
                for (const auto & t_path : spaths)
                {
                    auto & p = this->search_paths_.emplace_back(utils::toU8String(t_path.as_string()));
                    logger.trace("---- searchPath: {}", p.u8string());
                }
            }
        }

        if (root.contains("cmake"))
        {
            logger.trace("-- [cmake] section");
            const auto & t_cmake = root.at("cmake");
            if (t_cmake.contains("path"))
            {
                this->cmake_executable_ = utils::toU8String(t_cmake.at("path").as_string());
                logger.trace("---- executable: {}", this->cmake_executable_);
            }
            if (t_cmake.contains("generator"))
            {
                this->generator_ = utils::toU8String(t_cmake.at("generator").as_string());
                logger.trace("---- generator: {}", this->generator_);
            }
            if (t_cmake.contains("architecture"))
            {
                this->architecture_ = utils::toU8String(t_cmake.at("architecture").as_string());
                logger.trace("---- architecture: {}", this->architecture_);
            }
        }
    }
    catch (const toml::exception & e)
    {
        AppError::throw_("Failed to parse config file '{}': {}", this->file_path_.string(), e.what());
    }

    this->build_dir_   = std::filesystem::absolute(this->build_dir_).make_preferred();
    this->install_dir_ = std::filesystem::absolute(this->install_dir_).make_preferred();

    logger.trace("-- Done.");
    logger.trace("[ Directory ] Build Directory   = {}", this->build_dir_.u8string());
    logger.trace("[ Directory ] Install Directory = {}", this->install_dir_.u8string());
    for (auto & p : this->search_paths_)
    {
        p = std::filesystem::absolute(p).make_preferred();
        logger.trace("[ Directory ] Search Path       = {}", p.u8string());
    }
    logger.trace("[   CMake   ] Executable        = {}", this->cmake_executable_);
    logger.trace("[   CMake   ] Generator         = {}", this->generator_);
    logger.trace("[   CMake   ] Architecture      = {}", this->architecture_);
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

std::filesystem::path
roah::distb::app::AppConfig::_getDefaultBuildDirectory()
{
    return std::filesystem::temp_directory_path() / "distb_buildtmp";
}
