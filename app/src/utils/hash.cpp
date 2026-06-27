#include "roah/distb/utils/hash.hpp"

#ifdef ROAH_ARCH_WIN32
#    include "impl/hash_win32.hpp"
#elif ROAH_ARCH_LINUX
#    include "impl/hash_linux.hpp"
#else
#    error "Unsupported platform"
#endif

#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <utility>

namespace roah::distb::utils::detail {

class HashGeneratorBase::Generator
{
protected:
    Generator() = default;

public:
    virtual ~Generator() noexcept = default;

    virtual void
    addData(const void * const data, const std::size_t size)
        = 0;

    virtual std::vector<std::byte>
    getHashBinary() const = 0;
};

}  // namespace roah::distb::utils::detail

roah::distb::utils::detail::HashGeneratorBase::HashGeneratorBase(std::unique_ptr<Generator> && generator)
    : generator_{ std::move(generator) }
{}

roah::distb::utils::detail::HashGeneratorBase::HashGeneratorBase(HashGeneratorBase &&) noexcept = default;

roah::distb::utils::detail::HashGeneratorBase::~HashGeneratorBase() noexcept = default;

void
roah::distb::utils::detail::HashGeneratorBase::addData(const void * const data, const std::size_t size)
{
    this->generator_->addData(data, size);
}

std::vector<std::byte>
roah::distb::utils::detail::HashGeneratorBase::getHashBinary() const
{
    return this->generator_->getHashBinary();
}

std::string
roah::distb::utils::detail::HashGeneratorBase::getHashAsHexString() const
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

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// SHA256Hash class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace roah::distb::utils {
class SHA256Hash::GeneratorImpl final : public detail::HashGeneratorBase::Generator
{
public:
    GeneratorImpl()                    = default;
    ~GeneratorImpl() noexcept override = default;

    void
    addData(const void * const data, const std::size_t size) override
    {
        this->impl_.addData(data, size);
    }

    std::vector<std::byte>
    getHashBinary() const override
    {
        return this->impl_.getHashBinary();
    }

private:
#ifdef ROAH_ARCH_WIN32
    HashGeneratorWin32Impl impl_{ HashGeneratorWin32Impl::HashAlgorithm::SHA256 };
#else ROAH_ARCH_LINUX
    HashGeneratorLinuxImpl impl_;
#endif
};
}  // namespace roah::distb::utils

roah::distb::utils::SHA256Hash::SHA256Hash()
    : HashGeneratorBase{ std::make_unique<GeneratorImpl>() }
{}

roah::distb::utils::SHA256Hash::SHA256Hash(SHA256Hash &&) noexcept = default;

roah::distb::utils::SHA256Hash::~SHA256Hash() = default;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// MD5Hash class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace roah::distb::utils {
class MD5Hash::GeneratorImpl final : public detail::HashGeneratorBase::Generator
{
public:
    GeneratorImpl() = default;

    void
    addData(const void * const data, const std::size_t size) override
    {
        this->impl_.addData(data, size);
    }

    std::vector<std::byte>
    getHashBinary() const override
    {
        return this->impl_.getHashBinary();
    }

private:
#ifdef ROAH_ARCH_WIN32
    HashGeneratorWin32Impl impl_{ HashGeneratorWin32Impl::HashAlgorithm::MD5 };
#else ROAH_ARCH_LINUX
    HashGeneratorLinuxImpl impl_;
#endif
};
}  // namespace roah::distb::utils

roah::distb::utils::MD5Hash::MD5Hash()
    : HashGeneratorBase{ std::make_unique<GeneratorImpl>() }
{}

roah::distb::utils::MD5Hash::MD5Hash(MD5Hash &&) noexcept = default;

roah::distb::utils::MD5Hash::~MD5Hash() = default;
