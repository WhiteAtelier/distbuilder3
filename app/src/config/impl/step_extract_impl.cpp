#include "step_extract_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <nlohmann/json.hpp>

roah::distb::config::impl::StepExtractImpl::StepExtractImpl()
    : StepDef{ kCmd }
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
    // 入力, 出力のパスを生成する.
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepExtractImpl::clone() const
{
    auto ret     = std::unique_ptr<StepExtractImpl>();
    ret->input_  = this->input_;
    ret->output_ = this->output_;
    return ret;
}

void
roah::distb::config::impl::StepExtractImpl::loadFromJson(const nlohmann::json & json)
{
    const auto get_fn = [&](const std::string & key, std::string & out) {
        auto iter = json.find(key);
        if (iter == json.end())
        {
            throw LibraryConfigError{ "Invalid 'extract' step definition: missing required field '{}'.", key };
        }
        if (!iter->is_string())
        {
            throw LibraryConfigError{ "Invalid 'extract' step definition: field '{}' must be a string.", key };
        }
        iter->get_to(out);
    };

    get_fn("input", this->input_);
    get_fn("output", this->output_);
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepExtractImplGenerator::operator()() const
{
    return std::make_unique<StepExtractImpl>();
}
