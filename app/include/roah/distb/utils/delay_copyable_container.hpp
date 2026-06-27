#ifndef ROAH_DISTB_UTILS_DELAY_COPYABLE_CONTAINER_HPP
#define ROAH_DISTB_UTILS_DELAY_COPYABLE_CONTAINER_HPP

#include <memory>
#include <stdexcept>

namespace roah::distb::utils {

template <typename T>
class DelayCopyableContainer
{
public:
    DelayCopyableContainer();
    DelayCopyableContainer(std::unique_ptr<T> && instance) noexcept;
    DelayCopyableContainer(const DelayCopyableContainer &) noexcept;
    DelayCopyableContainer(DelayCopyableContainer &&) noexcept;
    DelayCopyableContainer<T> &
    operator=(const DelayCopyableContainer &) noexcept;
    DelayCopyableContainer<T> &
    operator=(DelayCopyableContainer &&) noexcept;
    ~DelayCopyableContainer() noexcept;

    explicit
    operator bool() const noexcept;

    bool
    operator!() const noexcept;

    void
    reset() noexcept;

    const T &
    ref() const;

    const T &
    operator*() const;

    const T *
    operator->() const;

private:
    DelayCopyableContainer(const T * ptr) noexcept;

    std::unique_ptr<T> instance_;
    const T *          ptr_;
};

}  // namespace roah::distb::utils

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DelayCopyableContainer class implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////
template <typename T>
roah::distb::utils::DelayCopyableContainer<T>::DelayCopyableContainer()
    : instance_{ nullptr }
    , ptr_{ nullptr }
{}

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::DelayCopyableContainer(std::unique_ptr<T> && instance) noexcept
    : instance_{ std::move(instance) }
    , ptr_{ this->instance_.get() }
{}

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::DelayCopyableContainer(const T * ptr) noexcept
    : ptr_{ ptr }
{}

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::DelayCopyableContainer(
    const DelayCopyableContainer & rhs) noexcept
    : instance_{ nullptr }
    , ptr_{ rhs.ptr_ }
{}

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::DelayCopyableContainer(DelayCopyableContainer &&) noexcept
    = default;

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T> &
roah::distb::utils::DelayCopyableContainer<T>::operator=(const DelayCopyableContainer & rhs) noexcept
{
    if (this != &rhs)
    {
        this->reset();
        this->ptr_ = rhs.ptr_;
    }
    return *this;
}

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T> &
roah::distb::utils::DelayCopyableContainer<T>::operator=(DelayCopyableContainer &&) noexcept
    = default;

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::~DelayCopyableContainer() noexcept = default;

template <typename T>
inline roah::distb::utils::DelayCopyableContainer<T>::operator bool() const noexcept
{
    return this->ptr_ != nullptr;
}

template <typename T>
inline bool
roah::distb::utils::DelayCopyableContainer<T>::operator!() const noexcept
{
    return this->ptr_ == nullptr;
}

template <typename T>
inline void
roah::distb::utils::DelayCopyableContainer<T>::reset() noexcept
{
    this->instance_.reset();
    this->ptr_ = nullptr;
}

template <typename T>
inline const T &
roah::distb::utils::DelayCopyableContainer<T>::ref() const
{
    return **this;
}

template <typename T>
inline const T &
roah::distb::utils::DelayCopyableContainer<T>::operator*() const
{
    if (this->ptr_ == nullptr)
    {
        throw std::runtime_error{ "DelayCopyableContainer: null access." };
    }
    return *this->ptr_;
}

template <typename T>
inline const T *
roah::distb::utils::DelayCopyableContainer<T>::operator->() const
{
    return &**this;
}

#endif
