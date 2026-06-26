// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include <roah/distb/utils/hash.hpp>

// windows.h を先にインクルードする必要あり
#include <windows.h>
//
#include <bcrypt.h>

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

// 無名名前空間: 内部ヘルパー.
namespace {

// BCrypt の NTSTATUS をチェックし, 失敗時は例外を投げる.
void
checkBcrypt(const NTSTATUS status, const char * const msg)
{
    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error(msg);
    }
}

}  // namespace

namespace roah::distb::utils {

// ============================================================
// Impl クラス定義 (Windows BCrypt)
// ============================================================

class SHA256Hash::Impl
{
public:
    Impl();
    Impl(const Impl &) = delete;
    Impl(Impl &&)      = delete;
    Impl &
    operator=(const Impl &)
        = delete;
    Impl &
    operator=(Impl &&)
        = delete;
    ~Impl();

    void
    addData(const void * data, std::size_t size);

    std::vector<std::byte>
    getHashBinary() const;

    std::string
    getHashAsHexString() const;

private:
    BCRYPT_ALG_HANDLE  alg_handle_  = nullptr;
    BCRYPT_HASH_HANDLE hash_handle_ = nullptr;
    // BCrypt がハッシュオブジェクトを保持するためのバッファ.
    std::vector<BYTE>  hash_object_;
};

}  // namespace roah::distb::utils

// ============================================================
// Impl (Windows) 実装
// ============================================================

roah::distb::utils::SHA256Hash::Impl::Impl()
{
    checkBcrypt(BCryptOpenAlgorithmProvider(&this->alg_handle_, BCRYPT_SHA256_ALGORITHM, nullptr, 0),
                "BCryptOpenAlgorithmProvider failed");

    // ハッシュオブジェクトのサイズを取得する.
    DWORD obj_size     = 0;
    DWORD bytes_copied = 0;
    checkBcrypt(BCryptGetProperty(this->alg_handle_,
                                  BCRYPT_OBJECT_LENGTH,
                                  reinterpret_cast<PUCHAR>(&obj_size),
                                  sizeof(DWORD),
                                  &bytes_copied,
                                  0),
                "BCryptGetProperty(BCRYPT_OBJECT_LENGTH) failed");

    this->hash_object_.resize(obj_size);
    checkBcrypt(
        BCryptCreateHash(this->alg_handle_, &this->hash_handle_, this->hash_object_.data(), obj_size, nullptr, 0, 0),
        "BCryptCreateHash failed");
}

roah::distb::utils::SHA256Hash::Impl::~Impl()
{
    if (this->hash_handle_)
    {
        BCryptDestroyHash(this->hash_handle_);
    }
    if (this->alg_handle_)
    {
        BCryptCloseAlgorithmProvider(this->alg_handle_, 0);
    }
}

void
roah::distb::utils::SHA256Hash::Impl::addData(const void * const data, const std::size_t size)
{
    // BCryptHashData は PUCHAR (非 const) を要求するため, 書き込みが発生しないとはいえ
    // const_cast を使うと意図しない変更を許してしまう恐れがある.
    // そのため, 別バッファにコピーしてから渡す.
    std::vector<BYTE> buf(static_cast<const BYTE *>(data), static_cast<const BYTE *>(data) + size);
    checkBcrypt(BCryptHashData(this->hash_handle_, buf.data(), static_cast<ULONG>(size), 0), "BCryptHashData failed");
}

std::vector<std::byte>
roah::distb::utils::SHA256Hash::Impl::getHashBinary() const
{
    // 内部状態を変化させないために, ハッシュを複製してから Finish する.
    std::vector<BYTE>  dup_obj(this->hash_object_.size());
    BCRYPT_HASH_HANDLE dup_handle = nullptr;
    checkBcrypt(
        BCryptDuplicateHash(this->hash_handle_, &dup_handle, dup_obj.data(), static_cast<ULONG>(dup_obj.size()), 0),
        "BCryptDuplicateHash failed");

    // ハッシュ長を取得する.
    DWORD hash_len     = 0;
    DWORD bytes_copied = 0;
    checkBcrypt(BCryptGetProperty(this->alg_handle_,
                                  BCRYPT_HASH_LENGTH,
                                  reinterpret_cast<PUCHAR>(&hash_len),
                                  sizeof(DWORD),
                                  &bytes_copied,
                                  0),
                "BCryptGetProperty(BCRYPT_HASH_LENGTH) failed");

    std::vector<std::byte> hash_bytes(static_cast<std::size_t>(hash_len));
    const NTSTATUS         finish_status = BCryptFinishHash(dup_handle,  //
                                                    reinterpret_cast<BYTE *>(hash_bytes.data()),
                                                    hash_len,
                                                    0);
    BCryptDestroyHash(dup_handle);
    checkBcrypt(finish_status, "BCryptFinishHash failed");
    return hash_bytes;
}

std::string
roah::distb::utils::SHA256Hash::Impl::getHashAsHexString() const
{
    const auto hash_bytes = this->getHashBinary();

    // 16 進数文字列に変換する.
    std::ostringstream oss;
    for (const auto b : hash_bytes)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(b);
    }
    return oss.str();
}

// ============================================================
// SHA256Hash 実装 (Impl に委譲)
// ============================================================

roah::distb::utils::SHA256Hash::SHA256Hash()
    : impl_{ std::make_unique<Impl>() }
{}

roah::distb::utils::SHA256Hash::SHA256Hash(SHA256Hash &&) noexcept = default;

roah::distb::utils::SHA256Hash &
roah::distb::utils::SHA256Hash::operator=(SHA256Hash &&) noexcept
    = default;

roah::distb::utils::SHA256Hash::~SHA256Hash() = default;

void
roah::distb::utils::SHA256Hash::addData(const void * const data, const std::size_t size)
{
    this->impl_->addData(data, size);
}

std::vector<std::byte>
roah::distb::utils::SHA256Hash::getHashBinary() const
{
    return this->impl_->getHashBinary();
}

std::string
roah::distb::utils::SHA256Hash::getHashAsHexString() const
{
    return this->impl_->getHashAsHexString();
}
