#ifndef ROM_H
#define ROM_H

#include <initializer_list>
#include <filesystem>

#include "memory_interface.h"

class Rom final : public MemoryInterface
{
public:
    template <typename... Paths>
    Rom(Paths &&...file_paths)
    {
        (InsertData(std::forward<Paths>(file_paths)),
         ...);
    }

    Rom(const Rom &) = delete;

    Rom(Rom &&rom) : data_(std::move(rom.data_)) {}

    virtual ~Rom(){};

    std::size_t GetSize() const override
    {
        return data_.size();
    }

    bool Read(std::size_t index, uint8_t &data) const override
    {
        data = static_cast<uint8_t>(data_[index]);

        return true;
    }

    bool Write(std::size_t index, uint8_t data) override
    {
        data_[index] = static_cast<char>(data);

        return true;
    }

    auto operator=(const Rom &) = delete;

    auto operator=(Rom &&) = delete;

private:
    std::vector<char> data_;

    template <typename Path>
    void InsertData(const Path &file_path)
    {
        std::ifstream file_stream(file_path, std::ios::binary);
        data_.insert(data_.end(), std::istreambuf_iterator<char>(file_stream), std::istreambuf_iterator<char>());
    }
};

#endif /* ROM_H */
