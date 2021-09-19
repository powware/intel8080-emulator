#ifndef REGISTER_H
#define REGISTER_H

#include <array>
#include <string>
#include <string_view>

class Register8 final
{
public:
    Register8(std::string name) : data_reference_(data_), name_(name) {}

    Register8(uint8_t &data, std::string name) : data_reference_(data), name_(name) {}

    Register8(const Register8 &) = delete;

    Register8(Register8 &&) = delete;

    Register8 &operator=(uint8_t data)
    {
        data_reference_ = data;

        return *this;
    }

    Register8 &operator=(Register8 &data)
    {
        data_reference_ = data;

        return *this;
    }

    operator uint8_t &()
    {
        return data_;
    }

    operator const std::string &() const
    {
        return name_;
    }

private:
    uint8_t data_{0};
    uint8_t &data_reference_;
    const std::string name_;
};

class Register16 final
{
public:
    Register16(std::string name) : name_(name) {}

    Register16(const Register16 &) = delete;

    Register16(Register16 &&) = delete;

    uint8_t &GetHigh()
    {
        return reinterpret_cast<uint8_t *>(&data_)[0];
    }

    uint8_t &GetLow()
    {
        return reinterpret_cast<uint8_t *>(&data_)[1];
    }

    uint16_t &GetHighLow()
    {
        return data_;
    }

    operator uint16_t &()
    {
        return data_;
    }

    operator const std::string &() const
    {
        return name_;
    }

private:
    uint16_t data_{0};
    const std::string name_;
};

class RegisterPair final
{
public:
    RegisterPair(std::string name_high, std::string name_low) : high_low_(name_high + name_low), high_(high_low_.GetHigh(), name_high), low_(high_low_.GetLow(), name_low) {}

    Register8 &GetHigh()
    {
        return high_;
    }

    Register8 &GetLow()
    {
        return low_;
    }

    Register16 &GetHighLow()
    {
        return high_low_;
    }

    operator Register16 &()
    {
        return high_low_;
    }

    operator uint16_t &()
    {
        return high_low_;
    }

    operator const std::string &() const
    {
        return high_low_;
    }

private:
    Register16 high_low_;
    Register8 high_;
    Register8 low_;
    const std::string name_;
};

#endif /* REGISTER_H */
