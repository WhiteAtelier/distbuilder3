#ifndef ROAH_DISTB_UTILS_STRING_HPP
#define ROAH_DISTB_UTILS_STRING_HPP

#include <string>

namespace roah::distb::utils {

std::u8string
toU8String(const std::string_view str);

std::string
toString(const std::u8string_view str);

bool
contains(const std::string_view str, const char character);

bool
contains(const std::u8string_view str, const char8_t character);

}  // namespace roah::distb::utils

#endif
