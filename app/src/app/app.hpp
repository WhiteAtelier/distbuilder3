#ifndef ROAH_DISTB_APP_APP_HPP
#define ROAH_DISTB_APP_APP_HPP

#include <memory>

namespace roah::distb::app {

class App
{
public:
    static int
    run(int argc, const char * const argv[]);

    ~App() noexcept;

private:
    App();

    class Impl_;
    std::unique_ptr<Impl_> impl_;
};

}  // namespace roah::distb::app

#endif
