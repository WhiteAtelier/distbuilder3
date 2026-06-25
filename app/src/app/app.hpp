#ifndef ROAH_DISTB_APP_APP_HPP
#define ROAH_DISTB_APP_APP_HPP

#include <filesystem>
#include <memory>

namespace roah::distb::app {

class Dependency;

class App
{
public:
    static int
    run(int argc, const char * const argv[]);

    ~App() noexcept;

    bool
    hasDependency(const std::string_view name) const;

    Dependency &
    addDependency(std::string author, std::string repo);

private:
    App(std::filesystem::path executable_dir);

    class Impl_;
    std::unique_ptr<Impl_> impl_;
};

}  // namespace roah::distb::app

#endif
