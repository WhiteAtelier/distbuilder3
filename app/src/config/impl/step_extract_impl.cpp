#include "step_extract_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::StepExtractImpl::StepExtractImpl()
    : StepDef{ kCmd }
    , output_{ "." }  // = working directory
    , verbosity_{ false }
    , error_ok_{ false }
{}

roah::distb::config::impl::StepExtractImpl::StepExtractImpl(const StepExtractImpl &) = default;

roah::distb::config::impl::StepExtractImpl::StepExtractImpl(StepExtractImpl &&) noexcept = default;

roah::distb::config::impl::StepExtractImpl &
roah::distb::config::impl::StepExtractImpl::operator=(const StepExtractImpl &)
    = default;

roah::distb::config::impl::StepExtractImpl &
roah::distb::config::impl::StepExtractImpl::operator=(StepExtractImpl &&) noexcept
    = default;

roah::distb::config::impl::StepExtractImpl::~StepExtractImpl() noexcept = default;

void
roah::distb::config::impl::StepExtractImpl::operator()(const WorkingContext & context) const
{
    AppError::check(!this->input_.empty(), "Input is empty.");
    AppError::check(!this->output_.empty(), "Output path is empty.");

    logger.log("Extract: {} -> {}", this->input_, this->output_);

    // Path を決定する
    const auto & root   = context.getCurrentWorkingDirectory();
    const auto   input  = utils::makeAbsolutePath(root / context.resolveString(this->input_));
    const auto   output = utils::makeAbsolutePath(root / context.resolveString(this->output_));
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved input path: {}", input.u8string());
    logger.trace("Resolved output path: {}", output.u8string());

    // ".." などで root の外に出ている可能性がある.
    if (!utils::isSubDirectory(root, input))
    {
        throw LibraryConfigError{ "StepExtractImpl: input path is outside of the working directory." };
    }
    if (!utils::isSubDirectory(root, output))
    {
        throw LibraryConfigError{ "StepExtractImpl: output path is outside of the working directory." };
    }

    if (!std::filesystem::exists(input))
    {
        throw AppError{ "StepExtractImpl: input file does not exist: {}", input.u8string() };
    }

    if (root != output)
    {
        if (std::filesystem::exists(output))
        {
            logger.trace("Output directory already exists. Remove.");
            std::filesystem::remove_all(output);
        }
        std::filesystem::create_directories(output);
    }

    logger.trace("Extracting...");

    // 解凍する
    std::vector<std::u8string> cmd{ {
        u8"tar",
        u8"-xf",
        input.u8string(),
        u8"-C",
        output.u8string(),
    } };
    if (this->verbosity_)
    {
        cmd.emplace_back(u8"-v");
    }

    const auto result = utils::run(cmd,
                                   {
                                       .print_stdout = logger.isVerbose(),
                                       .print_stderr = logger.isVerbose(),
                                   });

    if (!this->error_ok_)
    {
        AppError::check(result.exit_code == 0, "StepExtractImpl: tar exited with code {}.", result.exit_code);
    }

    logger.log("Extract Done.");
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepExtractImpl::clone() const
{
    return std::make_unique<StepExtractImpl>(*this);
}

void
roah::distb::config::impl::StepExtractImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "input", this->input_);
    this->_getStringFromJson(kCmd, json, "output", this->output_);
    this->_getBoolFromJson(kCmd, json, "verbosity", this->verbosity_);
    this->_getBoolFromJson(kCmd, json, "error_ok", this->error_ok_);
}
