// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/option_value.hpp>

#include <algorithm>
#include <stdexcept>

roah::distb::utils::OptionValue::OptionValue()
    : value_{ std::string{} }
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

bool
roah::distb::utils::OptionValue::operator==(const OptionValue & other) const
{
    return this->value_ == other.value_;
}

bool
roah::distb::utils::OptionValue::operator!=(const OptionValue & other) const
{
    return !(*this == other);
}

roah::distb::utils::OptionValue::operator std::string() const
{
    if (std::holds_alternative<std::string>(this->value_))
    {
        return std::get<std::string>(this->value_);
    }
    if (std::holds_alternative<bool>(this->value_))
    {
        return std::get<bool>(this->value_) ? "1" : "0";
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
        // double は bool 変換の仕様に含まれないので例外を投げる.
        throw std::runtime_error{ "OptionValue: cannot convert double to bool" };
    }
    // string: 小文字化して判定する.
    auto s = std::get<std::string>(this->value_);
    std::transform(s.begin(), s.end(), s.begin(), [](const unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    if (s == "1" || s == "true" || s == "on" || s == "yes")
    {
        return true;
    }
    if (s == "0" || s == "false" || s == "off" || s == "no")
    {
        return false;
    }
    throw std::runtime_error{ "OptionValue: cannot convert string \"" + s + "\" to bool" };
}

roah::distb::utils::OptionValue::operator std::int64_t() const
{
    if (!std::holds_alternative<std::int64_t>(this->value_))
    {
        throw std::runtime_error{ "OptionValue: value is not int64_t" };
    }
    return std::get<std::int64_t>(this->value_);
}

roah::distb::utils::OptionValue::operator double() const
{
    if (!std::holds_alternative<double>(this->value_))
    {
        throw std::runtime_error{ "OptionValue: value is not double" };
    }
    return std::get<double>(this->value_);
}
