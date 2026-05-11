// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "roah/distb/config/library_entry.hpp"

#include "impl/step_wget_impl.hpp"

#include <nlohmann/json.hpp>

#include <cstdint>

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
    // base_entry の全フィールドをコピーして基底として設定する.
    // その後に updateFromJson を呼ぶことで差分が適用される.
    this->options_      = base_entry.options_;
    this->dependencies_ = base_entry.dependencies_;
    this->recipes_      = base_entry.recipes_;
    // unique_ptr はコピー不可なので, 各ステップを clone() で複製する.
    this->steps_.clear();
    for (const auto & [key, ptr] : base_entry.steps_)
    {
        if (ptr)
        {
            this->steps_.emplace(key, ptr->clone());
        }
    }
}

void
roah::distb::config::LibraryEntry::updateFromJson(const nlohmann::json & json)
{
    if (!json.is_object())
    {
        return;
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

    // dependencies フィールド: マージ. null 値はキーを削除する.
    if (json.contains("dependencies") && json["dependencies"].is_object())
    {
        for (const auto & [key, val] : json["dependencies"].items())
        {
            if (val.is_null())
            {
                this->dependencies_.erase(key);
            }
            else if (val.is_object())
            {
                // name フィールドが明記されていればそれを使い, なければキー名を使う.
                std::string dep_name = key;
                if (val.contains("name") && val["name"].is_string())
                {
                    dep_name = val["name"].get<std::string>();
                }

                auto it = this->dependencies_.find(key);
                if (it == this->dependencies_.end())
                {
                    DependencySpec dep{ std::move(dep_name) };
                    dep.updateFromJson(val);
                    this->dependencies_.emplace(key, std::move(dep));
                }
                else
                {
                    it->second.updateFromJson(val);
                }
            }
        }
    }

    // recipes フィールド: 存在する場合は全体を上書き.
    if (json.contains("recipes") && json["recipes"].is_array())
    {
        this->recipes_.clear();
        for (const auto & r : json["recipes"])
        {
            if (r.is_string())
            {
                this->recipes_.push_back(r.get<std::string>());
            }
        }
    }

    // steps フィールド: マージ. null 値はキーを削除する.
    if (json.contains("steps") && json["steps"].is_object())
    {
        for (const auto & [key, val] : json["steps"].items())
        {
            if (val.is_null())
            {
                this->steps_.erase(key);
            }
            else if (val.is_object())
            {
                if (!val.contains("cmd") || !val["cmd"].is_string())
                {
                    continue;
                }
                const auto cmd = val["cmd"].get<std::string>();
                if (cmd == "wget")
                {
                    auto step = StepWgetImpl::loadFromJson(val);
                    if (step)
                    {
                        this->steps_.insert_or_assign(key, std::move(step));
                    }
                }
                else
                {
                    // TODO: "subp", "cmake-configure", "cmake-build" などのステップは未実装.
                }
            }
        }
    }
}

const std::string &
roah::distb::config::LibraryEntry::getVersion() const noexcept
{
    return this->version_;
}

const std::unordered_map<std::string, roah::distb::config::OptionValue> &
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
roah::distb::config::LibraryEntry::getRecipes() const noexcept
{
    return this->recipes_;
}

const std::unordered_map<std::string, std::unique_ptr<roah::distb::config::StepDef>> &
roah::distb::config::LibraryEntry::getSteps() const noexcept
{
    return this->steps_;
}
