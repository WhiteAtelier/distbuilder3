// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#include "hash_win32.hpp"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

// 無名名前空間: 内部ヘルパー.
namespace {

// BCrypt の NTSTATUS をチェックし, 失敗時は例外を投げる.
void
_checkBcrypt(const NTSTATUS status, const char * const msg)
{
    if (!BCRYPT_SUCCESS(status))
    {
        throw std::runtime_error{ msg };
    }
}

}  // namespace

roah::distb::utils::HashGeneratorWin32Impl::HashGeneratorWin32Impl(const HashAlgorithm hash_algorithm)
{
    LPCWSTR alg_id = nullptr;
    switch (hash_algorithm)
    {
    case HashAlgorithm::SHA256: alg_id = BCRYPT_SHA256_ALGORITHM; break;
    case HashAlgorithm::MD5: alg_id = BCRYPT_MD5_ALGORITHM; break;
    default: throw std::invalid_argument{ "Unsupported hash algorithm" };
    }

    _checkBcrypt(BCryptOpenAlgorithmProvider(&this->alg_handle_, alg_id, nullptr, 0),
                 "BCryptOpenAlgorithmProvider failed");

    // ハッシュオブジェクトのサイズを取得する.
    DWORD obj_size     = 0;
    DWORD bytes_copied = 0;
    _checkBcrypt(BCryptGetProperty(this->alg_handle_,
                                   BCRYPT_OBJECT_LENGTH,
                                   reinterpret_cast<PUCHAR>(&obj_size),
                                   sizeof(DWORD),
                                   &bytes_copied,
                                   0),
                 "BCryptGetProperty(BCRYPT_OBJECT_LENGTH) failed");

    this->hash_object_.resize(obj_size);
    _checkBcrypt(
        BCryptCreateHash(this->alg_handle_, &this->hash_handle_, this->hash_object_.data(), obj_size, nullptr, 0, 0),
        "BCryptCreateHash failed");
}

roah::distb::utils::HashGeneratorWin32Impl::~HashGeneratorWin32Impl()
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
roah::distb::utils::HashGeneratorWin32Impl::addData(const void * const data, const std::size_t size)
{
    // BCryptHashData は PUCHAR (非 const) を要求するため, 書き込みが発生しないとはいえ
    // const_cast を使うと意図しない変更を許してしまう恐れがある.
    // そのため, 別バッファにコピーしてから渡す.
    std::vector<BYTE> buf(static_cast<const BYTE *>(data), static_cast<const BYTE *>(data) + size);
    _checkBcrypt(BCryptHashData(this->hash_handle_, buf.data(), static_cast<ULONG>(size), 0), "BCryptHashData failed");
}

std::vector<std::byte>
roah::distb::utils::HashGeneratorWin32Impl::getHashBinary() const
{
    // 内部状態を変化させないために, ハッシュを複製してから Finish する.
    std::vector<BYTE>  dup_obj(this->hash_object_.size());
    BCRYPT_HASH_HANDLE dup_handle = nullptr;
    _checkBcrypt(
        BCryptDuplicateHash(this->hash_handle_, &dup_handle, dup_obj.data(), static_cast<ULONG>(dup_obj.size()), 0),
        "BCryptDuplicateHash failed");

    // ハッシュ長を取得する.
    DWORD hash_len     = 0;
    DWORD bytes_copied = 0;
    _checkBcrypt(BCryptGetProperty(this->alg_handle_,
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
    _checkBcrypt(finish_status, "BCryptFinishHash failed");
    return hash_bytes;
}
