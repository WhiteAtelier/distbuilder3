#include "roah/distb/utils/string.hpp"

#include <cstring>
#include <string>

std::u8string
roah::distb::utils::toU8String(const std::string_view str)
{
    std::u8string ret(str.size(), 0);
    std::memcpy(ret.data(), str.data(), str.size());
    return ret;
}
