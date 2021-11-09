#include "logger.h"

Logger::Logger() noexcept {}

Logger::~Logger() noexcept {}

const Logger &Logger::Instance() noexcept
{
    static Logger logger;
    return logger;
}
