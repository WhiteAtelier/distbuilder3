#include "dependency.hpp"

#include "app_config.hpp"
#include "working_context_impl.hpp"
//
#include "roah/distb/config/library.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/base32.hpp"
#include "roah/distb/utils/hash.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/string_expander.hpp"

#include <cstdint>
#include <fstream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

namespace {
std::string
_getAuthorFromName(const std::string_view name)
{
    const auto pos = name.find('.');
    if (pos == std::string::npos)
    {
        throw roah::distb::LibraryConfigError{ "Dependency {}: Invalid format. Expected 'author.repo'.", name };
    }
    return std::string{ name.substr(0, pos) };
}
std::string
_getRepoFromName(const std::string_view name)
{
    const auto pos = name.find('.');
    if (pos == std::string::npos)
    {
        throw roah::distb::LibraryConfigError{ "Dependency {}: Invalid format. Expected 'author.repo'.", name };
    }
    return std::string{ name.substr(pos + 1) };
}
}  // namespace

roah::distb::app::Dependency::Dependency(std::string name)
    : library_conf_{ _getAuthorFromName(name), _getRepoFromName(name) }
    , library_conf_loaded_{ false }
{}

roah::distb::app::Dependency::Dependency(std::string author, std::string repo)
    : library_conf_{ std::move(author), std::move(repo) }
    , library_conf_loaded_{ false }
{}

roah::distb::app::Dependency::Dependency(Dependency &&) noexcept = default;

roah::distb::app::Dependency::~Dependency() noexcept = default;

roah::distb::app::Dependency &
roah::distb::app::Dependency::operator=(Dependency &&) noexcept
    = default;

const std::string &
roah::distb::app::Dependency::getAuthor() const noexcept
{
    return this->library_conf_.getAuthor();
}

const std::string &
roah::distb::app::Dependency::getRepo() const noexcept
{
    return this->library_conf_.getRepo();
}

void
roah::distb::app::Dependency::setVersion(std::string version)
{
    this->version_ = std::move(version);
}

const std::string &
roah::distb::app::Dependency::getVersion() const noexcept
{
    return this->version_;
}

bool
roah::distb::app::Dependency::checkVersionRange(const std::vector<std::string> & version_range) const
{
    const auto all_versions = this->library_conf_.getAllVersions();

    std::unordered_set<std::string> selectable_versions;

    const auto add_versions_fn = [&](const auto & start, const auto & end) {
        for (auto iter = start; iter != end; ++iter)
        {
            selectable_versions.emplace(*iter);
        }
    };

    auto       begin = all_versions.begin();
    const auto end   = all_versions.end();

    enum struct Mode
    {
        Point,
        InRange,
    };

    Mode mode = Mode::Point;

    for (const auto & v : version_range)
    {
        if (v == "...")
        {
            mode = Mode::InRange;
        }
        else
        {
            auto iter = std::find(begin, end, v);
            if (iter != end)
            {
                switch (mode)
                {
                case Mode::Point:
                    add_versions_fn(iter, iter + 1);
                    begin = iter + 1;
                    break;
                case Mode::InRange:
                    // レンジの終端
                    add_versions_fn(begin, iter + 1);
                    begin = iter + 1;
                    mode  = Mode::Point;
                    break;
                }
            }
            else
            {
                // 指定したバージョンが見つからない場合はエラー
                throw DependencyResolveError{
                    "Dependency {}.{}: Version '{}' is not found (not defined) in the library or invalid order.",
                    this->getAuthor(),
                    this->getRepo(),
                    v
                };
            }
        }
    }
    if (mode == Mode::InRange)
    {
        // "..." で終わったとき
        add_versions_fn(begin, end);
    }

    return selectable_versions.contains(this->version_);
}

template <typename T>
roah::distb::utils::OptionValue &
roah::distb::app::Dependency::setOption(std::string key, T value)
{
    return this->options_.try_emplace(std::move(key), std::move(value)).first->second;
}

// clang-format off
template roah::distb::utils::OptionValue & roah::distb::app::Dependency::setOption(std::string, std::string);
template roah::distb::utils::OptionValue & roah::distb::app::Dependency::setOption(std::string, bool);
template roah::distb::utils::OptionValue & roah::distb::app::Dependency::setOption(std::string, double);
template roah::distb::utils::OptionValue & roah::distb::app::Dependency::setOption(std::string, std::int64_t);
// clang-format on

const std::map<std::string, roah::distb::utils::OptionValue> &
roah::distb::app::Dependency::getOptions() const noexcept
{
    return this->options_;
}

void
roah::distb::app::Dependency::loadLibraryConfig(const AppConfig &         app_config,
                                                const config::Variables & override_options)
{
    if (this->library_conf_loaded_)
    {
        return;
    }

    const auto & search_paths = app_config.getSearchPaths();
    const auto   file_name    = this->getAuthor() + "." + this->getRepo();
    for (const auto & path : search_paths)
    {
        if (this->_loadLibraryConfig(path / (file_name + ".jsonc"), app_config, override_options)
            || this->_loadLibraryConfig(path / (file_name + ".json"), app_config, override_options))
        {
            this->library_conf_loaded_ = true;
            return;
        }
    }
    AppError::throw_("Library config '{}' is not found.", file_name);
}

bool
roah::distb::app::Dependency::_loadLibraryConfig(const std::filesystem::path & path,
                                                 const AppConfig &             app_config,
                                                 const config::Variables &     override_options)
{
    std::ifstream ifst{ path };
    if (ifst)
    {
        // 先に, すでにある option に対して override を行う.
        // この時点では, deps.toml によって指定された option しか存在しない.
        // すでに存在する option は上書きしない.
        // 変数評価できていないため, ここではエラーにできない.
        // 後程 build 時に, 変数評価して, もし conflict があればエラーになる.
        for (const auto & [key, value] : override_options)
        {
            this->options_.try_emplace(key, value);
        }

        // レシピ読み込み
        this->library_conf_.loadFromJson(ifst);

        // バージョン指定がない場合は, 最新をセットする.
        if (this->version_.empty())
        {
            const auto & versions = this->library_conf_.getAllVersions();
            if (versions.empty())
            {
                throw LibraryConfigError{ "{}.{}: No available version.", this->getAuthor(), this->getRepo() };
            }
            this->version_ = versions.back();
        }

        // option をマージする
        const auto & le = this->getLibraryEntityConfigOfSelectedVersion();
        for (const auto & [name, base_option] : le.getOptions())
        {
            // すでに存在する場合は上書きしない.
            this->options_.try_emplace(name, base_option);
        }

        const auto variables = this->generateVariables(app_config);

        // そのバージョンでの dependencies をキューに追加する.
        const auto & child_deps = le.getDependencies();
        for (const auto & child_dep : child_deps | std::ranges::views::values)
        {
            if (child_dep.evalCondition(variables))
            {
                this->resolved_dependencies_.emplace(child_dep.getName());
            }
        }

        return true;
    }
    return false;
}

const roah::distb::config::Library &
roah::distb::app::Dependency::getLibraryConfig() const noexcept
{
    return this->library_conf_;
}

const roah::distb::config::LibraryEntry &
roah::distb::app::Dependency::getLibraryEntityConfigOfSelectedVersion() const
{
    const auto * const ret = this->library_conf_.findLibraryEntryByVersion(this->version_);
    AppError::check(ret != nullptr,
                    "Dependency {}.{}: Version '{}' is not found in the library.",
                    this->getAuthor(),
                    this->getRepo(),
                    this->version_);
    return *ret;
}

const std::string &
roah::distb::app::Dependency::getStateHash() const noexcept
{
    return this->state_hash_;
}

roah::distb::config::Variables
roah::distb::app::Dependency::generateVariables(const AppConfig & app_config) const
{
    roah::distb::config::Variables variables;
    variables["version"]      = this->version_;
    variables["author"]       = this->getAuthor();
    variables["repo"]         = this->getRepo();
    variables["repository"]   = this->getRepo();
    variables["generator"]    = utils::toString(app_config.getGenerator());
    variables["arch"]         = utils::toString(app_config.getArchitecture());
    variables["architecture"] = utils::toString(app_config.getArchitecture());
    variables["is_windows"]   = false;
    variables["is_linux"]     = false;
    variables["is_macos"]     = false;

#ifdef ROAH_ARCH_WIN32
    variables["platform"]   = "windows";
    variables["is_windows"] = true;
#elif ROAH_ARCH_LINUX
    variables["platform"] = "linux";
    variables["is_linux"] = true;
#elif ROAH_ARCH_MACOS
    variables["platform"] = "macos";
    variables["is_macos"] = true;
#else
#    error "Unknown platform"
#endif

    // -- option は "option." の名前空間に入れる
    for (const auto & [key, value] : this->options_)
    {
        variables["options." + key] = value;
    }
    return variables;
}

const std::unordered_set<std::string> &
roah::distb::app::Dependency::getResolvedDependencies() const noexcept
{
    return this->resolved_dependencies_;
}

void
roah::distb::app::Dependency::build(const AppConfig &                                   app_config,
                                    const bool                                          dryrun,
                                    const bool                                          force_build,
                                    const std::unordered_map<std::string, Dependency> & all_dependencies,
                                    const std::string &                                 cxx_standard)
{
    // ビルドできる状態であるとする.
    // まず state hash を計算する.
    this->_calculateStateHash(all_dependencies, cxx_standard);

    const auto & le = this->getLibraryEntityConfigOfSelectedVersion();

    // license が設定されていない場合はエラーにする.
    if (le.getLicenseFilePath().empty())
    {
        throw LibraryConfigError{ "Dependency {}.{}: License file path is not specified in the library config.",
                                  this->getAuthor(),
                                  this->getRepo() };
    }

    const auto install_dir = app_config.getInstallDirectory()             //
                           / (this->getAuthor() + "." + this->getRepo())  //
                           / this->state_hash_;
    const auto build_root = app_config.getBuildDirectory()               //
                          / (this->getAuthor() + "." + this->getRepo())  //
                          / this->state_hash_;

    logger.trace("-----------------------------------------------------------");
    logger.trace("[Build] Library '{}.{}'", this->getAuthor(), this->getRepo());
    logger.trace("-----------------------------------------------------------");
    logger.trace("-- Version     = {}", this->getVersion());
    logger.trace("-- State hash  = {}", this->state_hash_);
    logger.trace("-- Build dir   = {}", utils::toString(build_root.u8string()));
    logger.trace("-- Install dir = {}", utils::toString(install_dir.u8string()));

    // variables 作る
    auto variables                = this->generateVariables(app_config);
    variables["install_root_dir"] = utils::toString(app_config.getInstallDirectory().u8string());
    variables["build_root_dir"]   = utils::toString(app_config.getBuildDirectory().u8string());
    variables["cmake_executable"] = utils::toString(app_config.getCMakeExecutable());
    variables["working_dir"]      = utils::toString(build_root.u8string());
    variables["install_dir"]      = utils::toString(install_dir.u8string());
    variables["cxx_standard"]     = cxx_standard;

    const auto print_value_fn = [](const std::string_view prefix, const auto & key, const auto & value) {
        if (value.hasString())
        {
            logger.trace("{}-- {:<24} = <str> '{}'", prefix, key, static_cast<std::string>(value));
        }
        else if (value.hasBool())
        {
            logger.trace("{}-- {:<24} = <bool> {}", prefix, key, static_cast<std::string>(value));
        }
        else if (value.hasInt())
        {
            logger.trace("{}-- {:<24} = <int> {}", prefix, key, static_cast<std::string>(value));
        }
        else if (value.hasDouble())
        {
            logger.trace("{}-- {:<24} = <double> {}", prefix, key, static_cast<std::string>(value));
        }
        else
        {
            // Program error
            throw std::runtime_error{ "Unknown variable type for key: " + key };
        }
    };

    if (logger.isVerbose())
    {
        logger.trace("-- Options ------------------------------------------------");
        for (const auto & [key, value] : this->options_)
        {
            print_value_fn("   ", key, value);
        }

        logger.trace("-- Variables ----------------------------------------------");
        for (const auto & [key, value] : variables)
        {
            print_value_fn("   ", key, value);
        }
    }

    logger.trace("-- Dependencies -------------------------------------------");
    if (this->resolved_dependencies_.empty())
    {
        logger.trace("    (No Dependency)");
    }

    std::unordered_map<std::string, std::string> dependencies;
    for (const auto & name : this->resolved_dependencies_)
    {
        logger.trace("   -- {}", name);

        // 依存のバージョン一致を判定する.
        const auto & dep     = all_dependencies.at(name);
        const auto & req_dep = le.getDependencies().at(name);

        logger.trace("      -- Version {}", dep.getVersion());

        if (!dep.checkVersionRange(req_dep.getRequiredVersionRange()))
        {
            std::string version_range_str;
            for (const auto & v : req_dep.getRequiredVersionRange())
            {
                if (!version_range_str.empty())
                {
                    version_range_str += ", ";
                }
                version_range_str += v;
            }

            throw DependencyResolveError{
                "Library '{}.{}' (Version {}) is required Library {}.{} in version range [{}], "
                "but version '{}' is selected.",
                this->getAuthor(),
                this->getRepo(),
                this->getVersion(),
                dep.getAuthor(),
                dep.getRepo(),
                version_range_str,
                dep.getVersion()
            };
        }

        logger.trace("      -- Override Options ---------------------------------");

        // override option のチェックを行う
        const auto & dep_options = dep.getOptions();
        const auto & req_options = req_dep.getOptions();
        if (req_options.empty())
        {
            logger.trace("         (No Option)");
        }
        for (const auto & [key, _req_value] : req_options)
        {
            const auto iter = dep_options.find(key);
            if (iter != dep_options.end())
            {
                // req 側は自分の variable で展開する
                auto req_value = _req_value;
                if (req_value.hasString())
                {
                    std::string ret;
                    utils::expandTemplate(static_cast<std::string>(req_value), variables, ret);
                    req_value = ret;
                }
                print_value_fn("         ", "(request) " + key, req_value);

                // dep 側は dep 側の variable で展開する
                auto dep_value = iter->second;
                if (dep_value.hasString())
                {
                    std::string ret;
                    utils::expandTemplate(static_cast<std::string>(dep_value), dep.generateVariables(app_config), ret);
                    dep_value = ret;
                }
                print_value_fn("         ", " (actual) " + key, dep_value);

                if (dep_value != req_value)
                {
                    throw DependencyResolveError{
                        "Library '{}.{}' (Version {}) is required Library '{}.{}' with option '{}={}', "
                        "but actually '{}={}' is set.",
                        this->getAuthor(),
                        this->getRepo(),
                        this->getVersion(),
                        dep.getAuthor(),
                        dep.getRepo(),
                        key,
                        static_cast<std::string>(req_value),
                        key,
                        static_cast<std::string>(dep_value)
                    };
                }
            }
        }

        dependencies[name] = dep.getStateHash();
    }

    logger.trace("-------------------------------------------------");

    if (dryrun)
    {
        logger.log("Dependency {}.{}: Dry run. Skip build.", this->getAuthor(), this->getRepo());
        return;
    }

    // インストールディレクトリが存在した場合はスキップにする.
    if (!force_build && std::filesystem::exists(install_dir))
    {
        logger.log("Dependency {}.{}: Already built. Skip.", this->getAuthor(), this->getRepo());
        return;
    }

    // ビルド用のディレクトリを作る
    if (!std::filesystem::exists(build_root))
    {
        std::filesystem::create_directories(build_root);
    }

    // WorkingContext を作成する.
    WorkingContextImpl working_ctx{ app_config, build_root, variables, dependencies };

    // ビルド開始
    le.build(working_ctx);
}

void
roah::distb::app::Dependency::copyLicenseFile(const AppConfig &             app_config,
                                              const std::filesystem::path & output_dir) const
{
    config::Variables variables;
    variables["version"] = this->version_;
    for (const auto & [key, value] : this->options_)
    {
        variables["options." + key] = value;
    }

    const auto & le          = this->getLibraryEntityConfigOfSelectedVersion();
    const auto   install_dir = app_config.getInstallDirectory()           //
                           / (this->getAuthor() + "." + this->getRepo())  //
                           / this->state_hash_;

    if (le.getLicenseFilePath() == "!NoLicense")
    {
        // 明示的にライセンスファイルがないと指定されている場合はコピーしない.
        return;
    }
    if (le.getLicenseFilePath().empty())
    {
        // ライセンスファイルのパスが空の場合はエラー
        throw LibraryConfigError{ "Dependency {}.{}: License file path is not specified in the library config.",
                                  this->getAuthor(),
                                  this->getRepo() };
    }

    const auto src_file_path = install_dir / utils::toU8String(le.getLicenseFilePath());
    const auto dst_file_path
        = output_dir
        / (utils::toU8String(this->getAuthor() + "." + this->getRepo() + ".") + src_file_path.filename().u8string());

    if (!std::filesystem::exists(src_file_path))
    {
        throw LibraryConfigError{ "Dependency {}.{}: License file '{}' is not found in the install directory.",
                                  this->getAuthor(),
                                  this->getRepo(),
                                  src_file_path.u8string() };
    }

    logger.trace("Dependency {}.{}: Copy license file: {} -> {}",
                 this->getAuthor(),
                 this->getRepo(),
                 src_file_path.u8string(),
                 dst_file_path.u8string());
    std::filesystem::copy(src_file_path, dst_file_path, std::filesystem::copy_options::overwrite_existing);
}

void
roah::distb::app::Dependency::_calculateStateHash(const std::unordered_map<std::string, Dependency> & all_dependencies,
                                                  const std::string &                                 cxx_standard)
{
    // 現在の状態をハッシュ化する.
    std::stringstream state;
    state << "version=" << this->version_ << std::endl;
    state << "cxx_standard=" << cxx_standard << std::endl;
    state << "[options]" << std::endl;
    for (const auto & [key, value] : this->options_)
    {
        state << key << "=" << static_cast<std::string>(value) << std::endl;
    }

    state << "[dependencies]" << std::endl;
    for (const auto & name : this->resolved_dependencies_)
    {
        const auto & dep = all_dependencies.at(name);
        state << name << "=" << dep.getStateHash() << std::endl;
    }

    // ファイル名には MD5 を使う.
    utils::MD5Hash hash;
    auto           state_str = state.str();
    hash.addData(state_str.data(), state_str.size());
    const auto hash_bin = hash.getHashBinary();
    this->state_hash_   = utils::encodeBase32(hash_bin.data(), hash_bin.size(), /*small char*/ true);
}
