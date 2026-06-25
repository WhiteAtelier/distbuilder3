#include "app.hpp"

#include "app_config.hpp"
#include "dependency.hpp"
//
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <CLI/CLI.hpp>

#include <toml.hpp>

#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <unordered_map>
#include <unordered_set>

namespace roah::distb::app {

class App::Impl_
{
public:
    Impl_(std::filesystem::path executable_dir);
    ~Impl_() noexcept;

    bool
    hasDependency(const std::string_view name) const;

    Dependency &
    addDependency(std::string author, std::string repo);

    int
    run(int argc, const char * const argv[]);

private:
    void
    _setupCli(CLI::App & cli_app);

    void
    _initialize();

    void
    _parseDeps();

    void
    _resolveDeps(const std::vector<std::string> & library_names);

    void
    _buildDeps();

    using DependencyRef = std::reference_wrapper<Dependency>;

    bool                                           a_verbose_;
    std::string                                    a_config_file_;
    std::string                                    a_source_dir_;
    //
    AppConfig                                      app_config_;
    std::filesystem::path                          source_dir_;
    std::filesystem::path                          deps_file_path_;
    //
    std::unordered_map<std::string, DependencyRef> dependencies_;
    std::unordered_map<std::string, Dependency>    all_dependencies_;
};

}  // namespace roah::distb::app

// ============================================================================================= //
// Constructor / Destructor
// ============================================================================================= //
roah::distb::app::App::Impl_::Impl_(std::filesystem::path executable_dir)
    : a_verbose_{ false }
    , a_source_dir_{ "." }
    , app_config_{ std::move(executable_dir) }
{}

roah::distb::app::App::App(std::filesystem::path executable_dir)
    : impl_(std::make_unique<Impl_>(executable_dir))
{}

roah::distb::app::App::Impl_::~Impl_() noexcept = default;

roah::distb::app::App::~App() noexcept = default;

// ============================================================================================= //
// hasDependency()
// ============================================================================================= //
bool
roah::distb::app::App::Impl_::hasDependency(const std::string_view name) const
{
    return this->all_dependencies_.contains(std::string{ name });
}

bool
roah::distb::app::App::hasDependency(const std::string_view name) const
{
    return this->impl_->hasDependency(name);
}

// ============================================================================================= //
// addDependency()
// ============================================================================================= //
roah::distb::app::Dependency &
roah::distb::app::App::Impl_::addDependency(std::string author, std::string repo)
{
    auto key = author + "." + repo;
    return this->all_dependencies_.try_emplace(std::move(key), std::move(author), std::move(repo)).first->second;
}

roah::distb::app::Dependency &
roah::distb::app::App::addDependency(std::string author, std::string repo)
{
    return this->impl_->addDependency(std::move(author), std::move(repo));
}

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

    // Parse deps file
    this->_parseDeps();

    // Resolve dependencies
    const auto library_names_range = this->dependencies_ | std::ranges::views::keys;
    this->_resolveDeps({ library_names_range.begin(), library_names_range.end() });

    // Print: All Dependency
    for (const auto & dep : this->all_dependencies_ | std::ranges::views::values)
    {
        logger.log("Dependency: {}.{} (version: {})", dep.getAuthor(), dep.getRepo(), dep.getVersion());
    }

    return 0;
}

int
roah::distb::app::App::run(int argc, const char * const argv[])
{
    if (argc == 0) [[unlikely]]
    {
        logger.log("Error: Invalid argc ({})", argc);
        return 1;
    }

    try
    {
        App app{ std::filesystem::path{ argv[0] }.parent_path() };
        return app.impl_->run(argc, argv);
    }
    catch (const DistbuilderException & e)
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

    {
        // cmake check
        AppError::check(!this->app_config_.getCMakeExecutable().empty(), "CMake executable is empty.");
        const auto cmake_ret = utils::run({ this->app_config_.getCMakeExecutable(), u8"--version" },
                                          {
                                              .print_stdout   = false,
                                              .print_stderr   = false,
                                              .capture_stderr = false,
                                          });

        // cmake version 3.31.4
        std::regex  version_regex{ R"(cmake version ([^\s]+))" };
        std::smatch version_match;
        const auto  result = std::regex_search(cmake_ret.stdout_output, version_match, version_regex);
        AppError::check(result, "CMake \'{}\' is not executable.", this->app_config_.getCMakeExecutable());
        logger.log("CMake \'{}\' is available.", version_match[1].str());
    }

    // source directory check
    this->source_dir_ = std::filesystem::absolute(utils::toU8String(this->a_source_dir_)).make_preferred();
    AppError::check(std::filesystem::is_directory(this->source_dir_),
                    "Source directory does not exist: {}",
                    this->source_dir_.u8string());

    this->deps_file_path_ = this->source_dir_ / "deps.toml";
    AppError::check(std::filesystem::exists(this->deps_file_path_),
                    "{}/deps.toml is not found.",
                    this->source_dir_.u8string());
}

// ============================================================================================= //
// [PRIVATE] _parseDeps()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_parseDeps()
{
    // ifstream をそのまま toml11 に送るとエラーになった.
    // バグの疑い, いったん文字列に読みだしてから parse する.
    std::string content;
    {
        auto ifst = std::ifstream{ this->deps_file_path_ };
        AppError::check(ifst, "Failed to open deps.toml: {}", this->deps_file_path_.u8string());
        content = std::string{ (std::istreambuf_iterator<char>(ifst)), std::istreambuf_iterator<char>() };
    }

    try
    {
        const auto root = toml::parse_str(content);
        for (const auto & [author, t_author] : root.as_table())
        {
            for (const auto & [repo, t_repo] : t_author.as_table())
            {
                logger.log("[ Dependency ] Author: {}, Repo: {}", author, repo);
                auto & dep = this->all_dependencies_.try_emplace(author + "." + repo, author, repo).first->second;
                this->dependencies_.try_emplace(author + "." + repo, dep);

                for (const auto & [key, t_value] : t_repo.as_table())
                {
                    if (key == "version")
                    {
                        dep.setVersion(t_repo.at("version").as_string());
                        logger.trace("-- Version: {}", t_repo.at("version").as_string());
                    }
                    else if (t_value.is_string())
                    {
                        dep.setOption<std::string>(key, t_value.as_string());
                        logger.trace("-- Option: {} = {}", key, t_value.as_string());
                    }
                    else if (t_value.is_integer())
                    {
                        dep.setOption<std::int64_t>(key, t_value.as_integer());
                        logger.trace("-- Option: {} = {}", key, t_value.as_integer());
                    }
                    else if (t_value.is_boolean())
                    {
                        dep.setOption<bool>(key, t_value.as_boolean());
                        logger.trace("-- Option: {} = {}", key, t_value.as_boolean());
                    }
                    else if (t_value.is_floating())
                    {
                        dep.setOption<double>(key, t_value.as_floating());
                        logger.trace("-- Option: {} = {}", key, t_value.as_floating());
                    }
                    else
                    {
                        AppError::throw_("Invalid option value type for {}.{}: {}", author, repo, key);
                    }
                }
            }
        }
    }
    catch (const toml::exception & e)
    {
        AppError::throw_("Failed to parse deps.toml: {}", e.what());
    }
}

// ============================================================================================= //
// [PRIVATE] _resolveDeps()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_resolveDeps(const std::vector<std::string> & library_names)
{
    std::vector<std::string> new_library_names;
    for (const auto & name : library_names)
    {
        auto & dep = this->all_dependencies_.at(name);
        dep.loadLibraryConfig(this->app_config_);

        // そのバージョンでの dependencies をキューに追加する.
        const auto & le         = dep.getLibraryEntityConfigOfSelectedVersion();
        const auto & child_deps = le.getDependencies();
        for (const auto & child_dep : child_deps | std::ranges::views::values)
        {
            // todo: condition 評価
            if (const auto [iter, added]
                = this->all_dependencies_.try_emplace(child_dep.getName(), child_dep.getName());
                !added)
            {
                // バージョンチェック
                if (!iter->second.checkVersionRange(child_dep.getRequiredVersionRange()))
                {
                    throw DependencyResolveError{
                        "Dependency {}.{}: Version '{}' is not compatible with the required range.",
                        iter->second.getAuthor(),
                        iter->second.getRepo(),
                        iter->second.getVersion()
                    };
                }
            }
            else
            {
                // 新しく追加された.
                new_library_names.emplace_back(child_dep.getName());
            }
        }
    }

    if (!new_library_names.empty())
    {
        this->_resolveDeps(new_library_names);
    }
}

// ============================================================================================= //
// [PRIVATE] _buildDeps()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_buildDeps()
{
    // 依存のクリアしたものからビルドしていく.
    std::unordered_set<std::string> builts;
    bool                            completed = false;

    while (!completed)
    {
        completed = true;

        for (const auto & dep : this->all_dependencies_ | std::ranges::views::values)
        {
            const auto & le = dep.getLibraryEntityConfigOfSelectedVersion();
            le.getDependencies();
            //
        }
    }
}
