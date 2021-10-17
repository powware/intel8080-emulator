#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <tuple>
#include <limits>
#include <memory>

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
        const auto new_address = [&]() -> std::size_t
        {
            if (mapping_.size() == 0)
            {
                return 0;
            }

            const auto &[address, memory] = mapping_.back();

            return address + memory->size();
        }();

        constexpr uint16_t max_address = std::numeric_limits<uint16_t>::max();
        if (new_address > max_address)
        {
            throw std::runtime_error("Memory::AddMemory(): Attempting to create memory outside of addressable range.");
        }

        mapping_.emplace_back(static_cast<uint16_t>(new_address), std::make_unique<MemoryType>(std::forward<Args>(args)...));
    }

private:
    std::vector<std::tuple<uint16_t, std::unique_ptr<MemoryInterface>>> mapping_;

    std::tuple<uint16_t, MemoryInterface *> GetMappedMemory(uint16_t address) const;
};

#endif /* MEMORY_H */
