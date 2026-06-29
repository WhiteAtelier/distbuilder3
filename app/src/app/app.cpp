#include "app.hpp"

#include "app_config.hpp"
#include "dependency.hpp"
#include "library_store.hpp"
#include "working_context_impl.hpp"
//
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include <toml.hpp>

#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <regex>
#include <set>
#include <sstream>
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
    struct NewDependencyRequest
    {
        std::string       name;
        config::Variables override_options;
    };

    void
    _setupCli(CLI::App & cli_app);

    void
    _checkCMake();

    void
    _initialize();

    void
    _parseDeps();

    void
    _resolveDeps();

    void
    _resolveDeps(const std::vector<NewDependencyRequest> & new_libraries);

    void
    _buildDeps();

    void
    _exportFullDepsToml() const;

    void
    _exportCMakePresets() const;

    void
    _gatherLicenseFiles() const;

    using DependencyRef = std::reference_wrapper<Dependency>;

    bool                                           a_verbose_;
    bool                                           a_force_build_;
    bool                                           a_no_build_;
    bool                                           a_check_conf_;
    std::string                                    a_config_file_;
    std::string                                    a_source_dir_;
    std::string                                    a_cmake_preset_file_;
    std::optional<std::vector<std::string>>        a_cmake_preset_names_;
    std::string                                    a_licenses_dir_;
    //
    AppConfig                                      app_config_;
    std::filesystem::path                          source_dir_;
    std::filesystem::path                          deps_file_path_;
    std::filesystem::path                          cmake_preset_file_path_;
    std::string                                    cxx_standard_;
    //
    LibraryStore                                   library_store_;
    //
    CMakeVersion                                   cmake_version_;
    //
    std::unordered_map<std::string, DependencyRef> dependencies_;
    std::unordered_map<std::string, Dependency>    all_dependencies_;
};

}  // namespace roah::distb::app

roah::distb::app::CMakeVersion::CMakeVersion() noexcept
    : major{ 0 }
    , minor{ 0 }
    , patch{ 0 }
{}

// ============================================================================================= //
// Constructor / Destructor
// ============================================================================================= //
roah::distb::app::App::Impl_::Impl_(std::filesystem::path executable_dir)
    : a_verbose_{ false }
    , a_force_build_{ false }
    , a_no_build_{ false }
    , a_check_conf_{ false }
    , a_source_dir_{ "." }
    , a_cmake_preset_file_{ "./CMakeUserPresets.json" }
    , app_config_{ std::move(executable_dir) }
    , library_store_{ this->app_config_ }
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
    // -- check-conf の場合も verbose にする
    logger.setVerbose(this->a_verbose_ || this->a_check_conf_);

    // Load AppConfig
    if (!this->a_config_file_.empty())
    {
        this->app_config_.setFilePath(this->a_config_file_);
    }

    // Load configuration
    this->app_config_.load();

    this->_checkCMake();

    if (this->a_check_conf_)
    {
        return 0;
    }

    // Validate/Initialize configuration
    this->_initialize();

    // Check update of libraries
    this->library_store_.fetch();

    // Parse deps file
    this->_parseDeps();

    // Resolve dependencies
    this->_resolveDeps();

    // Print: All Dependency
    for (const auto & dep : this->all_dependencies_ | std::ranges::views::values)
    {
        logger.log("Dependency: {}.{} (version: {})", dep.getAuthor(), dep.getRepo(), dep.getVersion());
    }

    // build
    this->_buildDeps();

    // Update deps.full.toml
    this->_exportFullDepsToml();

    // Export CMakePresets
    this->_exportCMakePresets();

    // Gather license files
    this->_gatherLicenseFiles();

    logger.log("Completed!");
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
    catch (const std::filesystem::filesystem_error & e)
    {
        logger.log("Filesystem Error: {}", e.what());
    }
    catch (const std::runtime_error & e)
    {
        logger.log("Runtime Error: {}", e.what());
    }
    catch (const std::exception & e)
    {
        logger.log("Exception: {}", e.what());
    }
    catch (...)
    {
        logger.log("Unknown error occurred.");
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
    cli_app.set_version_flag("--version",
                             std::format("distbuilder {}.{}.{}",
                                         DISTBUILDER3_VERSION_MAJOR,
                                         DISTBUILDER3_VERSION_MINOR,
                                         DISTBUILDER3_VERSION_PATCH));

    cli_app.add_option(  //
        "-c,--config",
        this->a_config_file_,
        "Path to the configuration file."
#ifdef ROAH_ARCH_WIN32
        " (default: %USERPROFILE%\\distbuilder.conf)"
#elif defined(ROAH_ARCH_MACOS)
#    error "unsupported platform"
#elif defined(ROAH_ARCH_LINUX)
#    error "unsupported platform"
#else
#    error "unsupported platform"
#endif
    );

    auto * root_grp = cli_app.add_option_group("distbuilder");

    auto * cconf_grp = root_grp->add_option_group("Check config mode", "Options for checking the user conf file.");
    cconf_grp->footer(" ");  // Empty Line
    cconf_grp->add_flag("--check-conf", this->a_check_conf_, "Check configuration file.");

    auto * build_grp = root_grp->add_option_group("Build mode", "Main distbuilder options");
    build_grp->footer(" ");  // Empty Line

    build_grp  //
        ->add_flag("-v,--verbose", this->a_verbose_, "Enable verbose logging.")
        ->default_val(this->a_verbose_)
        ->capture_default_str();

    build_grp  //
        ->add_flag("-f,--force", this->a_force_build_, "Force (re)build.")
        ->default_val(this->a_force_build_)
        ->capture_default_str();

    build_grp  //
        ->add_flag("--no-build", this->a_no_build_, "Skip build step.")
        ->default_val(this->a_no_build_)
        ->capture_default_str();

    build_grp  //
        ->add_option("--export-presets", this->a_cmake_preset_names_, "CMake preset configure name(s) to setup.")
        ->expected(0, 256)
        ->default_str("$");

    build_grp  //
        ->add_option("--preset-file", this->a_cmake_preset_file_, "CMake preset file to use.")
        ->default_val(this->a_cmake_preset_file_)
        ->capture_default_str();

    build_grp  //
        ->add_option("--gather-licenses", this->a_licenses_dir_, "Directory to export licenses.")
        ->default_val(this->a_licenses_dir_);

    build_grp  //
        ->add_option("sourceDir", this->a_source_dir_, "Path to the source directory.")
        ->check(CLI::ExistingDirectory)
        ->required(true);

    root_grp->require_option(1);
}

// ============================================================================================= //
// [PRIVATE] _checkCMake()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_checkCMake()
{
    // cmake check
    AppError::check(!this->app_config_.getCMakeExecutable().empty(), "CMake executable is empty.");
    const auto cmake_ret = utils::run({ this->app_config_.getCMakeExecutable(), u8"--version" },
                                      {
                                          .print_stdout   = false,
                                          .print_stderr   = false,
                                          .capture_stderr = false,
                                      });

    std::regex  version_regex{ R"(cmake version (\d+)\.(\d+).(\d+))" };
    std::smatch version_match;
    const auto  result = std::regex_search(cmake_ret.stdout_output, version_match, version_regex);
    AppError::check(result, "CMake \'{}\' is not executable.", this->app_config_.getCMakeExecutable());

    this->cmake_version_.major = std::stoul(version_match[1].str());
    this->cmake_version_.minor = std::stoul(version_match[2].str());
    this->cmake_version_.patch = std::stoul(version_match[3].str());

    if (!(this->cmake_version_.major >= 3 && this->cmake_version_.minor >= 19))
    {
        // 最低バージョンは 3.19.0
        // CMakePresets の最低サポート
        throw AppError{ "CMake version {}.{}.{} is not supported. Minimum required version is 3.19.0.",
                        this->cmake_version_.major,
                        this->cmake_version_.minor,
                        this->cmake_version_.patch };
    }

    logger.log("CMake \'{}.{}.{}\' is available.",
               this->cmake_version_.major,
               this->cmake_version_.minor,
               this->cmake_version_.patch);
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

    if (this->a_cmake_preset_names_)
    {
        // CMakePreset 出力する
        this->cmake_preset_file_path_ = utils::makeAbsolutePath(this->a_cmake_preset_file_);
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

        if (root.contains("cxx_standard"))
        {
            const auto & t_cxx_standard = root.at("cxx_standard");
            AppError::check(t_cxx_standard.is_string() || t_cxx_standard.is_integer(),
                            "deps.toml: cxx_standard must be a integer or string.");
            if (t_cxx_standard.is_string())
            {
                this->cxx_standard_ = t_cxx_standard.as_string();
            }
            else if (t_cxx_standard.is_integer())
            {
                this->cxx_standard_ = std::to_string(t_cxx_standard.as_integer());
            }
        }

        for (const auto & [author, t_author] : root.as_table())
        {
            if (!t_author.is_table())
            {
                continue;
            }
            for (const auto & [repo, t_repo] : t_author.as_table())
            {
                logger.log("[ Dependency ] Author: {}, Repo: {}", author, repo);
                auto & dep = this->all_dependencies_.try_emplace(author + "." + repo, author, repo).first->second;
                this->dependencies_.try_emplace(author + "." + repo, dep);

                if (t_repo.contains("version"))
                {
                    const auto & t_version = t_repo.at("version");
                    AppError::check(t_version.is_string(),
                                    "deps.toml: version must be a string for {}.{}",
                                    author,
                                    repo);
                    dep.setVersion(t_version.as_string());
                    logger.trace("-- Selected version: {}", dep.getVersion());
                }
                if (t_repo.contains("options"))
                {
                    const auto & t_options = t_repo.at("options");
                    AppError::check(t_options.is_table(), "deps.toml: options must be a table for {}.{}", author, repo);

                    for (const auto & [key, t_value] : t_options.as_table())
                    {
                        if (key.starts_with("_"))
                        {
                            // hidden option, 指定もできない.
                            throw AppError{ "{}: hidden option '{}' cannot be specified for {}.{}",
                                            this->deps_file_path_.filename().u8string(),
                                            key,
                                            author,
                                            repo };
                        }
                        if (t_value.is_string())
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
roah::distb::app::App::Impl_::_resolveDeps()
{
    std::vector<NewDependencyRequest> new_libraries;
    new_libraries.reserve(this->dependencies_.size());

    for (const auto & root_dep_name : this->dependencies_ | std::ranges::views::keys)
    {
        new_libraries.emplace_back(NewDependencyRequest{ .name = root_dep_name });
    }
    this->_resolveDeps(new_libraries);
}

void
roah::distb::app::App::Impl_::_resolveDeps(const std::vector<NewDependencyRequest> & libraries)
{
    std::vector<NewDependencyRequest> new_libraries;
    for (const auto & library : libraries)
    {
        auto & dep = this->all_dependencies_.at(library.name);
        dep.loadLibraryConfig(this->app_config_, library.override_options);
        const auto & dep_ds = dep.getLibraryEntityConfigOfSelectedVersion().getDependencies();

        for (const auto & child_dep_name : dep.getResolvedDependencies())
        {
            if (this->all_dependencies_.try_emplace(child_dep_name, child_dep_name).second)
            {
                // 新しく追加された.

                // 依存先が, ほかのライブラリに使われていないときに限って,
                // override option がそのまま適用されるようにする.
                // もしくは依存先が複数のライブラリに使われている場合,
                // すでに設定されている option と同じでなければエラーになる.
                new_libraries.emplace_back(NewDependencyRequest{
                    .name             = child_dep_name,
                    .override_options = dep_ds.at(child_dep_name).getOptions(),
                });
            }
            // else:
            // すでに追加されている場合, override option が同じでなければエラーになる.
            // ただし, option の値そのものは variables で評価しなければならないので, ここではチェックは行わない.
            // build() 時に判定する.
        }
    }

    if (!new_libraries.empty())
    {
        this->_resolveDeps(new_libraries);
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
    bool                            completed    = false;
    auto                            builts_count = builts.size();

    while (!completed)
    {
        completed = true;

        for (auto & [name, dep] : this->all_dependencies_)
        {
            if (builts.contains(name))
            {
                continue;
            }

            bool can_build = true;
            for (const auto & child_dep_name : dep.getResolvedDependencies())
            {
                if (!builts.contains(child_dep_name))
                {
                    can_build = false;
                    break;
                }
            }

            if (can_build)
            {
                logger.log("Building dependency: {}.{} (version: {})",
                           dep.getAuthor(),
                           dep.getRepo(),
                           dep.getVersion());

                try
                {
                    dep.build(this->app_config_,
                              this->a_no_build_,
                              this->a_force_build_,
                              this->all_dependencies_,
                              this->cxx_standard_);
                }
                catch (...)
                {
                    // Rethrow する前に, install directory を削除しておく
                    std::filesystem::remove_all(this->app_config_.getInstallDirectory() / name / dep.getStateHash());
                    throw;
                }

                builts.emplace(name);
            }
            else
            {
                completed = false;
            }
        }

        // 少なくともひとつは進んでいるはず.
        if (builts_count == builts.size())
        {
            throw AppError{ "Dependency resolution failed: Circular dependency detected." };
        }
        builts_count = builts.size();
    }
}

// ============================================================================================= //
// [PRIVATE] _exportFullDepsToml()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_exportFullDepsToml() const
{
    // deps name を sort する.
    auto                  dep_name_range = this->all_dependencies_ | std::ranges::views::keys;
    std::set<std::string> sorted_names{ dep_name_range.begin(), dep_name_range.end() };

    toml::value root;
    for (const auto & name : sorted_names)
    {
        const auto & dep   = this->all_dependencies_.at(name);
        auto &       t_dep = root[dep.getAuthor()][dep.getRepo()];
        t_dep["version"]   = dep.getVersion();

        toml::value t_opt{ toml::table{} };
        t_opt.as_table_fmt().fmt = toml::table_format::dotted;

        for (const auto & [key, value] : dep.getOptions())
        {
            if (key.starts_with("_"))
            {
                // hidden option, skip export
                continue;
            }
            if (value.hasBool())
            {
                t_opt[key] = static_cast<bool>(value);
            }
            else if (value.hasInt())
            {
                t_opt[key] = static_cast<std::int64_t>(value);
            }
            else if (value.hasDouble())
            {
                t_opt[key] = static_cast<double>(value);
            }
            else if (value.hasString())
            {
                t_opt[key] = static_cast<std::string>(value);
            }
        }

        t_dep["options"] = std::move(t_opt);

        t_dep.comments().emplace_back(" hash: " + dep.getStateHash());
        t_dep.comments().emplace_back(
            " path: "
            + utils::toString((this->app_config_.getInstallDirectory() / name / dep.getStateHash()).u8string()));
    }
    std::stringstream sstr;
    sstr << toml::format(root) << std::endl;

    std::filesystem::path export_path{ this->deps_file_path_.stem().u8string() + u8".full.toml" };
    std::ofstream         ofst{ export_path };
    AppError::check(ofst, "Failed to open file for writing: {}", export_path.u8string());

    std::regex remove_author_object_line_re{ R"(\[[^.]+\])" };
    ofst << std::regex_replace(sstr.str(), remove_author_object_line_re, "") << std::endl;
}

// ============================================================================================= //
// [PRIVATE] _exportCMakePresets()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_exportCMakePresets() const
{
    if (!this->a_cmake_preset_names_)
    {
        // CMakePreset 出力しない.
        return;
    }
    std::unordered_set<std::string> cmake_preset_names{ this->a_cmake_preset_names_->begin(),
                                                        this->a_cmake_preset_names_->end() };

    // CLI11 の仕様上, デフォルト値を "$" にしているので, それを除外する.
    cmake_preset_names.erase("$");

    nlohmann::json j_root{};
    bool           new_file = true;

    if (std::ifstream ifst{ this->cmake_preset_file_path_ }; ifst)
    {
        try
        {
            j_root   = nlohmann::json::parse(ifst);
            new_file = false;
        }
        catch (const nlohmann::json::parse_error & e)
        {
            AppError::throw_("Failed to parse CMake preset file: {}\n{}",
                             this->cmake_preset_file_path_.u8string(),
                             e.what());
        }
    }

    if (new_file)
    {
        // ひな形を作る

        // --- versions ---
        // >= 4.4 : 12
        // >= 4.3 : 11
        // >= 3.31 : 10
        // >= 3.30 : 9
        // >= 3.28 : 8
        // >= 3.27 : 7
        // >= 3.25 : 6
        // >= 3.24 : 5
        // >= 3.23 : 4
        // >= 3.21 : 3
        // >= 3.20 : 2
        // >= 3.19 : 1
        // このアプリの最低バージョンは 3.19 で絞っている.
        int preset_version = 0;
        if (this->cmake_version_.major >= 4)
        {
            if (this->cmake_version_.minor >= 4)
            {
                preset_version = 12;
            }
            else if (this->cmake_version_.minor >= 3)
            {
                preset_version = 11;
            }
            else
            {
                preset_version = 10;
            }
        }
        else if (this->cmake_version_.major == 3)
        {
            if (this->cmake_version_.minor >= 31)
            {
                preset_version = 10;
            }
            else if (this->cmake_version_.minor >= 30)
            {
                preset_version = 9;
            }
            else if (this->cmake_version_.minor >= 28)
            {
                preset_version = 8;
            }
            else if (this->cmake_version_.minor >= 27)
            {
                preset_version = 7;
            }
            else if (this->cmake_version_.minor >= 25)
            {
                preset_version = 6;
            }
            else if (this->cmake_version_.minor >= 24)
            {
                preset_version = 5;
            }
            else if (this->cmake_version_.minor >= 23)
            {
                preset_version = 4;
            }
            else if (this->cmake_version_.minor >= 21)
            {
                preset_version = 3;
            }
            else if (this->cmake_version_.minor >= 20)
            {
                preset_version = 2;
            }
            else if (this->cmake_version_.minor >= 19)
            {
                preset_version = 1;
            }
        }
        AppError::check(preset_version > 0,
                        "Unsupported CMake version (Failed to determine cmake preset version.): {}.{}",
                        this->cmake_version_.major,
                        this->cmake_version_.minor);
        j_root["version"] = preset_version;
    }

    if (!j_root.contains("configurePresets"))
    {
        j_root["configurePresets"] = nlohmann::json::array();
    }
    auto & j_conf_presets = j_root["configurePresets"];
    AppError::check(j_conf_presets.is_array(), "CMake preset file is invalid: configurePresets is not an object.");

    const auto setup_fn = [this](auto & j_obj, std::string name) {
        j_obj["name"] = std::move(name);
        if (const auto path = this->app_config_.getCMakePresetsDefaultBuildDir(); !path.empty())
        {
            j_obj["binaryDir"] = utils::toString(path);
        }
        if (const auto path = this->app_config_.getCMakePresetsDefaultInstallDir(); !path.empty())
        {
            j_obj["installDir"] = utils::toString(path);
        }
    };

    std::vector<std::reference_wrapper<nlohmann::json>> target_presets;
    if (cmake_preset_names.empty())
    {
        if (!j_conf_presets.empty())
        {
            // preset names について, 空配列の場合は先頭のプリセットのみ対象にする
            target_presets.emplace_back(j_conf_presets.front());
        }
        else
        {
            // preset name の指定もなく, preset 配列も空の場合は, 新規に preset を作る.
            auto & j = target_presets  //
                           .emplace_back(*j_conf_presets.insert(j_conf_presets.begin(), nlohmann::json::object()))
                           .get();
            setup_fn(j, "default");
        }
    }
    else
    {
        if (!j_conf_presets.empty())
        {
            // すでにプリセットが用意されていて,
            // 名前の指定がある場合は, その名前の preset を探す.
            for (const auto & preset_name : cmake_preset_names)
            {
                bool found = false;
                for (auto & j : j_conf_presets)
                {
                    if (j.contains("name") && j["name"].is_string() && j["name"] == preset_name)
                    {
                        target_presets.emplace_back(j);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // 見つからなかった場合はエラーにする.
                    throw AppError{ "CMake preset file does not contain preset with name: {}", preset_name };
                }
            }
        }
        else
        {
            // 名前が指定されているが, preset 配列が空の場合は, 新規に preset を作る.
            for (const auto & preset_name : cmake_preset_names)
            {
                auto & j = target_presets
                               .emplace_back(*j_conf_presets.insert(j_conf_presets.begin(), nlohmann::json::object()))
                               .get();
                setup_fn(j, preset_name);
            }
        }
    }

    // 構築する
    for (auto & j_preset : target_presets)
    {
        // 環境変数として追加する
        auto & env = j_preset.get()["environment"];

        std::u8string paths;
        for (const auto & dep : this->all_dependencies_ | std::ranges::views::values)
        {
            if (!paths.empty())
            {
                paths += u8";";
            }
            paths += (this->app_config_.getInstallDirectory()    //
                      / (dep.getAuthor() + "." + dep.getRepo())  //
                      / dep.getStateHash())
                         .u8string();
        }
        env["CMAKE_PREFIX_PATH"] = utils::toString(paths);

        // CXX Standard の指定がある場合は, ARGS に追加する
        if (!this->cxx_standard_.empty())
        {
            auto & args                = j_preset.get()["cacheVariables"];
            args["CMAKE_CXX_STANDARD"] = this->cxx_standard_;
        }
    }

    // 書き出す
    std::ofstream ofst{ this->cmake_preset_file_path_ };
    AppError::check(ofst, "Failed to open file for writing: {}", this->cmake_preset_file_path_.u8string());
    ofst << j_root.dump(4) << std::endl;

    logger.log("CMake preset file updated: {}", this->cmake_preset_file_path_.u8string());
}

// ============================================================================================= //
// [PRIVATE] _gatherLicenseFiles()
// ============================================================================================= //
void
roah::distb::app::App::Impl_::_gatherLicenseFiles() const
{
    if (this->a_licenses_dir_.empty())
    {
        return;
    }

    const auto output_dir = utils::makeAbsolutePath(this->a_licenses_dir_);
    logger.log("Gathering license files to: {}", output_dir.u8string());

    // 消すことはしなくていいかな...
    // 手動でほかのものも集めている可能性もあるので
    if (!std::filesystem::exists(output_dir))
    {
        std::filesystem::create_directories(output_dir);
    }

    for (const auto & dep : this->all_dependencies_ | std::ranges::views::values)
    {
        dep.copyLicenseFile(this->app_config_, output_dir);
    }
}
