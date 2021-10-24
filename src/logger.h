#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <format>

#include "register.h"

class Logger
{
public:
    template <typename... Args>
    inline static void Log(std::string_view format_string, Args &&...args) noexcept
    {
#ifdef DEBUG
        std::cout << std::vformat(format_string, std::make_format_args(std::forward<Args>(args)...));
#else
        unused(format_string, std::forward<Args>(args)...);
#endif // DEBUG
    }
};

#endif /* LOGGER_H */
