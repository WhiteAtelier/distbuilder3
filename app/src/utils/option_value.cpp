// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/option_value.hpp>

#include <algorithm>
#include <bit>
#include <bitset>
#include <stdexcept>

roah::distb::utils::OptionValue::OptionValue()
    : value_{ /* string */ }
{}

roah::distb::utils::OptionValue::OptionValue(std::string value)
    : value_{ std::move(value) }
{}

roah::distb::utils::OptionValue::OptionValue(const bool value)
    : value_{ value }
{}

roah::distb::utils::OptionValue::OptionValue(const std::int64_t value)
    : value_{ value }
{}

roah::distb::utils::OptionValue::OptionValue(const double value)
    : value_{ value }
{}

roah::distb::utils::OptionValue::OptionValue(const OptionValue &)     = default;
roah::distb::utils::OptionValue::OptionValue(OptionValue &&) noexcept = default;

roah::distb::utils::OptionValue &
roah::distb::utils::OptionValue::operator=(const OptionValue &)
    = default;

roah::distb::utils::OptionValue &
roah::distb::utils::OptionValue::operator=(OptionValue &&) noexcept
    = default;

roah::distb::utils::OptionValue::~OptionValue() = default;

namespace {
struct EqualComp
{
    using ReturnType = bool;

    template <typename T>
    ReturnType
    operator()(const T & lhs, const T & rhs) const
    {
        return lhs == rhs;
    }
};

struct SpaceshipComp
{
    using ReturnType = std::partial_ordering;

    template <typename T>
    ReturnType
    operator()(const T & lhs, const T & rhs) const
    {
        return lhs <=> rhs;
    }
};
}  // namespace

template <typename Comp>
typename Comp::ReturnType
roah::distb::utils::OptionValue::_compare(const OptionValue & lhs, const OptionValue & rhs)
{
    Comp comp;
    if (lhs.value_.index() == rhs.value_.index())
    {
        // 同じ型同士は, それぞれの型の == 演算子で比較する.
        return comp(lhs.value_, rhs.value_);
    }

    const auto lhs_bool = std::holds_alternative<bool>(lhs.value_);
    const auto lhs_str  = std::holds_alternative<std::string>(lhs.value_);
    const auto lhs_int  = std::holds_alternative<std::int64_t>(lhs.value_);
    const auto lhs_dbl  = std::holds_alternative<double>(lhs.value_);
    const auto rhs_bool = std::holds_alternative<bool>(rhs.value_);
    const auto rhs_str  = std::holds_alternative<std::string>(rhs.value_);
    const auto rhs_int  = std::holds_alternative<std::int64_t>(rhs.value_);
    const auto rhs_dbl  = std::holds_alternative<double>(rhs.value_);

    // bool vs int
    if ((lhs_bool && rhs_int) || (lhs_int && rhs_bool))
    {
        return comp(static_cast<std::int64_t>(lhs), static_cast<std::int64_t>(rhs));
    }

    // bool vs double, int vs double
    if ((lhs_dbl && (rhs_bool || rhs_int)) || ((lhs_bool || lhs_int) && rhs_dbl))
    {
        return comp(static_cast<double>(lhs), static_cast<double>(rhs));
    }

    // bool vs string
    if ((lhs_bool && rhs_str) || (lhs_str && rhs_bool))
    {
        return comp(static_cast<bool>(lhs), static_cast<bool>(rhs));
    }

    // int vs string, double vs string
    if (lhs_str && (rhs_int || rhs_dbl))
    {
        if (rhs_dbl || std::get<std::string>(lhs.value_).find('.') != std::string::npos)
        {
            // double/int string vs int/double
            return comp(static_cast<double>(lhs), static_cast<double>(rhs));
        }
        else
        {
            // int string vs int
            return comp(static_cast<std::int64_t>(lhs), static_cast<std::int64_t>(rhs));
        }
    }

    if ((lhs_int || lhs_dbl) && rhs_str)
    {
        if (lhs_dbl || std::get<std::string>(rhs.value_).find('.') != std::string::npos)
        {
            // double/int string vs int/double
            return comp(static_cast<double>(lhs), static_cast<double>(rhs));
        }
        else
        {
            // int string vs int
            return comp(static_cast<std::int64_t>(lhs), static_cast<std::int64_t>(rhs));
        }
    }

    throw std::runtime_error{ "Unsupported comparison" };
}

bool
roah::distb::utils::OptionValue::operator==(const OptionValue & rhs) const
{
    return _compare<EqualComp>(*this, rhs);
}

std::partial_ordering
roah::distb::utils::OptionValue::operator<=>(const OptionValue & rhs) const
{
    return _compare<SpaceshipComp>(*this, rhs);
}

roah::distb::utils::OptionValue::operator std::string() const
{
    if (std::holds_alternative<std::string>(this->value_))
    {
        return std::get<std::string>(this->value_);
    }
    if (std::holds_alternative<bool>(this->value_))
    {
        return std::get<bool>(this->value_) ? "ON" : "OFF";
    }
    if (std::holds_alternative<std::int64_t>(this->value_))
    {
        return std::to_string(std::get<std::int64_t>(this->value_));
    }
    return std::to_string(std::get<double>(this->value_));
}

roah::distb::utils::OptionValue::operator bool() const
{
    if (std::holds_alternative<bool>(this->value_))
    {
        return std::get<bool>(this->value_);
    }
    if (std::holds_alternative<std::int64_t>(this->value_))
    {
        return std::get<std::int64_t>(this->value_) != 0;
    }
    if (std::holds_alternative<double>(this->value_))
    {
        static_assert(sizeof(double) == sizeof(std::uint64_t), "Unexpected double size");
        const auto bits = std::bit_cast<std::uint64_t>(std::get<double>(this->value_));
        return bits != 0;
    }

    // string: 小文字化して判定する.
    auto s = std::get<std::string>(this->value_);
    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    if (s.empty() || s == "0" || s == "false" || s == "off" || s == "no")
    {
        return false;
    }
    return true;
}

roah::distb::utils::OptionValue::operator std::int64_t() const
{
    if (std::holds_alternative<std::string>(this->value_))
    {
        return std::stoll(std::get<std::string>(this->value_));
    }
    if (std::holds_alternative<bool>(this->value_))
    {
        return std::get<bool>(this->value_) ? 1LL : 0LL;
    }
    if (std::holds_alternative<std::int64_t>(this->value_))
    {
        return std::get<std::int64_t>(this->value_);
    }
    return static_cast<std::int64_t>(std::get<double>(this->value_));
}

roah::distb::utils::OptionValue::operator double() const
{
    if (std::holds_alternative<std::string>(this->value_))
    {
        return std::stod(std::get<std::string>(this->value_));
    }
    if (std::holds_alternative<bool>(this->value_))
    {
        return std::get<bool>(this->value_) ? 1.0 : 0.0;
    }
    if (std::holds_alternative<std::int64_t>(this->value_))
    {
        return static_cast<double>(std::get<std::int64_t>(this->value_));
    }
    return std::get<double>(this->value_);
}
