#ifndef ROAH_DISTB_APP_LIBRARY_STORE_HPP
#define ROAH_DISTB_APP_LIBRARY_STORE_HPP

#include <chrono>
#include <string>

namespace roah::distb::app {

class AppConfig;

class LibraryStore
{
public:
    LibraryStore(const AppConfig & app_config);
    ~LibraryStore() noexcept;

    LibraryStore(const LibraryStore &) = delete;
    LibraryStore &
    operator=(const LibraryStore &)
        = delete;
    LibraryStore(LibraryStore &&) = delete;
    LibraryStore &
    operator=(LibraryStore &&)
        = delete;

    void
    fetch();

private:
    const AppConfig &                     app_config_;
    std::string                           commit_hash_;
    std::chrono::system_clock::time_point last_fetched_at_;
};
}  // namespace roah::distb::app

#endif
