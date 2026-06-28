#include "step_install_file_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

roah::distb::config::impl::StepInstallFileImpl::StepInstallFileImpl()
    : StepDef{ kCmd }
{}

roah::distb::config::impl::StepInstallFileImpl::StepInstallFileImpl(const StepInstallFileImpl &) = default;

roah::distb::config::impl::StepInstallFileImpl::StepInstallFileImpl(StepInstallFileImpl &&) noexcept = default;

roah::distb::config::impl::StepInstallFileImpl &
roah::distb::config::impl::StepInstallFileImpl::operator=(const StepInstallFileImpl &)
    = default;

roah::distb::config::impl::StepInstallFileImpl &
roah::distb::config::impl::StepInstallFileImpl::operator=(StepInstallFileImpl &&) noexcept
    = default;

roah::distb::config::impl::StepInstallFileImpl::~StepInstallFileImpl() noexcept = default;

void
roah::distb::config::impl::StepInstallFileImpl::operator()(const WorkingContext & context) const
{
    AppError::check(!this->source_file_.empty(), "Source file is empty.");
    AppError::check(!this->destination_dir_.empty(), "Destination directory is empty.");

    logger.log("Install file: {} file(s)", this->source_file_.size());

    // Path を決定する
    const auto & root        = context.getCurrentWorkingDirectory();
    const auto   install_dir = utils::makeAbsolutePath(context.resolveString("${install_dir}"));
    const auto   dest_dir = utils::makeAbsolutePath(context.resolveString("${install_dir}/" + this->destination_dir_));

    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved destination directory path: {}", install_dir.u8string());

    // ".." などで root の外に出ている可能性がある.
    if (!utils::isSubDirectory(install_dir, dest_dir))
    {
        throw LibraryConfigError{
            "StepInstallFileImpl: Destination directory path is outside of the install directory."
        };
    }

    for (const auto & src_file : this->source_file_)
    {
        const auto src_path = utils::makeAbsolutePath(root / context.resolveString(src_file));

        // ".." などで root の外に出ている可能性がある.
        if (!utils::isSubDirectory(root, src_path))
        {
            throw LibraryConfigError{ "StepInstallFileImpl: Source file path is outside of the working directory." };
        }

        logger.log("Installing file: {} -> {}", src_path.u8string(), dest_dir.u8string());
        std::filesystem::copy(src_path,
                              dest_dir / src_path.filename(),
                              std::filesystem::copy_options::overwrite_existing);
    }

    logger.log("Copy {} file(s) completed successfully.", this->source_file_.size());
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepInstallFileImpl::clone() const
{
    return std::make_unique<StepInstallFileImpl>(*this);
}

void
roah::distb::config::impl::StepInstallFileImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "destination_dir", this->destination_dir_);

    if (const auto iter = json.find("source_file"); iter != json.end())
    {
        if (iter->is_array())
        {
            for (const auto & item : *iter)
            {
                if (!item.is_string())
                {
                    throw LibraryConfigError{ "StepInstallFileImpl: 'source_file' array must contain only strings." };
                }
                this->source_file_.emplace(item.get<std::string>());
            }
        }
        else if (iter->is_string())
        {
            this->source_file_.emplace(iter->get<std::string>());
        }
        else
        {
            throw LibraryConfigError{ "StepInstallFileImpl: 'source_file' must be a string or an array of strings." };
        }
    }
}
