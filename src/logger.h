#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#include "register.h"

class Logger
{
public:
    template <typename... Args>
    inline static void Log(std::string_view format_string, Args &&...args) noexcept
    {
#ifdef DEBUG
        unused(format_string, std::forward<Args>(args)...);
#else
        unused(format_string, std::forward<Args>(args)...);
#endif // DEBUG
    }
};

#endif /* LOGGER_H */
