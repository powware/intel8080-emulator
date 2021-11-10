#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H

#include <cstdint>

class MemoryInterface
{
public:
    virtual ~MemoryInterface() {}

    virtual std::size_t size() const noexcept = 0;

    virtual uint8_t Read(std::size_t index) const = 0;

    virtual void Write(std::size_t index, uint8_t data) = 0;
};

#endif /* MEMORY_INTERFACE_H */
