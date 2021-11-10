#include "rom.h"

#include <fstream>
#include <stdexcept>

#include "utilities.h"

ROM::ROM(const std::filesystem::path &file_path)
{
    const auto file_size = std::filesystem::file_size(file_path);
    data_.resize(file_size);

    std::ifstream file_stream(file_path, std::ios::binary);
    file_stream.read(reinterpret_cast<char *>(data_.data()), static_cast<std::streamsize>(file_size));
}

ROM::~ROM() {}

std::size_t ROM::size() const noexcept
{
    return data_.size();
}

uint8_t ROM::Read(std::size_t index) const noexcept
{
    return data_[index];
}

void ROM::Write(std::size_t index, uint8_t data)
{
    unused(index, data);

    throw std::runtime_error("ROM::Write(): Attempting to write read only memory.");
}
