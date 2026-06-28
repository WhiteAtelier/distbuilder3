#include "roah/distb/config/library_entry.hpp"

#include "roah/distb/config/step_def.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <unordered_map>

roah::distb::config::LibraryEntry::LibraryEntry(std::string version)
    : version_{ std::move(version) }
{}

roah::distb::config::LibraryEntry::LibraryEntry(LibraryEntry && other) noexcept = default;

roah::distb::config::LibraryEntry &
roah::distb::config::LibraryEntry::operator=(LibraryEntry &&) noexcept
    = default;

roah::distb::config::LibraryEntry::~LibraryEntry() noexcept = default;

void
roah::distb::config::LibraryEntry::setBase(const LibraryEntry & base_entry)
{
    // copy
    this->options_           = base_entry.options_;
    this->dependencies_      = base_entry.dependencies_;
    this->order_             = base_entry.order_;
    this->license_file_path_ = base_entry.license_file_path_;
    this->steps_             = base_entry.steps_;
}

void
roah::distb::config::LibraryEntry::updateFromJson(const nlohmann::json & json)
{
    if (auto i_options = json.find("options"); i_options != json.end())
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
                    "Invalid option value type for key '{}': expected string, boolean, integer, or float.",
                    key
                };
            }
        }
    }

    // dependencies フィールド: マージ. null 値はキーを削除する.
    if (const auto i_deps = json.find("dependencies"); i_deps != json.end())
    {
        if (!i_deps->is_object())
        {
            throw LibraryConfigError{ "Invalid 'dependencies' field: expected an object." };
        }

        for (const auto & [key, val] : i_deps->items())
        {
            if (val.is_null())
            {
                this->dependencies_.erase(key);
            }
            else if (val.is_object())
            {
                auto & dep = this->dependencies_.try_emplace(key, key).first->second;
                try
                {
                    dep.updateFromJson(val);
                }
                catch (const LibraryConfigError & e)
                {
                    // エラーを, 依存ライブラリ名を付加して再スローする.
                    throw LibraryConfigError{ "Error in dependency spec for '{}': {}", key, e.what() };
                }
            }
            else
            {
                throw LibraryConfigError{ "Invalid dependency value type for '{}': expected an object or null.", key };
            }
        }
    }

    // steps フィールド: マージ. null 値はキーを削除する.
    // order field は特殊
    if (auto i_steps = json.find("steps"); i_steps != json.end())
    {
        if (!i_steps->is_object())
        {
            throw LibraryConfigError{ "Invalid 'steps' field: expected an object." };
        }

        if (auto i_order = i_steps->find("order"); i_order != i_steps->end())
        {
            if (!i_order->is_array())
            {
                throw LibraryConfigError{ "Invalid 'order' field: expected an array." };
            }

            this->order_.clear();
            for (const auto & v : *i_order)
            {
                if (v.is_string())
                {
                    this->order_.push_back(v.get<std::string>());
                }
                else
                {
                    throw LibraryConfigError{ "Invalid 'order' array element: expected a string." };
                }
            }
        }

        for (const auto & [key, val] : json["steps"].items())
        {
            if (key == "order")
            {
                continue;
            }

            if (val.is_null())
            {
                this->steps_.erase(key);
            }
            else if (val.is_object())
            {
                // TODO: ここ, key がすでに設定されていたら update する挙動にする.
                auto & step = this->steps_.try_emplace(key).first->second;
                if (!step)
                {
                    auto new_instance = makeStepDefFromJson(val);
                    new_instance->loadFromJson(val);
                    step = std::move(new_instance);
                }
                else
                {
                    // override する必要があるので, clone する
                    auto cloned_instance = step->clone();
                    cloned_instance->loadFromJson(val);
                    step = std::move(cloned_instance);
                }
            }
            else
            {
                throw LibraryConfigError{ "Invalid step value type for key '{}': expected an object or null.", key };
            }
        }
    }

    if (auto i_license = json.find("license"); i_license != json.end())
    {
        if (!i_license->is_string())
        {
            throw LibraryConfigError{ "Invalid 'license' field: expected a string." };
        }
        this->license_file_path_ = i_license->get<std::string>();
    }
}

const std::string &
roah::distb::config::LibraryEntry::getVersion() const noexcept
{
    return this->version_;
}

const std::unordered_map<std::string, roah::distb::utils::OptionValue> &
roah::distb::config::LibraryEntry::getOptions() const noexcept
{
    return this->options_;
}

const std::unordered_map<std::string, roah::distb::config::DependencySpec> &
roah::distb::config::LibraryEntry::getDependencies() const noexcept
{
    return this->dependencies_;
}

const std::vector<std::string> &
roah::distb::config::LibraryEntry::getStepOrder() const noexcept
{
    return this->order_;
}

const std::string &
roah::distb::config::LibraryEntry::getLicenseFilePath() const noexcept
{
    return this->license_file_path_;
}

void
roah::distb::config::LibraryEntry::build(WorkingContext & working_ctx) const
{
    for (const auto & step_name : this->order_)
    {
        const auto i_step = this->steps_.find(step_name);
        if (i_step == this->steps_.end())
        {
            throw LibraryConfigError{ "Step '{}' is not defined in the library entry.", step_name };
        }
        const auto & step = i_step->second;
        working_ctx.registRuntimeVariable("current_step_name", step_name, false);
        working_ctx.registRuntimeVariable("current_step_cmd", std::string{ step->getCmd() }, false);
        (*step)(working_ctx);
    }
}
