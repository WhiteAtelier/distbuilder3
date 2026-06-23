#ifndef ROAH_DISTB_UTILS_STRING_HPP
#define ROAH_DISTB_UTILS_STRING_HPP

#include <string>

namespace roah::distb::utils {

std::u8string
toU8String(const std::string_view str);

}  // namespace roah::distb::utils

#endif
