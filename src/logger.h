#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

#undef DEBUG

class Logger
{
public:
    virtual ~Logger() {}

    static Logger &Instance()
    {
        static Logger logger;
        return logger;
    }

    template <typename Type>
    auto operator<<(const Type &rhs)
    {
#ifdef DEBUG
        std::cout << rhs;
#else
        unused(rhs);
#endif // DEBUG

        return *this;
    }

private:
    Logger() {}
};

#endif /* LOGGER_H */
