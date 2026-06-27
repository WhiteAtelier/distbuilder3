#ifndef ROAH_DISTB_UTILS_IMPL_HASH_WIN32_HPP
#define ROAH_DISTB_UTILS_IMPL_HASH_WIN32_HPP

// windows.h を先にインクルードする必要あり
#include <windows.h>
//
#include <bcrypt.h>

#include <cstddef>
#include <string>
#include <vector>

namespace roah::distb::utils {

class HashGeneratorWin32Impl
{
public:
    enum struct HashAlgorithm
    {
        SHA256,
        MD5,
    };

    HashGeneratorWin32Impl(const HashAlgorithm hash_algorithm);
    ~HashGeneratorWin32Impl();

    HashGeneratorWin32Impl(const HashGeneratorWin32Impl &) = delete;
    HashGeneratorWin32Impl(HashGeneratorWin32Impl &&)      = delete;
    HashGeneratorWin32Impl &
    operator=(const HashGeneratorWin32Impl &)
        = delete;
    HashGeneratorWin32Impl &
    operator=(HashGeneratorWin32Impl &&)
        = delete;

    void
    addData(const void * data, std::size_t size);

    std::vector<std::byte>
    getHashBinary() const;

private:
    BCRYPT_ALG_HANDLE  alg_handle_  = nullptr;
    BCRYPT_HASH_HANDLE hash_handle_ = nullptr;
    // BCrypt がハッシュオブジェクトを保持するためのバッファ.
    std::vector<BYTE>  hash_object_;
};

}  // namespace roah::distb::utils

#endif
