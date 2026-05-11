// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "step_wget_impl.hpp"

#include "roah/distb/utils/hash.hpp"
#include "roah/distb/utils/subprocess.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <vector>

roah::distb::config::impl::StepWgetImpl::StepWgetImpl(std::string url, std::string output, std::string hash)
    : StepDef{ "wget" }
    , url_{ std::move(url) }
    , output_{ std::move(output) }
    , hash_{ std::move(hash) }
{}

void
roah::distb::config::impl::StepWgetImpl::operator()() const
{
    if (this->url_.empty())
    {
        throw std::runtime_error{ "StepWgetImpl: url is not set." };
    }

    // curl を使用して url からファイルをダウンロードする.
    // URL 内のテンプレート変数 (${version} など) の展開は呼び出し元が行う想定.
    std::vector<std::u8string> cmd = { u8"curl", u8"-L", u8"--fail" };

    if (!this->output_.empty())
    {
        cmd.push_back(u8"-o");
        cmd.emplace_back(this->output_.begin(), this->output_.end());
    }

    cmd.emplace_back(this->url_.begin(), this->url_.end());

    const auto result = utils::run(cmd);
    if (result.exit_code != 0)
    {
        throw std::runtime_error{ "StepWgetImpl: curl exited with code " + std::to_string(result.exit_code)
                                  + "." };
    }

    // hash_ が設定されている場合, ダウンロードしたファイルの SHA-256 を検証する.
    if (!this->hash_.empty())
    {
        if (this->output_.empty())
        {
            throw std::runtime_error{ "StepWgetImpl: hash is specified but output is not set." };
        }

        // ファイルをバイナリで読み込む.
        std::ifstream ifs{ this->output_, std::ios::binary };
        if (!ifs)
        {
            throw std::runtime_error{ "StepWgetImpl: failed to open downloaded file: " + this->output_ };
        }

        utils::SHA256Hash sha;
        std::vector<char> buf(65536);
        while (ifs.read(buf.data(), static_cast<std::streamsize>(buf.size())) || ifs.gcount() > 0)
        {
            sha.addData(buf.data(), static_cast<std::size_t>(ifs.gcount()));
        }

        const auto actual = sha.getHash();
        if (actual != this->hash_)
        {
            throw std::runtime_error{
                "StepWgetImpl: SHA-256 hash mismatch.\n"
                "  expected: " + this->hash_ + "\n"
                "  actual:   " + actual
            };
        }
    }
}

std::unique_ptr<roah::distb::config::StepDef>
roah::distb::config::impl::StepWgetImpl::clone() const
{
    return std::make_unique<StepWgetImpl>(*this);
}

std::unique_ptr<roah::distb::config::impl::StepWgetImpl>
roah::distb::config::impl::StepWgetImpl::loadFromJson(const nlohmann::json & json)
{
    if (!json.is_object() || !json.contains("url") || !json["url"].is_string())
    {
        return nullptr;
    }

    auto url = json["url"].get<std::string>();

    std::string output;
    if (json.contains("output") && json["output"].is_string())
    {
        output = json["output"].get<std::string>();
    }

    std::string hash;
    if (json.contains("hash") && json["hash"].is_string())
    {
        hash = json["hash"].get<std::string>();
    }

    return std::make_unique<StepWgetImpl>(std::move(url), std::move(output), std::move(hash));
}

const std::string &
roah::distb::config::impl::StepWgetImpl::getUrl() const noexcept
{
    return this->url_;
}

const std::string &
roah::distb::config::impl::StepWgetImpl::getOutput() const noexcept
{
    return this->output_;
}

const std::string &
roah::distb::config::impl::StepWgetImpl::getHash() const noexcept
{
    return this->hash_;
}
