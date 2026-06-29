#include "access_token.hpp"

#include "roah/distb/errors.hpp"

roah::distb::app::AccessToken::AccessToken(std::string name)
    : name_{ std::move(name) }
{}

const std::string &
roah::distb::app::AccessToken::getKey() const noexcept
{
    return this->name_;
}

void
roah::distb::app::AccessToken::addSecret(std::string key, std::string secret)
{
    this->secrets_[std::move(key)] = std::move(secret);
}

void
roah::distb::app::AccessToken::addSecret(std::string secret)
{
    this->secrets_["default"] = std::move(secret);
}

const std::string &
roah::distb::app::AccessToken::getSecret(const std::string & key) const noexcept
{
    if (const auto iter = this->secrets_.find(key); iter != this->secrets_.end())
    {
        return iter->second;
    }
    static const std::string empty_string;
    return empty_string;
}
