#ifndef RAM_H
#define RAM_H

#include <cassert>
#include <vector>

#include "memory_interface.h"

class RAM final : public MemoryInterface
{
public:
    RAM(std::size_t size) : data_(size, 0) {}

    virtual ~RAM() {}

    std::size_t GetSize() const noexcept override
    {
        return data_.size();
    }

    uint8_t Read(std::size_t index) const override
    {
        assert(index < data_.size());

        return data_[index];
    }

    void
    Write(std::size_t index, uint8_t data) override
    {
        assert(index < data_.size());

        data_[index] = data;
    }

    auto operator=(const RAM &) = delete;

    auto operator=(RAM &&) = delete;

private:
    std::vector<uint8_t> data_;
};

#endif /* RAM_H */
