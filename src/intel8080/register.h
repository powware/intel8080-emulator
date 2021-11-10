#ifndef REGISTER_H
#define REGISTER_H

#include <cstdint>
#include <string>
#include <type_traits>
#include <iostream>

class Register final
{
public:
    Register(std::string name) noexcept;

    Register(uint8_t &data, std::string name) noexcept;

    Register(const Register &) = delete;

    Register(Register &&) = delete;

    virtual ~Register();

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

    operator uint8_t() const noexcept;

    operator std::string() const noexcept;

    inline friend void swap(Register &lhs, Register &rhs) noexcept
    {
        uint8_t temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    inline friend auto &
    operator<<(std::ostream &os, const Register &rhs)
    {
        return os << static_cast<std::string>(rhs);
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

    RegisterPair(std::string high_name, std::string low_name) noexcept;

    RegisterPair(std::string name) noexcept;

    RegisterPair(const RegisterPair &) = delete;

    RegisterPair(RegisterPair &&) = delete;

    virtual ~RegisterPair();

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

    operator uint16_t() const noexcept;

    operator std::string() const noexcept;

    inline friend auto &operator<<(std::ostream &os, const RegisterPair &r)
    {
        return os << static_cast<std::string>(r);
    }

private:
    uint16_t data_{0};
    std::string name_;
};

#endif /* REGISTER_H */
