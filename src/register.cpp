#include "register.h"

Register::Register(std::string name) noexcept : data_(owned_data_), name_(name) {}

Register::Register(uint8_t &data, std::string name) noexcept : data_(data), name_(name) {}

Register::~Register() {}

Register::operator uint8_t() const noexcept
{
    return data_;
}

Register::operator std::string() const noexcept
{
    return name_ + "(" + std::to_string(int(data_)) + ")";
}

RegisterPair::RegisterPair(std::string high_name, std::string low_name) noexcept : low_(reinterpret_cast<uint8_t *>(&data_)[0], low_name), high_(reinterpret_cast<uint8_t *>(&data_)[1], high_name)
{
    name_ = high_name + low_name;
}

RegisterPair::RegisterPair(std::string name) noexcept : RegisterPair(name + "_low", name + "_high")
{
    name_ = name;
}

RegisterPair::~RegisterPair() {}

RegisterPair::operator uint16_t() const noexcept
{
    return data_;
}

RegisterPair::operator std::string() const noexcept
{
    return name_ + "(" + std::to_string(int(data_)) + ")";
}
