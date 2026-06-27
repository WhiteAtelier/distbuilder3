#ifndef ROAH_DISTB_UTILS_IMPL_HASH_LINUX_HPP
#define ROAH_DISTB_UTILS_IMPL_HASH_LINUX_HPP

#include <openssl/evp.h>

#include <cstddef>
#include <string>
#include <vector>

namespace roah::distb::utils {

class HashGeneratorLinuxImpl
{
public:
    enum struct HashAlgorithm
    {
        SHA256,
        MD5,
    };

    HashGeneratorLinuxImpl(const HashAlgorithm hash_algorithm);
    ~HashGeneratorLinuxImpl();

    HashGeneratorLinuxImpl(const HashGeneratorLinuxImpl &) = delete;
    HashGeneratorLinuxImpl(HashGeneratorLinuxImpl &&)      = delete;
    HashGeneratorLinuxImpl &
    operator=(const HashGeneratorLinuxImpl &)
        = delete;
    HashGeneratorLinuxImpl &
    operator=(HashGeneratorLinuxImpl &&)
        = delete;

    void
    addData(const void * data, std::size_t size);

    std::vector<std::byte>
    getHashBinary() const;

private:
    // OpenSSL の EVP ダイジェストコンテキスト.
    EVP_MD_CTX * ctx_ = nullptr;
};

}  // namespace roah::distb::utils

#endif
