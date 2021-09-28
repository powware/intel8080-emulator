#ifndef ROM_H
#define ROM_H

#include <initializer_list>
#include <filesystem>
#include <fstream>

#include "memory_interface.h"

class ROM final : public MemoryInterface
{
public:
    template <typename... Paths>
    ROM(Paths &&...file_paths)
    {
        data_.reserve((std::filesystem::file_size(std::forward<Paths>(file_paths)) + ...));

        (InsertData(std::forward<Paths>(file_paths)),
         ...);
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

    uint8_t Read(std::size_t index) const override
    {
        assert(index < data_.size());

        return data_[index];
    }

    void Write(std::size_t index, uint8_t data) override
    {
        (void)index;
        (void)data;

        throw std::runtime_error("Write on read only memory.");
    }

private:
    std::vector<uint8_t> data_;

    template <typename Path>
    void InsertData(const Path &file_path)
    {
        std::ifstream file_stream(file_path, std::ios::binary);
        data_.insert(data_.end(), std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
    }
};

#endif /* ROM_H */
