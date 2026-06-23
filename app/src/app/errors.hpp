#ifndef ROAH_DISTB_APP_ERRORS_HPP
#define ROAH_DISTB_APP_ERRORS_HPP

#include <concepts>
#include <format>
#include <stdexcept>
#include <string_view>

namespace roah::distb::app {

class AppError : public std::runtime_error
{
public:
    using runtime_error::runtime_error;

    template <typename T, typename... Args>
    static void
    check(const T & condition, const std::string_view fmtstr, const Args &... args);

    template <typename... Args>
    [[noreturn]]
    static void
    throw_(const std::string_view fmtstr, const Args &... args);
};

}  // namespace roah::distb::app

template <typename T, typename... Args>
inline void
roah::distb::app::AppError::check(const T & condition, const std::string_view fmtstr, const Args &... args)
{
    if (!static_cast<bool>(condition))
    {
        throw_<Args...>(fmtstr, args...);
    }
}

template <typename... Args>
inline void
roah::distb::app::AppError::throw_(const std::string_view fmtstr, const Args &... args)
{
    throw AppError{ std::vformat(fmtstr, std::make_format_args(args...)) };
}

#endif
