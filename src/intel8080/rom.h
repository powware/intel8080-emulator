#ifndef ROM_H
#define ROM_H

#include <filesystem>
#include <vector>
#include <cstdint>

#include "memory_interface.h"

class ROM final : public MemoryInterface
{
public:
    ROM(const std::filesystem::path &file_path);

    ROM(const ROM &) = delete;

    ROM(ROM &&) = delete;

    virtual ~ROM();

    auto operator=(const ROM &) = delete;

    auto operator=(ROM &&) = delete;

    std::size_t size() const noexcept override;

    uint8_t Read(std::size_t index) const noexcept override;

    void Write(std::size_t index, uint8_t data) override;

private:
    std::vector<uint8_t> data_;
};

#endif /* ROM_H */
