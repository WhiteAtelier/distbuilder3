#include "roah/distb/config/dependency_spec.hpp"

#include "roah/distb/config/condition.hpp"
#include "roah/distb/errors.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>

roah::distb::config::DependencySpec::DependencySpec(std::string name)
    : name_{ std::move(name) }
{}

roah::distb::config::DependencySpec::DependencySpec(const DependencySpec &) = default;

roah::distb::config::DependencySpec::DependencySpec(DependencySpec &&) noexcept = default;

roah::distb::config::DependencySpec &
roah::distb::config::DependencySpec::operator=(const DependencySpec &)
    = default;

roah::distb::config::DependencySpec &
roah::distb::config::DependencySpec::operator=(DependencySpec &&) noexcept
    = default;

roah::distb::config::DependencySpec::~DependencySpec() = default;

void
roah::distb::config::DependencySpec::updateFromJson(const nlohmann::json & json)
{
    if (!json.is_object())
    {
        throw LibraryConfigError{ "Invalid dependency spec: expected an object." };
    }

    // version フィールド: 上書き.
    if (const auto i_version = json.find("version"); i_version != json.end())
    {
        if (!i_version->is_array())
        {
            throw LibraryConfigError{ "Invalid 'version' field: expected an array." };
        }

        this->required_version_range_.clear();
        for (const auto & v : *i_version)
        {
            if (v.is_string())
            {
                this->required_version_range_.emplace_back(v.get<std::string>());
            }
            else
            {
                throw LibraryConfigError{ "Invalid 'version' array element: expected a string." };
            }
        }
    }

    // options フィールド: マージ. null 値はキーを削除する.
    if (const auto i_options = json.find("options"); i_options != json.cend())
    {
        if (!i_options->is_object())
        {
            throw LibraryConfigError{ "Invalid 'options' field: expected an object." };
        }

        for (const auto & [key, val] : i_options->items())
        {
            if (val.is_null())
            {
                this->options_.erase(key);
            }
            else if (val.is_string())
            {
                this->options_[key] = val.get<std::string>();
            }
            else if (val.is_boolean())
            {
                this->options_[key] = val.get<bool>();
            }
            else if (val.is_number_integer())
            {
                this->options_[key] = val.get<std::int64_t>();
            }
            else if (val.is_number_float())
            {
                this->options_[key] = val.get<double>();
            }
            else
            {
                throw LibraryConfigError{
                    "Invalid option value type for '{}': expected a string, number, boolean, or null.",
                    key
                };
            }
        }
    }

    // condition
    if (const auto i_condition = json.find("condition"); i_condition != json.cend())
    {
        if (i_condition->is_null())
        {
            this->condition_.reset();
        }
        else
        {
            auto new_condition = makeConditionFromJson(*i_condition);
            new_condition->loadFromJson(*i_condition);
            this->condition_ = std::move(new_condition);
        }
    }
}

const std::string &
roah::distb::config::DependencySpec::getName() const noexcept
{
    return this->name_;
}

const std::vector<std::string> &
roah::distb::config::DependencySpec::getRequiredVersionRange() const noexcept
{
    return this->required_version_range_;
}

const std::unordered_map<std::string, roah::distb::utils::OptionValue> &
roah::distb::config::DependencySpec::getOptions() const noexcept
{
    return this->options_;
}

bool
roah::distb::config::DependencySpec::evalCondition(const config::Variables & variables) const
{
    if (this->condition_)
    {
        return this->condition_->eval(variables);
    }
    return true;
}
