// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "roah/distb/config/dependency_spec.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>

// ---------- コンストラクタ / デストラクタ ----------

roah::distb::config::DependencySpec::DependencySpec(std::string name)
    : name_{ std::move(name) }
{}

roah::distb::config::DependencySpec::DependencySpec(const DependencySpec &) = default;

roah::distb::config::DependencySpec::DependencySpec(DependencySpec &&) noexcept = default;

roah::distb::config::DependencySpec &
roah::distb::config::DependencySpec::operator=(const DependencySpec &) = default;

roah::distb::config::DependencySpec &
roah::distb::config::DependencySpec::operator=(DependencySpec &&) noexcept = default;

roah::distb::config::DependencySpec::~DependencySpec() = default;

// ---------- 公開メソッド ----------

bool
roah::distb::config::DependencySpec::updateFromJson(const nlohmann::json & json)
{
    if (!json.is_object())
    {
        return false;
    }

    // version フィールド: 上書き.
    if (json.contains("version") && json["version"].is_array())
    {
        this->required_version_range_.clear();
        for (const auto & v : json["version"])
        {
            if (v.is_string())
            {
                this->required_version_range_.push_back(v.get<std::string>());
            }
        }
    }

    // options フィールド: マージ. null 値はキーを削除する.
    if (json.contains("options") && json["options"].is_object())
    {
        for (const auto & [key, val] : json["options"].items())
        {
            if (val.is_null())
            {
                this->options_.erase(key);
            }
            else if (val.is_string())
            {
                this->options_.insert_or_assign(key, OptionValue{ val.get<std::string>() });
            }
            else if (val.is_boolean())
            {
                this->options_.insert_or_assign(key, OptionValue{ val.get<bool>() });
            }
            else if (val.is_number_integer())
            {
                this->options_.insert_or_assign(key, OptionValue{ val.get<std::int64_t>() });
            }
            else if (val.is_number_float())
            {
                this->options_.insert_or_assign(key, OptionValue{ val.get<double>() });
            }
        }
    }

    return true;
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

const std::unordered_map<std::string, roah::distb::config::OptionValue> &
roah::distb::config::DependencySpec::getOptions() const noexcept
{
    return this->options_;
}
