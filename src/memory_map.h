#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <vector>
#include <tuple>

#include "memory_interface.h"

class MemoryMap final
{

public:
    template <class... Memory>
    MemoryMap(Memory &&...memories)
    {
        (CreateMapping(std::forward<Memory>(memories)), ...);
    }

    std::size_t GetSize() const
    {
        return size_;
    }

    bool Read(uint16_t address, uint8_t &data) const
    {
        std::size_t index;
        if (!GetMappingIndex(address, index))
        {
            return false;
        }

        return std::get<1>(mappings_[index])->Read(address - std::get<0>(mappings_[index]), data);
    }

    bool Write(uint16_t address, uint8_t data) const
    {
        std::size_t index;
        if (!GetMappingIndex(address, index))
        {
            return false;
        }

        return std::get<1>(mappings_[index])->Write(address - std::get<0>(mappings_[index]), data);
    }

private:
    std::size_t size_{0};
    std::vector<std::tuple<uint16_t, std::unique_ptr<MemoryInterface>>> mappings_;

    template <class Memory>
    void CreateMapping(Memory &&memory)
    {
        const uint16_t address = mappings_.size() ? static_cast<uint16_t>(std::get<0>(mappings_.back()) + std::get<1>(mappings_.back())->GetSize()) : 0;
        mappings_.emplace_back(std::make_tuple(address, std::make_unique<Memory>(std::forward<Memory>(memory))));
        size_ += std::get<1>(mappings_.back())->GetSize();
    }

    bool GetMappingIndex(uint16_t address, std::size_t &index) const
    {
        for (std::size_t i = 0; i < mappings_.size(); i++)
        {
            if (std::get<0>(mappings_[i]) <= address && std::get<0>(mappings_[i]) + std::get<1>(mappings_[i])->GetSize() > address)
            {
                index = i;
                return true;
            }
        }

        return false;
    }
};

#endif /* MEMORY_MAP_H */
