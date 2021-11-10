
#include "ram.h"

RAM::RAM(std::size_t size) : data_(size, 0) {}

RAM::~RAM() {}

std::size_t RAM::size() const noexcept
{
    return data_.size();
}

uint8_t RAM::Read(std::size_t index) const noexcept
{
    return data_[index];
}

void RAM::Write(std::size_t index, uint8_t data) noexcept
{
    data_[index] = data;
}