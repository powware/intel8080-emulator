#ifndef RAM_H
#define RAM_H

#include <cstdint>
#include <vector>

#include "memory_interface.h"

class RAM final : public MemoryInterface
{
public:
    RAM(std::size_t size);

    RAM(const RAM &) = delete;

    RAM(RAM &&) = delete;

    virtual ~RAM();

    auto operator=(const RAM &) = delete;

    auto operator=(RAM &&) = delete;

    std::size_t size() const noexcept override;

    uint8_t Read(std::size_t index) const noexcept override;

    void Write(std::size_t index, uint8_t data) noexcept override;

private:
    std::vector<uint8_t> data_;
};

#endif /* RAM_H */
