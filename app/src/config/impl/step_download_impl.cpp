#include "step_download_impl.hpp"

#include "roah/distb/errors.hpp"
#include "roah/distb/logger.hpp"
#include "roah/distb/utils/base32.hpp"
#include "roah/distb/utils/hash.hpp"
#include "roah/distb/utils/option_value.hpp"
#include "roah/distb/utils/path.hpp"
#include "roah/distb/utils/string.hpp"
#include "roah/distb/utils/subprocess.hpp"
#include "roah/distb/working_context.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <vector>

roah::distb::config::impl::StepDownloadImpl::StepDownloadImpl()
    : StepDef{ kCmd }
{}

roah::distb::config::impl::StepDownloadImpl::StepDownloadImpl(const std::string_view     cmd_name_driven_by,
                                                              std::string                url,
                                                              std::string                output,
                                                              std::string                hash,
                                                              std::vector<std::u8string> curl_extra_args)
    : StepDef{ cmd_name_driven_by }
    , url_{ std::move(url) }
    , output_{ std::move(output) }
    , hash_{ std::move(hash) }
    , curl_extra_args_{ std::move(curl_extra_args) }
{}

roah::distb::config::impl::StepDownloadImpl::StepDownloadImpl(const StepDownloadImpl &) = default;

roah::distb::config::impl::StepDownloadImpl::StepDownloadImpl(StepDownloadImpl &&) noexcept = default;

roah::distb::config::impl::StepDownloadImpl::~StepDownloadImpl() noexcept = default;

void
roah::distb::config::impl::StepDownloadImpl::operator()(WorkingContext & context) const
{
    AppError::check(!this->url_.empty(), "'url' is empty.");
    AppError::check(!this->output_.empty(), "'output' path is empty.");
    AppError::check(this->hash_.size() == 64, "'hash' is invalid SHA256 string.");

    const auto url = context.resolveString(this->url_);

    // hash string を hex 解釈して, バイナリに変換する.
    const auto hash_bin = utils::toBinaryFromHexString(this->hash_);
    const auto hash_b32 = utils::encodeBase32(hash_bin.data(), hash_bin.size(), true, false);

    logger.log("Download: {}", url);

    // Path を決定する
    const auto & root   = context.getCurrentWorkingDirectory();
    const auto   output = utils::makeAbsolutePath(root / context.resolveString(this->output_));
    logger.trace("Working directory: {}", root.u8string());
    logger.trace("Resolved output path: {}", output.u8string());

    if (!utils::isSubDirectory(root, output))
    {
        // ".." などで root の外に出ている可能性がある.
        throw LibraryConfigError{ "StepDownloadImpl: output path is outside of the working directory." };
    }

    context.registRuntimeVariable("url", url);
    context.registRuntimeVariable("output", utils::toString(output.u8string()));

    // すでに output にファイルが存在する場合, キャッシュを検証してダウンロードをスキップする.
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

    // blob にファイルをキャッシュする.
    // -- blob path は 53文字の base32 文字列で表される SHA256 ハッシュ値をファイル名とする.
    // -- そのため, blob_dir / <2文字>/<2文字>/<2文字>/<のこり> というディレクトリ構造にする.
    const auto blob_dir       = context.getBuildRootDirectory() / "_blob";
    const auto blob_file_path = blob_dir / hash_b32.substr(0, 2) / hash_b32.substr(2, 2)  //
                              / hash_b32.substr(4, 2) / hash_b32.substr(6);
    logger.trace("Blob filepath: {}", blob_file_path.u8string());

    // blob にファイルがなければダウンロードする
    if (!std::filesystem::exists(blob_file_path))
    {
        std::filesystem::create_directories(blob_dir);

        const auto tmp_output = blob_dir / "data";
        std::filesystem::remove(tmp_output);

        // curl を使用して url からファイルをダウンロードする.
        std::vector<std::u8string> cmd = { u8"curl", u8"--fail", u8"--location" };

        // extra args
        cmd.insert(cmd.end(), this->curl_extra_args_.begin(), this->curl_extra_args_.end());

        // output
        cmd.emplace_back(u8"-o");
        cmd.emplace_back(tmp_output.u8string());

        // url
        cmd.emplace_back(utils::toU8String(url));

        logger.trace("Downloading...");
        const auto result = utils::run(cmd,
                                       {
                                           .print_stdout = logger.isVerbose(),
                                           .print_stderr = logger.isVerbose(),
                                       });
        if (result.exit_code != 0)
        {
            throw AppError{ "{}: curl exited with code {}.", this->getCmd(), result.exit_code };
        }

        // hash 検証
        logger.trace("hash calculating... Expected = {}", this->hash_);
        const auto actual = this->_calculateHash(tmp_output);
        logger.trace("hash calculated.    Actual   = {}", actual);
        AppError::check(actual == this->hash_, "{}: SHA-256 hash mismatch. URL = {}", this->getCmd(), url);

        // 検証後ファイルをリネームし, hash base のファイル名にする.
        // -- blob_file_path は存在しないはず
        std::filesystem::create_directories(blob_file_path.parent_path());
        std::filesystem::rename(tmp_output, blob_file_path);
    }

    // blob から output へコピーする
    std::filesystem::create_directories(output.parent_path());
    std::filesystem::copy(blob_file_path, output);

    logger.log("Download OK.");
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepDownloadImpl::clone() const
{
    return std::make_unique<StepDownloadImpl>(*this);
}

void
roah::distb::config::impl::StepDownloadImpl::loadFromJson(const nlohmann::json & json)
{
    this->_getStringFromJson(kCmd, json, "url", this->url_);
    this->_getStringFromJson(kCmd, json, "output", this->output_);
    this->_getStringFromJson(kCmd, json, "hash", this->hash_);
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
roah::distb::config::impl::StepDownloadImpl::getHashAsHexString() const noexcept
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
        throw AppError{ "{}: failed to open downloaded file: {}", this->getCmd(), this->output_ };
    }

    utils::SHA256Hash sha;
    std::vector<char> buf(65536, 0);
    while (ifs.read(buf.data(), static_cast<std::streamsize>(buf.size())) || ifs.gcount() > 0)
    {
        sha.addData(buf.data(), static_cast<std::size_t>(ifs.gcount()));
    }

    return sha.getHashAsHexString();
}
