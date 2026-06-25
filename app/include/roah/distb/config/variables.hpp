#ifndef ROAH_DISTB_CONFIG_VARIABLES_HPP
#define ROAH_DISTB_CONFIG_VARIABLES_HPP

#include "roah/distb/utils/option_value.hpp"

#include <string>
#include <unordered_map>

namespace roah::distb::config {
using Variables = std::unordered_map<std::string, utils::OptionValue>;
}  // namespace roah::distb::config

#endif
