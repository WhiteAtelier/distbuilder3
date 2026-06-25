#include "roah/distb/logger.hpp"

#include <iostream>

roah::distb::Logger::Logger()
    : verbose_{ false }
{}

void
roah::distb::Logger::setVerbose(const bool verbose) noexcept
{
    this->verbose_ = verbose;
}

bool
roah::distb::Logger::isVerbose() const noexcept
{
    return this->verbose_;
}

void
roah::distb::Logger::_log(const std::string_view msg)
{
    std::cerr << msg << std::endl;
}

roah::distb::Logger roah::distb::logger;
