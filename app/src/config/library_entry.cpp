#include "roah/distb/config/library_entry.hpp"

#include "roah/distb/config/step_def.hpp"
#include "roah/distb/errors.hpp"

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
    this->options_      = base_entry.options_;
    this->dependencies_ = base_entry.dependencies_;
    this->order_        = base_entry.order_;

    this->steps_.clear();
    for (const auto & [key, ptr] : base_entry.steps_)
    {
        this->steps_.try_emplace(key, ptr.copy());
    }
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

        if (auto i_order = json.find("order"); i_order != json.end())
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
                this->steps_[key] = makeStepDefFromJson(val);
            }
            else
            {
                throw LibraryConfigError{ "Invalid step value type for key '{}': expected an object or null.", key };
            }
        }
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

// const std::unordered_map<std::string, std::unique_ptr<roah::distb::config::StepDef>> &
// roah::distb::config::LibraryEntry::getSteps() const noexcept
//{
//     return this->steps_;
// }

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// StepDefHolder class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
roah::distb::config::LibraryEntry::StepDefHolder::StepDefHolder()
    : instance_{ nullptr }
    , ptr_{ nullptr }
{}

roah::distb::config::LibraryEntry::StepDefHolder::StepDefHolder(std::unique_ptr<StepDef> && instance) noexcept
    : instance_{ std::move(instance) }
    , ptr_{ this->instance_.get() }
{}

roah::distb::config::LibraryEntry::StepDefHolder::StepDefHolder(const StepDef * ptr) noexcept
    : ptr_{ ptr }
{}

roah::distb::config::LibraryEntry::StepDefHolder::StepDefHolder(StepDefHolder &&) noexcept = default;

roah::distb::config::LibraryEntry::StepDefHolder &
roah::distb::config::LibraryEntry::StepDefHolder::operator=(StepDefHolder &&) noexcept
    = default;

roah::distb::config::LibraryEntry::StepDefHolder::~StepDefHolder() noexcept = default;

const roah::distb::config::StepDef &
roah::distb::config::LibraryEntry::StepDefHolder::ref() const
{
    if (this->ptr_ == nullptr)
    {
        throw std::runtime_error{ "null StepDef access." };
    }
    return *this->ptr_;
}

roah::distb::config::LibraryEntry::StepDefHolder
roah::distb::config::LibraryEntry::StepDefHolder::copy() const noexcept
{
    return StepDefHolder{ this->ptr_ };
}
