#ifndef ROAH_DISTB_CONFIG_DEPENDENCY_SPEC_HPP
#define ROAH_DISTB_CONFIG_DEPENDENCY_SPEC_HPP

#include "roah/distb/config/variables.hpp"
#include "roah/distb/utils/delay_copyable_container.hpp"
#include "roah/distb/utils/option_value.hpp"

#include <nlohmann/json_fwd.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace roah::distb::config {

class Condition;

/// @brief ライブラリのビルドに必要な依存関係を表すクラス.
class DependencySpec
{
public:
    DependencySpec(std::string name);
    DependencySpec(const DependencySpec &);
    DependencySpec(DependencySpec &&) noexcept;
    DependencySpec &
    operator=(const DependencySpec &);
    DependencySpec &
    operator=(DependencySpec &&) noexcept;
    ~DependencySpec();

    /// @brief JSON から依存関係の情報を更新する.
    ///
    /// 現状持っている情報にマージする形
    void
    updateFromJson(const nlohmann::json & json);

    const std::string &
    getName() const noexcept;

    const std::vector<std::string> &
    getRequiredVersionRange() const noexcept;

    const std::unordered_map<std::string, utils::OptionValue> &
    getOptions() const noexcept;

    bool
    evalCondition(const config::Variables & variables) const;

private:
    using ConditionHolder = utils::DelayCopyableContainer<Condition>;

    std::string                                         name_;  ///< 依存するライブラリの名前.
    std::vector<std::string>                            required_version_range_;
    std::unordered_map<std::string, utils::OptionValue> options_;
    ConditionHolder                                     condition_;
};

}  // namespace roah::distb::config

#endif  // ROAH_DISTB_CONFJG_DEPENDENCY_SPEC_HPP
