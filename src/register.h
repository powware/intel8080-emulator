#ifndef REGISTER_H
#define REGISTER_H

#include <array>
#include <string>
#include <type_traits>

#include "types.h"

enum class RegisterCode : uint8_t
{
    A = 0b111,
    B = 0b000,
    C = 0b001,
    D = 0b010,
    E = 0b011,
    H = 0b100,
    L = 0b101
};

enum RegisterPairCode : uint8_t
{
    BC = 0b00,
    DE = 0b01,
    HL = 0b10,
    SP = 0b11
};

class Register final
{
public:
    Register(std::string name) noexcept : data_(owned_data_), name_(name) {}

    Register(uint8_t &data, std::string name) noexcept : data_(data), name_(name) {}

    Register(const Register &) = delete;

    Register(Register &&) = delete;

    virtual ~Register() {}

    auto &GetData() noexcept
    {
        return data_;
    }

    auto &operator=(const Register &r) noexcept
    {
        data_ = r.data_;

        return *this;
    }

    auto &operator=(Register &&r) = delete;

    auto &operator=(const int &value) noexcept
    {
        data_ = narrow_cast<uint8_t>(value);

        return *this;
    }

    auto &operator++(int) = delete;

    operator uint8_t() const noexcept
    {
        return data_;
    }

    friend inline void swap(Register &r1, Register &r2) noexcept
    {
        uint8_t temp = r1;
        r1 = r2;
        r2 = temp;
    }

    friend inline auto &
    operator<<(std::ostream &os, const Register &r)
    {
        return os << r.name_ << "(" << int{r.data_} << ")";
    }

private:
    uint8_t owned_data_{0};
    uint8_t &data_;
    const std::string name_;
};

class RegisterPair final
{
public:
    Register high_;
    Register low_;

    RegisterPair(std::string name) noexcept : high_(reinterpret_cast<uint8_t *>(&data_)[0], name + "_high"), low_(reinterpret_cast<uint8_t *>(&data_)[1], name + "_low"), name_(name) {}

    RegisterPair(std::string high_name, std::string low_name) noexcept : high_(reinterpret_cast<uint8_t *>(&data_)[0], high_name), low_(reinterpret_cast<uint8_t *>(&data_)[1], low_name), name_(high_name + low_name) {}

    RegisterPair(const RegisterPair &) = delete;

    RegisterPair(RegisterPair &&) = delete;

    virtual ~Register() {}

    auto &operator=(const RegisterPair &r) noexcept
    {
        data_ = r.data_;

        return *this;
    }

    auto &operator=(RegisterPair &&) = delete;

    auto &operator=(int value) noexcept
    {
        data_ = narrow_cast<uint16_t>(value);

        return *this;
    }

    auto &operator++() noexcept
    {
        ++data_;

        return *this;
    }

    auto &operator++(int) = delete;

    operator uint16_t() const noexcept
    {
        return data_;
    }

    friend auto &operator<<(std::ostream &os, const RegisterPair &r)
    {
        return os << r.name_ << "(" << int{r.data_} << ")";
    }

private:
    uint16_t data_{0};
    const std::string name_;
};

#endif /* REGISTER_H */
