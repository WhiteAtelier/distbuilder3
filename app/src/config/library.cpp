#include "roah/distb/config/library.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/option_value.hpp"
#include "roah/distb/utils/string_expander.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::Library::Library(std::string author, std::string repo)
    : author_{ std::move(author) }
    , repo_{ std::move(repo) }
{}

roah::distb::config::Library::Library(Library &&) noexcept = default;

roah::distb::config::Library::~Library() noexcept = default;

const std::string &
roah::distb::config::Library::getAuthor() const noexcept
{
    return this->author_;
}

const std::string &
roah::distb::config::Library::getRepo() const noexcept
{
    return this->repo_;
}

void
roah::distb::config::Library::loadFromJson(std::ifstream & ifst)
{
    std::unordered_map<std::string, utils::OptionValue> vars;

    try
    {
        const auto j_root = nlohmann::json::parse(ifst, nullptr, true, true);
        if (const auto i_entries = j_root.find("entries"); i_entries != j_root.end())
        {
            if (!i_entries->is_array())
            {
                throw LibraryConfigError{ "{}.{}: 'entries' field is not an array.", this->author_, this->repo_ };
            }

            for (const auto & j_entry : i_entries.value())
            {
                LibraryEntry entry{ j_entry.at("version").get<std::string>() };
                if (const auto i_base = j_entry.find("base"); i_base != j_entry.end())
                {
                    if (!i_base->is_string())
                    {
                        throw LibraryConfigError{ "{}.{}: 'base' field is not a string.", this->author_, this->repo_ };
                    }

                    std::string base_version;
                    utils::expandTemplate(i_base.value().get<std::string>(), vars, base_version);
                    const auto * const base_entry = this->findLibraryEntryByVersion(base_version);
                    if (base_entry == nullptr)
                    {
                        throw LibraryConfigError{ "{}.{}: Version '{}', Base version '{}' is not found.",
                                                  this->author_,
                                                  this->repo_,
                                                  entry.getVersion(),
                                                  base_version };
                    }
                    entry.setBase(*base_entry);
                }

                try
                {
                    entry.updateFromJson(j_entry);
                }
                catch (const LibraryConfigError & e)
                {
                    // エラーメッセージを包み直して再度 throw
                    throw LibraryConfigError{ "{}.{}: Version '{}', {}",
                                              this->author_,
                                              this->repo_,
                                              entry.getVersion(),
                                              e.what() };
                }

                vars["previous"] = entry.getVersion();
                logger.trace("-- Added version: {}.{}, {}", this->author_, this->repo_, entry.getVersion());
                this->_addLibraryEntry(std::move(entry));
            }
        }
    }
    catch (const nlohmann::json::parse_error & e)
    {
        throw LibraryConfigError{ "{}.{}: Failed to parse JSON (parse error): {}",
                                  this->author_,
                                  this->repo_,
                                  e.what() };
    }
    catch (const nlohmann::json::type_error & e)
    {
        throw LibraryConfigError{ "{}.{}: Failed to parse JSON (type error): {}",
                                  this->author_,
                                  this->repo_,
                                  e.what() };
    }
    catch (const nlohmann::json::other_error & e)
    {
        throw LibraryConfigError{ "{}.{}: Failed to parse JSON (other error): {}",
                                  this->author_,
                                  this->repo_,
                                  e.what() };
    }
    catch (const nlohmann::json::exception & e)
    {
        throw LibraryConfigError{ "{}.{}: Failed to parse JSON: {}", this->author_, this->repo_, e.what() };
    }
}

const roah::distb::config::LibraryEntry *
roah::distb::config::Library::findLibraryEntryByVersion(const std::string & version) const
{
    if (const auto iter = this->entries_version_map_.find(version);  //
        iter != this->entries_version_map_.end())
    {
        return &iter->second;
    }
    return nullptr;
}

std::vector<std::string>
roah::distb::config::Library::getAllVersions() const
{
    std::vector<std::string> versions;
    versions.reserve(this->entries_.size());
    for (const auto & entry : this->entries_)
    {
        versions.emplace_back(entry.get().getVersion());
    }
    return versions;
}

void
roah::distb::config::Library::_addLibraryEntry(LibraryEntry && item)
{
    auto version             = item.getVersion();
    const auto [iter, added] = this->entries_version_map_.try_emplace(std::move(version), std::move(item));
    if (!added)
    {
        throw LibraryConfigError{ "{}.{}: Version '{}' is already existing.", this->author_, this->repo_, iter->first };
    }
    this->entries_.emplace_back(iter->second);
}
