#ifndef ROAH_DISTB_CONFIG_LIBRARY_ENTRY_HPP
#define ROAH_DISTB_CONFIG_LIBRARY_ENTRY_HPP

#include "dependency_spec.hpp"
#include "step_def.hpp"
//
#include "roah/distb/utils/option_value.hpp"

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace roah::distb::config {

/// @brief ライブラリの単一バージョンエントリを表すクラス.
///
/// base フィールドを使用することで, 継承による差分定義が可能.
/// 継承エントリでは, 明記されたフィールドは上書き, null 値は削除, 省略は継承となる.
class LibraryEntry
{
public:
    LibraryEntry(std::string version);

    LibraryEntry(const LibraryEntry &) = delete;
    LibraryEntry(LibraryEntry &&) noexcept;
    LibraryEntry &
    operator=(const LibraryEntry &)
        = delete;
    LibraryEntry &
    operator=(LibraryEntry &&) noexcept;
    ~LibraryEntry() noexcept;

    void
    setBase(const LibraryEntry & base_entry);

    void
    updateFromJson(const nlohmann::json & json);

    const std::string &
    getVersion() const noexcept;

    const std::unordered_map<std::string, utils::OptionValue> &
    getOptions() const noexcept;

    const std::unordered_map<std::string, DependencySpec> &
    getDependencies() const noexcept;

    const std::vector<std::string> &
    getStepOrder() const noexcept;

    // const std::unordered_map<std::string, std::unique_ptr<StepDef>> &
    // getSteps() const noexcept;

private:
    class StepDefHolder
    {
    public:
        StepDefHolder();
        StepDefHolder(std::unique_ptr<StepDef> && instance) noexcept;
        StepDefHolder(StepDefHolder &&) noexcept;
        StepDefHolder &
        operator=(StepDefHolder &&) noexcept;
        ~StepDefHolder() noexcept;

        StepDefHolder(const StepDefHolder &) = delete;
        StepDefHolder &
        operator=(const StepDefHolder &)
            = delete;

        const StepDef &
        ref() const;

        StepDefHolder
        copy() const noexcept;

    private:
        StepDefHolder(const StepDef * ptr) noexcept;

        std::unique_ptr<StepDef> instance_;
        const StepDef *          ptr_;
    };

    std::string                                         version_;
    std::unordered_map<std::string, utils::OptionValue> options_;
    std::unordered_map<std::string, DependencySpec>     dependencies_;
    std::vector<std::string>                            order_;
    std::unordered_map<std::string, StepDefHolder>      steps_;
};

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFJG_LIBRARY_ENTRY_HPP
