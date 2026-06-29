#ifndef ROAH_DISTB_APP_ACCESS_TOKEN_HPP
#define ROAH_DISTB_APP_ACCESS_TOKEN_HPP

#include <string>
#include <unordered_map>

namespace roah::distb::app {

class AccessToken
{
public:
    AccessToken(std::string name);

    const std::string &
    getKey() const noexcept;

    void
    addSecret(std::string key, std::string secret);

    void
    addSecret(std::string secret);

    const std::string &
    getSecret(const std::string & key) const noexcept;

private:
    std::string                                  name_;
    std::unordered_map<std::string, std::string> secrets_;
};

}  // namespace roah::distb::app

#endif
