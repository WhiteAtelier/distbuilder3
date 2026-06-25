#include "step_download_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/hash.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <vector>

roah::distb::config::impl::StepDownloadImpl::StepDownloadImpl()
    : StepDef{ kCmd }
{}

roah::distb::config::impl::StepDownloadImpl::~StepDownloadImpl() noexcept = default;

void
roah::distb::config::impl::StepDownloadImpl::operator()(const WorkingContext & context) const
{
    AppError::check(!this->url_.empty(), "URL is empty.");
    AppError::check(!this->output_.empty(), "Output path is empty.");
    AppError::check(!this->hash_.empty(), "Hash is empty.");
    logger.log("Download: {}", this->url_);

    // Path を決定する
    const auto & root   = context.getCurrentWorkingDirectory();
    const auto   output = utils::makeAbsolutePath(root / this->output_);
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved output path: {}", output.u8string());

    if (utils::isSubDirectory(root, output))
    {
        // ".." などで root の外に出ている可能性がある.
        throw LibraryConfigError{ "StepDownloadImpl: output path is outside of the working directory." };
    }

    // すでにファイルが存在する場合, その hash を検査して一致したらダウンロードをスキップする.
    if (std::filesystem::exists(output))
    {
        logger.trace("Output file is already existing. Checking hash...");
        const auto ret = this->_calculateHash(output);
        if (ret == this->hash_)
        {
            logger.log("-- Download: output file already exists and hash matches. Skip download.");
            return;
        }

        // 一致しなかったのでファイル削除する
        logger.trace("-- Hash check NG. Redownload.");
        std::filesystem::remove(output);
    }

    // curl を使用して url からファイルをダウンロードする.
    std::vector<std::u8string> cmd = { u8"curl", u8"--fail" };

    cmd.emplace_back(u8"-o");
    cmd.emplace_back(output.u8string());
    cmd.emplace_back(this->url_);

    const auto result = utils::run(cmd,
                                   {
                                       .print_stdout = logger.isVerbose(),
                                       .print_stderr = logger.isVerbose(),
                                   });
    if (result.exit_code != 0)
    {
        throw std::runtime_error{ "StepDownloadImpl: curl exited with code " + std::to_string(result.exit_code) + "." };
    }

    // 再度 hash 検証
    logger.trace("hash calculating... Expected = {}", this->hash_);
    const auto actual = this->_calculateHash(output);
    logger.trace("hash calculated.    Actual   = {}", actual);
    AppError::check(actual == this->hash_, "StepDownloadImpl: SHA-256 hash mismatch. URL = {}", this->url_);

    logger.log("Download OK.")
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepDownloadImpl::clone() const
{
    return std::make_unique<StepDownloadImpl>(*this);
}

void
roah::distb::config::impl::StepDownloadImpl::loadFromJson(const nlohmann::json & json)
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

    get_fn("url", this->url_);
    get_fn("output", this->output_);
    get_fn("hash", this->hash_);
}

const std::string &
roah::distb::config::impl::StepDownloadImpl::getUrl() const noexcept
{
    return this->url_;
}

const std::string &
roah::distb::config::impl::StepDownloadImpl::getOutput() const noexcept
{
    return this->output_;
}

const std::string &
roah::distb::config::impl::StepDownloadImpl::getHash() const noexcept
{
    return this->hash_;
}

std::string
roah::distb::config::impl::StepDownloadImpl::_calculateHash(const std::filesystem::path & path) const
{
    // ファイルをバイナリで読み込む.
    std::ifstream ifs{ path, std::ios::binary };
    if (!ifs)
    {
        throw AppError{ "StepDownloadImpl: failed to open downloaded file: " + this->output_ };
    }

    utils::SHA256Hash sha;
    std::vector<char> buf(65536, 0);
    while (ifs.read(buf.data(), static_cast<std::streamsize>(buf.size())) || ifs.gcount() > 0)
    {
        sha.addData(buf.data(), static_cast<std::size_t>(ifs.gcount()));
    }

    return sha.getHash();
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepDownloadImplGenerator::operator()() const
{
    return std::make_unique<StepDownloadImpl>();
}