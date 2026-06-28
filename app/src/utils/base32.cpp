#include "roah/distb/utils/base32.hpp"

#include <cstddef>
#include <string>

std::string
roah::distb::utils::encodeBase32(const void * const _data,
                                 const std::size_t  size,
                                 const bool         small_char,
                                 const bool         with_padding)
{
    std::string ret;
    ret.reserve((size + 4ull) / 5ull * 8ull);  // 5byte -> 8char

    const auto * const data   = reinterpret_cast<const std::byte *>(_data);
    const auto         get_fn = [&](const std::size_t i) { return (i < size) ? data[i] : std::byte{ 0 }; };

    constexpr std::string_view value_char_map_sm{ "abcdefghijklmnopqrstuvwxyz234567" };
    constexpr std::string_view value_char_map_lg{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567" };
    const std::string_view &   value_char_map = small_char ? value_char_map_sm : value_char_map_lg;

    constexpr std::byte mask{ 0x1Fu };
    for (std::size_t i = 0; i < size; ++i)
    {
        const auto a = (get_fn(i) >> 3) & mask;                               // 0>AAAA.ABBB
        const auto b = ((get_fn(i) << 2) | (get_fn(i + 1) >> 6)) & mask;      // 0>AAAA.ABBB | BBCC.CCCE
        const auto c = (get_fn(i + 1) >> 1) & mask;                           // 1>BBCC.CCCD
        const auto d = ((get_fn(i + 1) << 4) | (get_fn(i + 2) >> 4)) & mask;  // 1>BBCC.CCCD | DDDD.EEEE
        const auto e = ((get_fn(i + 2) << 1) | (get_fn(i + 3) >> 7)) & mask;  // 2>DDDD.EEEE | EFFF.FFGG
        const auto f = (get_fn(i + 3) >> 2) & mask;                           // 3>EFFF.FFGG
        const auto g = ((get_fn(i + 3) << 3) | (get_fn(i + 4) >> 5)) & mask;  // 3>EFFF.FFGG | GGGH.HHHH
        const auto h = get_fn(i + 4) & mask;                                  // 4>GGGH.HHHH

        ret.push_back(value_char_map[static_cast<std::size_t>(a)]);
        ret.push_back(value_char_map[static_cast<std::size_t>(b)]);

        if (++i < size)
        {
            ret.push_back(value_char_map[static_cast<std::size_t>(c)]);
            ret.push_back(value_char_map[static_cast<std::size_t>(d)]);
            if (++i < size)
            {
                ret.push_back(value_char_map[static_cast<std::size_t>(e)]);
                if (++i < size)
                {
                    ret.push_back(value_char_map[static_cast<std::size_t>(f)]);
                    ret.push_back(value_char_map[static_cast<std::size_t>(g)]);
                    if (++i < size)
                    {
                        ret.push_back(value_char_map[static_cast<std::size_t>(h)]);
                    }
                }
            }
        }
    }

    // padding?
    if (with_padding)
    {
        if (const auto pad = ret.size() % 8ull; pad != 0)
        {
            ret += std::string(static_cast<std::size_t>(8ull - pad), '=');
        }
    }
    return ret;
}
