#include "logger.hpp"

#include <iostream>

roah::distb::app::Logger::Logger()
    : verbose_{ false }
{}

void
roah::distb::app::Logger::setVerbose(const bool verbose) noexcept
{
    this->verbose_ = verbose;
}

void
roah::distb::app::Logger::_log(const std::string_view msg)
{
    std::cerr << msg << std::endl;
}

roah::distb::app::Logger roah::distb::app::logger;
