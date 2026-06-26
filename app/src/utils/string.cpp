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

std::string
roah::distb::utils::toString(const std::u8string_view str)
{
    return std::string{ reinterpret_cast<const char *>(str.data()), str.size() };
}

bool
roah::distb::utils::contains(const std::string_view str, const char character)
{
    return str.find(character) != std::string_view::npos;
}

bool
roah::distb::utils::contains(const std::u8string_view str, const char8_t character)
{
    return str.find(character) != std::string_view::npos;
}
