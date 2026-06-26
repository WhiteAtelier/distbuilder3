#ifndef ROAH_DISTB_UTILS_BASE32_HPP
#define ROAH_DISTB_UTILS_BASE32_HPP

#include <cstddef>
#include <string>

namespace roah::distb::utils {

std::string
encodeBase32(const std::byte * const data,
             const std::size_t       size,
             const bool              small_char   = false,
             const bool              with_padding = false);

}  // namespace roah::distb::utils

#endif
