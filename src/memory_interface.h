#ifndef MEMORY_INTERFACE_H
#define MEMORY_INTERFACE_H

class MemoryInterface
{
public:
    virtual ~MemoryInterface() {}

    virtual std::size_t GetSize() const = 0;

    virtual bool Read(std::size_t index, uint8_t &data) const = 0;

    virtual bool Write(std::size_t index, uint8_t data) = 0;
};

#endif /* MEMORY_INTERFACE_H */
