#ifndef RAM_H
#define RAM_H

#include <vector>

#include "memory_interface.h"

class RAM final : public MemoryInterface
{
public:
    RAM(std::size_t size) : data_(size, 0) {}

    virtual ~RAM() {}

    std::size_t GetSize() const override
    {
        return data_.size();
    }

    bool Read(std::size_t index, uint8_t &data) const override
    {
        data = data_[index];

        return true;
    }

    bool Write(std::size_t index, uint8_t data) override
    {
        data_[index] = data;

        return true;
    }

    auto operator=(const RAM &) = delete;

    auto operator=(RAM &&) = delete;

private:
    std::vector<uint8_t> data_;
};

#endif /* RAM_H */
