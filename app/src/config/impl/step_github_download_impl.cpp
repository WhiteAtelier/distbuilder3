#include "step_github_download_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/working_context.hpp"
#include "step_download_impl.hpp"
#include "step_extract_impl.hpp"

#include <nlohmann/json.hpp>

#include <format>

roah::distb::config::impl::StepGithubDownloadImpl::StepGithubDownloadImpl()
    : StepDef{ kCmd }
    , output_dir_{ "src" }
    , ref_{}
    , hash_{}
    , author_{ "${author}" }
    , repo_{ "${repo}" }
    , archive_path_{ "src.zip" }
    , error_ok_{ false }
{}

roah::distb::config::impl::StepGithubDownloadImpl::StepGithubDownloadImpl(const StepGithubDownloadImpl &) = default;

roah::distb::config::impl::StepGithubDownloadImpl::StepGithubDownloadImpl(StepGithubDownloadImpl &&) noexcept = default;

roah::distb::config::impl::StepGithubDownloadImpl::~StepGithubDownloadImpl() noexcept = default;

void
roah::distb::config::impl::StepGithubDownloadImpl::operator()(WorkingContext & context) const
{
    AppError::check(this->hash_.size() == 64, "'hash' is invalid SHA256 string.");
    AppError::check(!this->author_.empty(), "'author' is empty.");
    AppError::check(!this->repo_.empty(), "'repo' is empty.");
    AppError::check(!this->ref_.empty(), "'ref' is empty.");
    AppError::check(!this->output_dir_.empty(), "'output_dir' is empty.");
    AppError::check(!this->archive_path_.empty(), "'archive_path' is empty.");

    // Path を決定する
    const auto & root       = context.getCurrentWorkingDirectory();
    const auto   output_dir = utils::makeAbsolutePath(root / context.resolveString(this->output_dir_));
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved output path: {}", output_dir.u8string());

    const auto current_step_name = context.resolveString("${current_step_name}");

    // ".." などで root の外に出ている可能性がある.
    if (!utils::isSubDirectory(root, output_dir))
    {
        throw LibraryConfigError{ "{}: output_dir path is outside of the working directory.", this->getCmd() };
    }

    {
        // Download
        std::vector<std::u8string> curl_extra_args;

        curl_extra_args.emplace_back(u8"-H");
        curl_extra_args.emplace_back(u8"X-GitHub-Api-Version: 2026-03-10");
        if (const auto & pat = context.getGitHubPublicAccessToken(); !pat.empty())
        {
            curl_extra_args.emplace_back(u8"-H");
            curl_extra_args.emplace_back(u8"Authorization: Bearer " + utils::toU8String(pat));
        }

        StepDownloadImpl download{ kCmd,
                                   "https://api.github.com/repos/${author}/${repo}/zipball/" + this->ref_,
                                   this->archive_path_,
                                   this->hash_,
                                   std::move(curl_extra_args) };
        download(context);
    }

    {
        // Extract
        StepExtractImpl extract{ kCmd,
                                 "${step." + current_step_name + ".output}",
                                 this->output_dir_ + ".tmp",
                                 true,
                                 this->error_ok_ };
        extract(context);
    }

    // tmp_output の中にはディレクトリがもうひとつ含まれている.
    // そのディレクトリ名は ${author}-${repo}-${commitHash} となっているが,
    // commit hash が分からない.
    // ディレクトリを覗いて, とりあえずそのひとつのディレクトリをひっぱりあげて, src と名前をつける.
    for (const auto & entry :
         std::filesystem::directory_iterator{ context.resolveString("${step." + current_step_name + ".output}") })
    {
        if (entry.is_directory())
        {
            const auto src_dir = entry.path();
            std::filesystem::remove_all(output_dir);
            std::filesystem::rename(src_dir, output_dir);
            break;
        }
    }
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepGithubDownloadImpl::clone() const
{
    return std::make_unique<StepGithubDownloadImpl>(*this);
}

void
roah::distb::config::impl::StepGithubDownloadImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "output_dir", this->output_dir_);
    this->_getStringFromJson(kCmd, json, "ref", this->ref_);
    this->_getStringFromJson(kCmd, json, "hash", this->hash_);
    this->_getStringFromJson(kCmd, json, "author", this->author_);
    this->_getStringFromJson(kCmd, json, "repo", this->repo_);
    this->_getStringFromJson(kCmd, json, "archive_path", this->archive_path_);
    this->_getBoolFromJson(kCmd, json, "error_ok", this->error_ok_);
}
