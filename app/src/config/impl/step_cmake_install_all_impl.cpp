#include "step_cmake_install_all_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

roah::distb::config::impl::StepCMakeInstallAllImpl::StepCMakeInstallAllImpl()
    : StepDef{ kCmd }
{}

roah::distb::config::impl::StepCMakeInstallAllImpl::StepCMakeInstallAllImpl(const StepCMakeInstallAllImpl &) = default;

roah::distb::config::impl::StepCMakeInstallAllImpl::StepCMakeInstallAllImpl(StepCMakeInstallAllImpl &&) noexcept
    = default;

roah::distb::config::impl::StepCMakeInstallAllImpl &
roah::distb::config::impl::StepCMakeInstallAllImpl::operator=(const StepCMakeInstallAllImpl &)
    = default;

roah::distb::config::impl::StepCMakeInstallAllImpl &
roah::distb::config::impl::StepCMakeInstallAllImpl::operator=(StepCMakeInstallAllImpl &&) noexcept
    = default;

roah::distb::config::impl::StepCMakeInstallAllImpl::~StepCMakeInstallAllImpl() noexcept = default;

void
roah::distb::config::impl::StepCMakeInstallAllImpl::operator()(const WorkingContext & context) const
{
    AppError::check(!this->build_dir_.empty(), "Build directory is empty.");

    logger.log("CMake Install (Debug/Release): {}", this->build_dir_);

    // Path を決定する
    const auto & root        = context.getCurrentWorkingDirectory();
    const auto   build_dir   = utils::makeAbsolutePath(root / context.resolveString(this->build_dir_));
    const auto   install_dir = utils::makeAbsolutePath(context.resolveString("${installDir}"));
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved build directory path: {}", build_dir.u8string());
    logger.trace("Resolved install directory path: {}", install_dir.u8string());

    // ".." などで root の外に出ている可能性がある.
    if (!utils::isSubDirectory(root, build_dir))
    {
        throw LibraryConfigError{
            "StepCMakeInstallAllImpl: build directory path is outside of the working directory."
        };
    }

    // install dir 消す
    if (std::filesystem::exists(install_dir))
    {
        logger.trace("Removing existing install directory.");
        std::filesystem::remove_all(install_dir);
    }

    // build dir あるか
    if (!std::filesystem::exists(build_dir))
    {
        throw AppError{ "StepCMakeInstallAllImpl: source directory does not exist: {}", build_dir.u8string() };
    }

    // install する
    std::vector<std::u8string> build_cmd{ {
        utils::toU8String(context.resolveString("${cmakeBin}")),
        u8"--build",
        build_dir.u8string(),
        u8"--config",
        u8"",
    } };
    std::vector<std::u8string> install_cmd{ {
        utils::toU8String(context.resolveString("${cmakeBin}")),
        u8"--install",
        build_dir.u8string(),
        u8"--prefix",
        install_dir.u8string(),
        u8"--config",
        u8"",
    } };

    const auto build_fn = [&](const std::u8string & config) {
        logger.log("Executing CMake Build and Install ({})...", config);
        build_cmd.back()   = config;
        install_cmd.back() = config;

        // build
        const auto ret_build = utils::run(build_cmd,
                                          {
                                              .capture_stdout = false,
                                              .capture_stderr = false,
                                          });
        AppError::check(ret_build.exit_code == 0,
                        "CMake build ({}) failed with exit code {}.",
                        config,
                        ret_build.exit_code);
        logger.log("CMake build ({}) completed successfully.", config);

        // install
        const auto ret_install = utils::run(install_cmd,
                                            {
                                                .capture_stdout = false,
                                                .capture_stderr = false,
                                            });
        AppError::check(ret_install.exit_code == 0,
                        "CMake install ({}) failed with exit code {}.",
                        config,
                        ret_install.exit_code);
        logger.log("CMake install ({}) completed successfully.", config);
    };

    build_fn(u8"Debug");
    build_fn(u8"Release");
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepCMakeInstallAllImpl::clone() const
{
    return std::make_unique<StepCMakeInstallAllImpl>(*this);
}

void
roah::distb::config::impl::StepCMakeInstallAllImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "build_dir", this->build_dir_);
}
