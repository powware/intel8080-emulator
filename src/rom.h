#ifndef ROM_H
#define ROM_H

#include <initializer_list>
#include <filesystem>

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

    ROM(ROM &&rom) : data_(std::move(rom.data_)) {}

    virtual ~ROM(){};

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
        (void)index;
        (void)data;

        return false;
    }

    auto operator=(const ROM &) = delete;

    auto operator=(ROM &&) = delete;

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
