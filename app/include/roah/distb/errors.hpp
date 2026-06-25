#ifndef ROAH_DISTB_ERRORS_HPP
#define ROAH_DISTB_ERRORS_HPP

#include <concepts>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>

namespace roah::distb {

class DistbuilderException : public std::runtime_error
{
public:
    enum struct ReturnCode : int
    {
        UnknownError       = 255,
        AppError           = 1,
        LibraryConfigError = 2,
    };

    template <typename... Args>
    DistbuilderException(const std::string_view fmtstr, const Args &... args);

    ReturnCode
    getReturnCode() const noexcept;

protected:
    DistbuilderException(const ReturnCode return_code, const std::string & msg);

private:
    ReturnCode code_;
};

class AppError : public DistbuilderException
{
public:
    template <typename... Args>
    AppError(const std::string_view fmtstr, const Args &... args);

    template <typename T, typename... Args>
    static void
    check(const T & condition, const std::string_view fmtstr, const Args &... args);

    template <typename... Args>
    [[noreturn]]
    static void
    throw_(const std::string_view fmtstr, const Args &... args);
};

class LibraryConfigError : public DistbuilderException
{
public:
    template <typename... Args>
    LibraryConfigError(const std::string_view fmtstr, const Args &... args);
};

class DependencyResolveError : public DistbuilderException
{
public:
    template <typename... Args>
    DependencyResolveError(const std::string_view fmtstr, const Args &... args);
};

}  // namespace roah::distb

template <typename... Args>
inline roah::distb::DistbuilderException::DistbuilderException(const std::string_view fmtstr, const Args &... args)
    : runtime_error{ std::vformat(fmtstr, std::make_format_args(args...)) }
    , code_{ ReturnCode::UnknownError }
{}

template <typename... Args>
inline roah::distb::AppError::AppError(const std::string_view fmtstr, const Args &... args)
    : DistbuilderException{ ReturnCode::AppError,  //
                            "[AppError] " + std::vformat(fmtstr, std::make_format_args(args...)) }
{}

template <typename T, typename... Args>
inline void
roah::distb::AppError::check(const T & condition, const std::string_view fmtstr, const Args &... args)
{
    if (!static_cast<bool>(condition))
    {
        throw_<Args...>(fmtstr, args...);
    }
}

template <typename... Args>
inline void
roah::distb::AppError::throw_(const std::string_view fmtstr, const Args &... args)
{
    throw AppError{ fmtstr, args... };
}

template <typename... Args>
inline roah::distb::LibraryConfigError::LibraryConfigError(const std::string_view fmtstr, const Args &... args)
    : DistbuilderException{ ReturnCode::LibraryConfigError,
                            "[LibraryConfigError] " + std::vformat(fmtstr, std::make_format_args(args...)) }
{}

template <typename... Args>
inline roah::distb::DependencyResolveError::DependencyResolveError(const std::string_view fmtstr, const Args &... args)
    : DistbuilderException{ ReturnCode::LibraryConfigError,
                            "[DependencyResolveError] " + std::vformat(fmtstr, std::make_format_args(args...)) }
{}

#endif
