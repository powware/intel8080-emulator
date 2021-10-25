#include "memory.h"

Memory::Memory() {}

Memory::~Memory() {}

std::tuple<uint16_t, const MemoryInterface &> Memory::GetMappedMemory(uint16_t address) const
{
    for (auto &[start_address, memory] : mapping_)
    {
        if (address >= start_address && address < start_address + memory->size())
        {
            return std::make_tuple(static_cast<uint16_t>(address - start_address), std::cref(*memory.get()));
        }
    }

    throw std::runtime_error("Memory::GetMappedMemory(): Address outside of addressable memory range.");
}

std::tuple<uint16_t, MemoryInterface &> Memory::GetMappedMemory(uint16_t address)
{
    auto [index, memory] = static_cast<const Memory *>(this)->GetMappedMemory(address);
    return std::make_tuple(index, std::ref(const_cast<MemoryInterface &>(memory)));
}

uint8_t Memory::Read(uint16_t address) const
{
    auto [index, memory] = GetMappedMemory(address);

    return memory.Read(index);
}

void Memory::Write(uint16_t address, uint8_t data)
{
    auto [index, memory] = GetMappedMemory(address);

    return memory.Write(index, data);
}
