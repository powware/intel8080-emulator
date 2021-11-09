#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>
#include <array>
#include <string>
#include <type_traits>

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

    auto &operator=(uint8_t value) noexcept
    {
        data_ = value;

        return *this;
    }

    auto &operator++() noexcept
    {
        ++data_;

        return *this;
    }

    auto operator++(int) noexcept
    {
        auto temp = data_;
        ++data_;

        return temp;
    }

    auto &operator--() noexcept
    {
        data_ = data_ + ~uint8_t(1) + 1;

        return *this;
    }

    auto operator--(int) noexcept
    {
        auto temp = data_;
        --*this;

        return temp;
    }

    operator uint8_t() const noexcept
    {
        return data_;
    }

    operator std::string() const noexcept
    {
        return name_ + "(" + std::to_string(int(data_)) + ")";
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
        return os << static_cast<std::string>(r);
    }

private:
    uint8_t owned_data_{0};
    uint8_t &data_;
    const std::string name_;
};

class RegisterPair final
{
public:
    Register low_;
    Register high_;

    RegisterPair(std::string high_name, std::string low_name) noexcept : low_(reinterpret_cast<uint8_t *>(&data_)[0], low_name), high_(reinterpret_cast<uint8_t *>(&data_)[1], high_name)
    {
        name_ = high_name + low_name;
    }

    RegisterPair(std::string name) noexcept : RegisterPair(name + "_low", name + "_high")
    {
        name_ = name;
    }

    RegisterPair(const RegisterPair &) = delete;

    RegisterPair(RegisterPair &&) = delete;

    virtual ~RegisterPair() {}

    auto &operator=(const RegisterPair &r) noexcept
    {
        data_ = r.data_;

        return *this;
    }

    auto &operator=(RegisterPair &&) = delete;

    auto &operator=(uint16_t value) noexcept
    {
        data_ = value;

        return *this;
    }

    auto &operator++() noexcept
    {
        ++data_;

        return *this;
    }

    auto operator++(int) noexcept
    {
        auto temp = data_;
        ++data_;

        return temp;
    }

    auto &operator--() noexcept
    {
        data_ = data_ + ~uint16_t(1) + 1;

        return *this;
    }

    auto operator--(int) noexcept
    {
        auto temp = data_;
        --*this;

        return temp;
    }

    operator uint16_t() const noexcept
    {
        return data_;
    }

    operator std::string() const noexcept
    {
        return name_ + "(" + std::to_string(int(data_)) + ")";
    }

    friend auto &operator<<(std::ostream &os, const RegisterPair &r)
    {
        return os << static_cast<std::string>(r);
    }

private:
    uint16_t data_{0};
    std::string name_;
};

#endif /* REGISTER_H */
