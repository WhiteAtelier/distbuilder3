#include "app.hpp"

#include "app_config.hpp"
#include "errors.hpp"
#include "logger.hpp"
//
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <CLI/CLI.hpp>

#include <iostream>

namespace roah::distb::app {

class App::Impl_
{
public:
    Impl_();
    ~Impl_() noexcept;

    int
    run(int argc, const char * const argv[]);

private:
    void
    _setupCli(CLI::App & cli_app);

    void
    _initialize();

    bool        a_verbose_;
    std::string a_config_file_;
    std::string a_source_dir_;
    AppConfig   app_config_;
};

}  // namespace roah::distb::app

// ============================================================================================= //
// Constructor / Destructor
// ============================================================================================= //
roah::distb::app::App::Impl_::Impl_()
    : a_verbose_{ false }
    , a_source_dir_{ "." }
{}

roah::distb::app::App::App()
    : impl_(std::make_unique<Impl_>())
{}

roah::distb::app::App::Impl_::~Impl_() noexcept = default;

roah::distb::app::App::~App() noexcept = default;

// ============================================================================================= //
// [STATIC] run()
// ============================================================================================= //
int
roah::distb::app::App::Impl_::run(int argc, const char * const argv[])
{
    // Parse command-line
    CLI::App cli_app{ "distbuilder - A tool for building and distributing software packages." };
    this->_setupCli(cli_app);
    CLI11_PARSE(cli_app, argc, argv);

    // Set verbose logging
    logger.setVerbose(this->a_verbose_);

    // Load AppConfig
    if (!this->a_config_file_.empty())
    {
        this->app_config_.setFilePath(this->a_config_file_);
    }

    // Load configuration
    this->app_config_.load();

    // Validate/Initialize configuration
    this->_initialize();

    return 0;
}

int
roah::distb::app::App::run(int argc, const char * const argv[])
{
    try
    {
        App app;
        return app.impl_->run(argc, argv);
    }
    catch (const AppError & e)
    {
        logger.log("Error: {}", e.what());
    }
#ifndef _DEBUG
    catch (const std::runtime_error & e)
    {
        logger.log("Runtime Error: {}", e.what());
        return 2;
    }
    catch (const std::exception & e)
    {
        logger.log("Exception: {}", e.what());
        return 2;
    }
    catch (...)
    {
        logger.log("Unknown error occurred.");
        return 3;
    }
#endif
    return 1;
}

// ============================================================================================= //
// [PRIVATE] _setupCli()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_setupCli(CLI::App & cli_app)
{
    cli_app
        .add_option(  //
            "-c,--config",
            this->a_config_file_,
            "Path to the configuration file."
#ifdef ROAH_ARCH_WIN32
            " (default: %APPDATA%\\distbuilder.conf)"
#elif defined(ROAH_ARCH_MACOS)
#    error "unsupported platform"
#elif defined(ROAH_ARCH_LINUX)
#    error "unsupported platform"
#else
#    error "unsupported platform"
#endif
            )
        ->check(CLI::ExistingFile);

    cli_app  //
        .add_flag("-v,--verbose", this->a_verbose_, "Enable verbose logging.")
        ->default_val(this->a_verbose_);

    cli_app
        .add_option(  //
            "sourceDir",
            this->a_source_dir_,
            "Path to the source directory. (default: current directory.)")
        ->check(CLI::ExistingDirectory)
        ->default_val(this->a_source_dir_);
}

// ============================================================================================= //
// [PRIVATE] _initialize()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_initialize()
{
    const auto & build_dir   = this->app_config_.getBuildDirectory();
    const auto & install_dir = this->app_config_.getInstallDirectory();
    AppError::check(!build_dir.empty(), "Build directory is empty.");
    AppError::check(!install_dir.empty(), "Install directory is empty.");

    if (!std::filesystem::exists(build_dir))
    {
        logger.trace("Build directory does not exist. Creating: {}", build_dir.u8string());
        std::filesystem::create_directories(build_dir);
    }
    if (!std::filesystem::exists(install_dir))
    {
        logger.trace("Install directory does not exist. Creating: {}", install_dir.u8string());
        std::filesystem::create_directories(install_dir);
    }

    // cmake check
    AppError::check(!this->app_config_.getCMakeExecutable().empty(), "CMake executable is empty.");
    const auto cmake_ret = utils::run({ this->app_config_.getCMakeExecutable(), u8"--version" });
}
