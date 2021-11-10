#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#include "utilities.h"

class Logger
{
public:
    virtual ~Logger() noexcept;

    static const Logger &Instance() noexcept;

    template <typename Type>
    auto operator<<(const Type &rhs) const noexcept
    {
#ifdef DEBUG
        std::cout << rhs;
#else
        unused(rhs);
#endif // DEBUG

        return *this;
    }

private:
    Logger() noexcept;
};

#endif /* LOGGER_H */
