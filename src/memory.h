#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <tuple>
#include <limits>

#include "ram.h"
#include "rom.h"

class Memory final
{
public:
    Memory();

    virtual ~Memory();

    uint8_t Read(uint16_t address) const;

    void Write(uint16_t address, uint8_t data);

    template <class MemoryType, typename... Args>
    void AddMemory(Args &&...args)
    {
        const auto address = mapping_.size() ? std::get<0>(mapping_.back()) + std::get<1>(mapping_.back())->GetSize() : 0;
        if (address > std::numeric_limits<uint16_t>::max())
        {
            throw std::runtime_error("Trying to add memory which would not be addressable.");
        }

        mapping_.emplace_back(std::make_tuple(static_cast<uint16_t>(address), std::make_unique<MemoryType>(std::forward<Args>(args)...)));
    }

private:
    std::vector<std::tuple<uint16_t, std::unique_ptr<MemoryInterface>>> mapping_;

    auto GetMappedMemory(uint16_t address) const;
};

#endif /* MEMORY_H */
