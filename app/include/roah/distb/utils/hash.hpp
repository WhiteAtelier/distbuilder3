// This file contains code generated with the assistance of Claude (Anthropic), an AI assistant.
// The generated code is provided as-is.

#ifndef ROAH_DISTB_UTILS_HASH_HPP
#define ROAH_DISTB_UTILS_HASH_HPP

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace roah::distb::utils {

namespace detail {
class HashGeneratorBase
{
public:
    HashGeneratorBase(HashGeneratorBase &&) noexcept;
    virtual ~HashGeneratorBase() noexcept;

    HashGeneratorBase(const HashGeneratorBase &) = delete;
    HashGeneratorBase &
    operator=(const HashGeneratorBase &)
        = delete;
    HashGeneratorBase &
    operator=(HashGeneratorBase &&)
        = delete;

    // データを追加してハッシュを更新する.
    void
    addData(const void * const data, const std::size_t size);

    std::vector<std::byte>
    getHashBinary() const;

    // 現在のハッシュ値を 16 進数文字列として返す.
    // この呼び出しによって内部状態は変化しない.
    std::string
    getHashAsHexString() const;

protected:
    class Generator;

    HashGeneratorBase(std::unique_ptr<Generator> && generator);

    std::unique_ptr<Generator> generator_;
};
}  // namespace detail

// SHA-256 ハッシュを計算するクラス.
// addData() を複数回呼び出してデータを追加し, getHash() でハッシュ値を取得する.
// getHash() を呼び出しても内部状態は変化しないため, 呼び出し後も addData() を継続できる.
class SHA256Hash final : public detail::HashGeneratorBase
{
public:
    SHA256Hash();
    SHA256Hash(SHA256Hash &&) noexcept;
    ~SHA256Hash() noexcept override;

private:
    class GeneratorImpl;
};

/// @brief MD5 ハッシュを計算するクラス.
class MD5Hash final : public detail::HashGeneratorBase
{
public:
    MD5Hash();
    MD5Hash(MD5Hash &&) noexcept;
    ~MD5Hash() noexcept override;

private:
    class GeneratorImpl;
};

}  // namespace roah::distb::utils

#endif  // ROAH_DISTB_UTILS_HASH_HPP
