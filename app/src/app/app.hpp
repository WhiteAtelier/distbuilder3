#ifndef ROAH_DISTB_APP_APP_HPP
#define ROAH_DISTB_APP_APP_HPP

#include <cstdint>
#include <filesystem>
#include <memory>

namespace roah::distb::app {

class Dependency;

struct CMakeVersion
{
    CMakeVersion() noexcept;

    std::uint32_t major;
    std::uint32_t minor;
    std::uint32_t patch;
};

class App
{
public:
    static int
    run(int argc, const char * const argv[]);

    ~App() noexcept;

private:
    App(std::filesystem::path executable_dir);

    class Impl_;
    std::unique_ptr<Impl_> impl_;
};

}  // namespace roah::distb::app

#endif
