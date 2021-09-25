#ifndef NEW_INSTRUCTION_H
#define NEW_INSTRUCTION_H

#include <utility>
#include <tuple>

class Instruction
{
public:
    constexpr Instruction(const char *mask) : mask_isolater_(CreateMaskIsolater(mask, std::make_index_sequence<sizeof(mask)>{})), mask_(CreateMask(mask, std::make_index_sequence<sizeof(mask)>{}))
    {
        static_assert(sizeof(mask) == 8);
    }

    constexpr virtual ~Instruction() {}

    inline constexpr bool operator==(uint8_t data) const
    {
        return (data & mask_isolater_) == mask_;
    }

    inline constexpr bool operator!=(uint8_t data) const
    {
        return !(*this == data);
    }

    inline friend bool operator==(uint8_t data, const Instruction &bitmask)
    {
        return bitmask == data;
    }

    inline friend bool operator!=(uint8_t data, const Instruction &bitmask)
    {
        return bitmask != data;
    }

protected:
    const uint8_t mask_isolater_;
    const uint8_t mask_;

    template <std::size_t... indexes>
    constexpr uint8_t CreateMaskIsolater(const char *mask, std::index_sequence<indexes...>)
    {
        return (((mask[indexes] == '0' || mask[indexes] == '1') << (sizeof(mask) - 1 - indexes)) | ...);
    }

    template <std::size_t... indexes>
    constexpr uint8_t CreateMask(const char *mask, std::index_sequence<indexes...>)
    {
        return (((mask[indexes] == '1') << (sizeof(mask) - 1 - indexes)) | ...);
    }
};

class DestinationRegisterInstruction : public Instruction
{
public:
    constexpr DestinationRegisterInstruction(const char *mask) : Instruction(mask)
    {
        static_assert(sizeof(mask) == 8);
    }

    auto GetDestinationRegister(uint8_t op_code)
    {
        return (op_code & 0b0011'1000) >> 3;
    }
};

class SourceRegisterInstruction : public Instruction
{
public:
    constexpr SourceRegisterInstruction(const char *mask) : Instruction(mask)
    {
        static_assert(sizeof(mask) == 8);
    }

    auto GetSourceRegister(uint8_t op_code)
    {
        return (op_code & 0b0000'0111);
    }
};

class DestinationSourceRegisterInstruction : public SourceRegisterInstruction, public DestinationRegisterInstruction
{
public:
    constexpr DestinationSourceRegisterInstruction(const char *mask) : Instruction(mask)
    {
        static_assert(sizeof(mask) == 8);
    }

    auto DestinationSourceRegisterInstruction(uint8_t op_code)
    {
        return (op_code & 0b0000'0111);
    }
};

struct Instructions
{
    static constexpr Instruction MOV_r1_r2 = Instruction("01dddsss");
    static constexpr auto MOV_r_M = DestinationRegisterInstruction("01ddd110");
    static constexpr Instruction MOV_M_r = Instruction("01110sss");
    static constexpr Instruction HALT = Instruction("01110110");
};

#endif /* NEW_INSTRUCTION_H */
