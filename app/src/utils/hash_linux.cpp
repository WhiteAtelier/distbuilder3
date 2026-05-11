// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/hash.hpp>

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <openssl/evp.h>

namespace roah::distb::utils {

// ============================================================
// Impl クラス定義 (Unix/OpenSSL)
// ============================================================

class SHA256Hash::Impl {
public:
    Impl();
    Impl(const Impl&)            = delete;
    Impl(Impl&&)                 = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&)      = delete;
    ~Impl();

    void        addData(const void* data, std::size_t size);
    std::string getHash() const;

private:
    // OpenSSL の EVP ダイジェストコンテキスト.
    EVP_MD_CTX* ctx_ = nullptr;
};

// ============================================================
// Impl (Unix/OpenSSL) 実装
// ============================================================

roah::distb::utils::SHA256Hash::Impl::Impl()
{
    this->ctx_ = EVP_MD_CTX_new();
    if (!this->ctx_) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }
    if (EVP_DigestInit_ex(this->ctx_, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(this->ctx_);
        this->ctx_ = nullptr;
        throw std::runtime_error("EVP_DigestInit_ex failed");
    }
}

roah::distb::utils::SHA256Hash::Impl::~Impl()
{
    if (this->ctx_) {
        EVP_MD_CTX_free(this->ctx_);
    }
}

void roah::distb::utils::SHA256Hash::Impl::addData(const void* const data, const std::size_t size)
{
    if (EVP_DigestUpdate(this->ctx_, data, size) != 1) {
        throw std::runtime_error("EVP_DigestUpdate failed");
    }
}

std::string roah::distb::utils::SHA256Hash::Impl::getHash() const
{
    // 内部状態を変化させないために, コンテキストを複製してから Final する.
    EVP_MD_CTX* const dup_ctx = EVP_MD_CTX_new();
    if (!dup_ctx) {
        throw std::runtime_error("EVP_MD_CTX_new failed");
    }
    if (EVP_MD_CTX_copy_ex(dup_ctx, this->ctx_) != 1) {
        EVP_MD_CTX_free(dup_ctx);
        throw std::runtime_error("EVP_MD_CTX_copy_ex failed");
    }

    std::array<unsigned char, 32> hash_bytes {};
    unsigned int hash_len = 0;
    if (EVP_DigestFinal_ex(dup_ctx, hash_bytes.data(), &hash_len) != 1) {
        EVP_MD_CTX_free(dup_ctx);
        throw std::runtime_error("EVP_DigestFinal_ex failed");
    }
    EVP_MD_CTX_free(dup_ctx);

    // 16 進数文字列に変換する.
    std::ostringstream oss;
    for (unsigned int i = 0; i < hash_len; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash_bytes[i]);
    }
    return oss.str();
}

// ============================================================
// SHA256Hash 実装 (Impl に委譲)
// ============================================================

roah::distb::utils::SHA256Hash::SHA256Hash()
    : impl_ { std::make_unique<Impl>() }
{
}

roah::distb::utils::SHA256Hash::SHA256Hash(SHA256Hash&&) noexcept = default;

roah::distb::utils::SHA256Hash& roah::distb::utils::SHA256Hash::operator=(SHA256Hash&&) noexcept = default;

roah::distb::utils::SHA256Hash::~SHA256Hash() = default;

void roah::distb::utils::SHA256Hash::addData(const void* const data, const std::size_t size)
{
    this->impl_->addData(data, size);
}

std::string roah::distb::utils::SHA256Hash::getHash() const
{
    return this->impl_->getHash();
}

} // namespace roah::distb::utils
