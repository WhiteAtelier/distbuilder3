// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "hash_linux.hpp"

#include "roah/distb/utils/hash.hpp"

#include <openssl/evp.h>

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>

roah::distb::utils::HashGeneratorLinuxImpl::HashGeneratorLinuxImpl(const HashAlgorithm hash_algorithm)
    : ctx_{ EVP_MD_CTX_new() }
{
    if (!this->ctx_)
    {
        throw std::runtime_error{ "EVP_MD_CTX_new failed" };
    }

    const EVP_MD * md = nullptr;
    switch (hash_algorithm)
    {
    case HashAlgorithm::SHA256: md = EVP_sha256(); break;
    case HashAlgorithm::MD5: md = EVP_md5(); break;
    default: throw std::invalid_argument{ "Unsupported hash algorithm" };
    }

    if (EVP_DigestInit_ex(this->ctx_, md, nullptr) != 1)
    {
        EVP_MD_CTX_free(this->ctx_);
        this->ctx_ = nullptr;
        throw std::runtime_error{ "EVP_DigestInit_ex failed" };
    }
}

roah::distb::utils::HashGeneratorLinuxImpl::~HashGeneratorLinuxImpl()
{
    if (this->ctx_)
    {
        EVP_MD_CTX_free(this->ctx_);
    }
}

void
roah::distb::utils::HashGeneratorLinuxImpl::addData(const void * const data, const std::size_t size)
{
    if (EVP_DigestUpdate(this->ctx_, data, size) != 1)
    {
        throw std::runtime_error{ "EVP_DigestUpdate failed" };
    }
}

std::vector<std::byte>
roah::distb::utils::HashGeneratorLinuxImpl::getHashBinary() const
{
    // 内部状態を変化させないために, コンテキストを複製してから Final する.
    EVP_MD_CTX * const dup_ctx = EVP_MD_CTX_new();
    if (!dup_ctx)
    {
        throw std::runtime_error{ "EVP_MD_CTX_new failed" };
    }
    if (EVP_MD_CTX_copy_ex(dup_ctx, this->ctx_) != 1)
    {
        EVP_MD_CTX_free(dup_ctx);
        throw std::runtime_error{ "EVP_MD_CTX_copy_ex failed" };
    }

    std::vector<std::byte> hash_bytes(EVP_MAX_MD_SIZE);
    unsigned int           hash_len = 0;
    if (EVP_DigestFinal_ex(dup_ctx, hash_bytes.data(), &hash_len) != 1)
    {
        EVP_MD_CTX_free(dup_ctx);
        throw std::runtime_error{ "EVP_DigestFinal_ex failed" };
    }
    EVP_MD_CTX_free(dup_ctx);

    hash_bytes.resize(hash_len);
    return hash_bytes;
}
