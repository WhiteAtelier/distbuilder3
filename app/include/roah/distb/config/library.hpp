#ifndef ROAH_DISTB_CONFIG_LIBRARY_HPP
#define ROAH_DISTB_CONFIG_LIBRARY_HPP

#include "library_entry.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>

namespace roah::distb::config {

class Library
{
public:
    Library(std::string author, std::string repo);
    Library(Library &&) noexcept;
    ~Library() noexcept;

    Library &
    operator=(Library &&) noexcept
        = default;

    Library(const Library &) = delete;
    Library &
    operator=(const Library &)
        = delete;

    const std::string &
    getAuthor() const noexcept;

    const std::string &
    getRepo() const noexcept;

    void
    loadFromJson(std::ifstream & ifst);

    const LibraryEntry *
    findLibraryEntryByVersion(const std::string & version) const;

    std::vector<std::string>
    getAllVersions() const;

private:
    void
    _addLibraryEntry(LibraryEntry && item);

    std::string                                       author_;
    std::string                                       repo_;
    std::unordered_map<std::string, LibraryEntry>     entries_version_map_;
    std::vector<std::reference_wrapper<LibraryEntry>> entries_;
};

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFJG_LIBRARY_HPP
