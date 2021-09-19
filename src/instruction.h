#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <utility>
#include <tuple>

class BitMask
{
public:
    constexpr BitMask(const char *mask) : isolater_(CreateIsolater(mask, std::make_index_sequence<sizeof(mask)>{})),
                                          mask_(CreateMask(mask, std::make_index_sequence<sizeof(mask)>{}))
    {
        static_assert(sizeof(mask) == 8);
    }

    inline constexpr bool operator==(uint8_t data) const
    {
        return (data & isolater_) == mask_;
    }

    inline constexpr bool operator!=(uint8_t data) const
    {
        return !(*this == data);
    }

    inline friend bool operator==(uint8_t data, const BitMask &bitmask)
    {
        return bitmask == data;
    }

    inline friend bool operator!=(uint8_t data, const BitMask &bitmask)
    {
        return bitmask != data;
    }

private:
    const uint8_t isolater_;
    const uint8_t mask_;

    template <std::size_t... indexes>
    constexpr uint8_t CreateIsolater(const char *mask, std::index_sequence<indexes...>)
    {
        return (((mask[indexes] == '0' || mask[indexes] == '1') << (sizeof(mask) - 1 - indexes)) | ...);
    }

    template <std::size_t... indexes>
    constexpr uint8_t CreateMask(const char *mask, std::index_sequence<indexes...>)
    {
        return (((mask[indexes] == '1') << (sizeof(mask) - 1 - indexes)) | ...);
    }
};

struct Instructions
{
    static constexpr BitMask MOV_r1_r2 = BitMask("01sssddd");
    static constexpr BitMask MOV_r_M = BitMask("01sss110");
    static constexpr BitMask MOV_M_r = BitMask("01110ddd");
    static constexpr BitMask HALT = BitMask("01110110");
};

#endif /* INSTRUCTION_H */
