#ifndef ROM_H
#define ROM_H

#include <filesystem>
#include <fstream>

#include "memory_interface.h"
#include "utilities.h"

class ROM final : public MemoryInterface
{
public:
    ROM(const std::filesystem::path &file_path)
    {
        const auto file_size = std::filesystem::file_size(file_path);
        data_.resize(file_size);

        std::ifstream file_stream(file_path, std::ios::binary);
        file_stream.read(reinterpret_cast<char *>(data_.data()), file_size);
    }

    ROM(const ROM &) = delete;

    ROM(ROM &&) = delete;

    virtual ~ROM(){};

    auto operator=(const ROM &) = delete;

    auto operator=(ROM &&) = delete;

    std::size_t GetSize() const noexcept override
    {
        return data_.size();
    }

    uint8_t Read(std::size_t index) const noexcept override
    {
        assert(index < data_.size());

        return data_[index];
    }

    void Write(std::size_t index, uint8_t data) override
    {
        unused(index, data);

        throw std::runtime_error("ROM::Write(): Attempting to write read only memory.");
    }

private:
    std::vector<uint8_t> data_;
};

#endif /* ROM_H */
