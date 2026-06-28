#include "roah/distb/utils/string.hpp"

#include <array>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

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

namespace {
inline std::byte
_toByte(const char c)
{
    if (c >= '0' && c <= '9')
    {
        return static_cast<std::byte>(c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
        return static_cast<std::byte>(c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
        return static_cast<std::byte>(c - 'A' + 10);
    }
    else
    {
        throw std::invalid_argument{ "Invalid hex character: " + std::string{ c } };
    }
}
}  // namespace

std::vector<std::byte>
roah::distb::utils::toBinaryFromHexString(const std::string_view str)
{
    if (str.size() % 2 != 0)
    {
        throw std::invalid_argument{ "Hex string must have even length. (" + std::string{ str } + ")" };
    }

    std::vector<std::byte> ret;
    ret.reserve(str.size() / 2);

    for (std::size_t i = 0; i < str.size(); i += 2)
    {
        ret.emplace_back(_toByte(str[i]) << 4 | _toByte(str[i + 1]));
    }
    return ret;
}
