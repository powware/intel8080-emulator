#include "memory.h"

Memory::Memory() {}

Memory::~Memory() {}

auto Memory::GetMappedMemory(uint16_t address) const
{
    for (auto &[start_address, memory] : mapping_)
    {
        if (address >= start_address && address < start_address + memory->GetSize())
        {
            return std::make_tuple(address - start_address, memory.get());
        }
    }

    throw std::runtime_error("Memory::GetMappedMemory(): Address outside of addressable memory range.");
}

uint8_t Memory::Read(uint16_t address) const
{
    auto [relative_address, memory] = GetMappedMemory(address);

    return memory->Read(relative_address);
}

void Memory::Write(uint16_t address, uint8_t data)
{
    auto [relative_address, memory] = GetMappedMemory(address);

    return memory->Write(relative_address, data);
}
