#ifndef ROAH_DISTB_LOGGER_HPP
#define ROAH_DISTB_LOGGER_HPP

#include <format>
#include <string>
#include <string_view>

namespace roah::distb::app {

/// @brief シンプルなロギングクラス.
class Logger
{
public:
    Logger();

    void
    setVerbose(const bool verbose = true) noexcept;

    template <typename... Args>
    void
    log(const std::string_view msg, const Args &... args);

    template <typename... Args>
    void
    trace(const std::string_view msg, const Args &... args);

private:
    void
    _log(const std::string_view msg);

    bool verbose_;
};

extern Logger logger;

template <typename... Args>
inline void
Logger::log(const std::string_view msg, const Args &... args)
{
    this->_log(std::vformat(msg, std::make_format_args(args...)));
}

template <typename... Args>
inline void
Logger::trace(const std::string_view msg, const Args &... args)
{
    if (this->verbose_)
    {
        this->_log(std::vformat(msg, std::make_format_args(args...)));
    }
}

}  // namespace roah::distb::app

namespace std {
template <>
struct formatter<std::u8string> : std::formatter<const char *>
{
    auto
    format(const std::u8string & c, std::format_context & ctx) const
    {
        return std::formatter<const char *>::format(reinterpret_cast<const char *>(c.c_str()), ctx);
    }
};
}  // namespace std

#endif
