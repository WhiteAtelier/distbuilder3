#include "dependency.hpp"

#include "app_config.hpp"
#include "roah/distb/config/library.hpp"
#include "roah/distb/errors.hpp"

#include <cstdint>
#include <fstream>
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
roah::distb::app::Dependency::checkVersionRange(const std::vector<std::string> & version_range)
{
    const auto all_versions = this->library_conf_.getAllVersions();

    std::unordered_set<std::string> selectable_versions;

    const auto add_versions_fn = [&](const auto & start, const auto & last) {
        const auto end = last + 1;
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
                    add_versions_fn(iter, iter);
                    begin = iter + 1;
                    break;
                case Mode::InRange:
                    // レンジの終端
                    add_versions_fn(begin, iter);
                    begin = iter + 1;
                    mode  = Mode::Point;
                    break;
                }
            }
            else
            {
                // 指定したバージョンが見つからない場合はエラー
                throw DependencyResolveError{
                    "Dependency {}.{}: Version '{}' is not found in the library or invalid order.",
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

void
roah::distb::app::Dependency::loadLibraryConfig(const AppConfig & app_config)
{
    if (this->library_conf_loaded_)
    {
        return;
    }

    const auto & search_paths = app_config.getSearchPaths();
    const auto   file_name    = this->getAuthor() + "." + this->getRepo();
    for (const auto & path : search_paths)
    {
        if (this->_loadLibraryConfig(path / (file_name + ".jsonc"))
            || this->_loadLibraryConfig(path / (file_name + ".json")))
        {
            this->library_conf_loaded_ = true;
            return;
        }
    }
    AppError::throw_("Library config '{}' is not found.", file_name);
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

bool
roah::distb::app::Dependency::_loadLibraryConfig(const std::filesystem::path & path)
{
    std::ifstream ifst{ path };
    if (ifst)
    {
        this->library_conf_.loadFromJson(ifst);

        if (this->version_.empty())
        {
            // 最新をセットする.
            const auto & versions = this->library_conf_.getAllVersions();
            if (versions.empty())
            {
                throw LibraryConfigError{ "{}.{}: No available version.", this->getAuthor(), this->getRepo() };
            }
            this->version_ = versions.back();
        }
        return true;
    }
    return false;
}
