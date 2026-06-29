#include "step_cmake_configure_impl.hpp"

#include "roah/distb/config/condition.hpp"
#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

roah::distb::config::impl::StepCMakeConfigureImpl::StepCMakeConfigureImpl()
    : StepDef{ kCmd }
    , source_dir_{ "src" }
    , build_dir_{ "build" }
    , configs_{ "Debug", "Release" }
{}

roah::distb::config::impl::StepCMakeConfigureImpl::StepCMakeConfigureImpl(const StepCMakeConfigureImpl &) = default;

roah::distb::config::impl::StepCMakeConfigureImpl::StepCMakeConfigureImpl(StepCMakeConfigureImpl &&) noexcept = default;

roah::distb::config::impl::StepCMakeConfigureImpl::~StepCMakeConfigureImpl() noexcept = default;

void
roah::distb::config::impl::StepCMakeConfigureImpl::operator()(WorkingContext & context) const
{
    AppError::check(!this->source_dir_.empty(), "Source directory is empty.");
    AppError::check(!this->build_dir_.empty(), "Build directory is empty.");

    logger.log("CMake Configure: {} -> {}", this->source_dir_, this->build_dir_);

    // Path を決定する
    const auto & root      = context.getCurrentWorkingDirectory();
    const auto   src_dir   = utils::makeAbsolutePath(root / context.resolveString(this->source_dir_));
    const auto   build_dir = utils::makeAbsolutePath(root / context.resolveString(this->build_dir_));
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved source directory path: {}", src_dir.u8string());
    logger.trace("Resolved build directory path: {}", build_dir.u8string());

    // ".." などで root の外に出ている可能性がある.
    if (!utils::isSubDirectory(root, src_dir))
    {
        throw LibraryConfigError{
            "StepCMakeConfigureImpl: source directory path is outside of the working directory."
        };
    }
    if (!utils::isSubDirectory(root, build_dir))
    {
        throw LibraryConfigError{ "StepCMakeConfigureImpl: build directory path is outside of the working directory." };
    }

    // 同じディレクトリは指定できない.
    if (src_dir == build_dir)
    {
        throw LibraryConfigError{ "StepCMakeConfigureImpl: source directory and build directory must be different." };
    }

    // build_dir は削除されてしまう, build_dir の中に src_dir があるならエラーにする.
    if (utils::isSubDirectory(build_dir, src_dir))
    {
        throw LibraryConfigError{
            "StepCMakeConfigureImpl: build directory must not be a subdirectory of source directory."
        };
    }

    // build dir 消す
    if (std::filesystem::exists(build_dir))
    {
        logger.trace("Removing existing build directory.");
        std::filesystem::remove_all(build_dir);
    }

    // src dir あるか
    if (!std::filesystem::exists(src_dir))
    {
        throw AppError{ "StepCMakeConfigureImpl: source directory does not exist: {}", src_dir.u8string() };
    }

    // command line args
    std::vector<std::u8string> cmd{ {
        utils::toU8String(context.resolveString("${cmake_executable}")),  //
        u8"-S",
        src_dir.u8string(),  // source dir
        u8"-B",
        build_dir.u8string(),  // build dir
    } };

    if (const auto generator = context.resolveString("${generator}"); !generator.empty())
    {
        cmd.emplace_back(u8"-G");
        cmd.emplace_back(utils::toU8String(generator));
    }
    if (const auto arch = context.resolveString("${arch}"); !arch.empty())
    {
        cmd.emplace_back(u8"-A");
        cmd.emplace_back(utils::toU8String(arch));
    }
    for (const auto & arg_subset : this->args_ | std::ranges::views::values)
    {
        if (!arg_subset.condition || context.evalCondition(*arg_subset.condition))
        {
            for (const auto & arg : arg_subset.args)
            {
                cmd.emplace_back(utils::toU8String(context.resolveString(arg)));
            }
        }
    }
    cmd.emplace_back(u8"-DCMAKE_DEBUG_POSTFIX=d");

    std::string config_types;
    for (const auto & config : this->configs_)
    {
        if (!config_types.empty())
        {
            config_types += ";";
        }
        config_types += config;
    }
    cmd.emplace_back(u8"-DCMAKE_CONFIGURATION_TYPES=" + utils::toU8String(config_types));

    if (const auto cxx_standard = context.resolveString("${cxx_standard}");  //
        !cxx_standard.empty())
    {
        cmd.emplace_back(u8"-DCMAKE_CXX_STANDARD=" + utils::toU8String(cxx_standard));
    }

    // Dependencies
    std::u8string deps;
    for (const auto & [lib_name, lib_hash] : context.getDependencies())
    {
        deps += utils::toU8String(context.resolveString("${install_root_dir}/" + lib_name + "/" + lib_hash + ";"));
    }
    if (!deps.empty())
    {
        cmd.emplace_back(u8"-DCMAKE_PREFIX_PATH=" + deps);
    }

    // cmake execute
    logger.trace("Executing CMake Configure...");
    if (logger.isVerbose())
    {
        std::u8string cmdline;
        for (const auto & arg : cmd)
        {
            const auto needs_dq = utils::contains(arg, u8' ');
            if (needs_dq)
            {
                cmdline += u8"\"" + arg + u8"\" ";
            }
            else
            {
                cmdline += arg + u8" ";
            }
        }
        logger.trace("-- CMake command: {}", cmdline);
    }

    const auto ret = utils::run(cmd,
                                {
                                    .capture_stdout = false,
                                    .capture_stderr = false,
                                });
    AppError::check(ret.exit_code == 0, "CMake configure failed with exit code {}.", ret.exit_code);

    logger.log("CMake configure completed successfully.");
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepCMakeConfigureImpl::clone() const
{
    return std::make_unique<StepCMakeConfigureImpl>(*this);
}

void
roah::distb::config::impl::StepCMakeConfigureImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "source_dir", this->source_dir_);
    this->_getStringFromJson(kCmd, json, "build_dir", this->build_dir_);

    if (const auto i_configs = json.find("configs"); i_configs != json.end())
    {
        if (i_configs->is_array())
        {
            this->configs_.clear();
            for (const auto & config : *i_configs)
            {
                if (!config.is_string())
                {
                    throw LibraryConfigError{ "Invalid 'configs' field: expected an array of strings." };
                }
                this->configs_.emplace_back(config.get<std::string>());
            }
        }
        else
        {
            throw LibraryConfigError{ "Invalid 'configs' field: expected an array." };
        }
    }

    if (const auto i_args = json.find("args"); i_args != json.end())
    {
        /*
        args 記法:

        基本型は
        "args": {
            "<subsetName>": {
                "args": ["arg1", "arg2", ...],
                "condition": <conditionObject>
            },
        }

        shorthand として以下も可能.
        "args": {
            "<subsetName>": ["arg1", "arg2", ...],
        }

        また, subsetName を "default" とする場合, 以下のようにも書ける.
        "args": ["arg1", "arg2", ...]
        */

        if (i_args->is_array())
        {
            auto & default_subset = this->args_["default"];
            for (const auto & arg : *i_args)
            {
                if (!arg.is_string())
                {
                    throw LibraryConfigError{ "Invalid 'args' field: expected an array of strings." };
                }
                default_subset.args.emplace_back(arg.get<std::string>());
            }
        }
        else if (i_args->is_object())
        {
            for (const auto & [key, value] : i_args->items())
            {
                if (value.is_null())
                {
                    this->args_.erase(key);
                    continue;
                }

                auto & subset = this->args_[key];
                if (value.is_array())
                {
                    for (const auto & arg : value)
                    {
                        if (!arg.is_string())
                        {
                            throw LibraryConfigError{ "Invalid 'args.{}' field: expected an array of strings.", key };
                        }
                        subset.args.emplace_back(arg.get<std::string>());
                    }
                }
                else if (value.is_object())
                {
                    if (const auto i_subset_args = value.find("args"); i_subset_args != value.end())
                    {
                        if (!i_subset_args->is_array())
                        {
                            throw LibraryConfigError{ "Invalid 'args.{}.args' field: expected an array.", key };
                        }
                        for (const auto & arg : *i_subset_args)
                        {
                            if (!arg.is_string())
                            {
                                throw LibraryConfigError{ "Invalid 'args.{}.args' field: expected an array of strings.",
                                                          key };
                            }
                            subset.args.emplace_back(arg.get<std::string>());
                        }
                    }
                    if (const auto i_condition = value.find("condition"); i_condition != value.end())
                    {
                        if (i_condition->is_null())
                        {
                            subset.condition.reset();
                        }
                        else
                        {
                            auto cd = makeConditionFromJson(*i_condition);
                            cd->loadFromJson(*i_condition);
                            subset.condition = std::move(cd);
                        }
                    }
                }
                else
                {
                    throw LibraryConfigError{ "Invalid 'args.{}' field: expected an object or array.", key };
                }
            }
        }
        else
        {
            throw LibraryConfigError{ "Invalid 'args' field: expected an array." };
        }
    }
}
