#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>
#include <cassert>
#include <utility>
#include <string_view>

class Instruction
{
public:
    constexpr Instruction(const char *mask) noexcept : mask_isolater_(CreateMaskIsolater(mask, std::make_index_sequence<CHAR_BIT>{})), mask_(CreateMask(mask, std::make_index_sequence<CHAR_BIT>{}))
    {
        assert(std::string_view(mask).size() == CHAR_BIT);
    }

    inline constexpr bool operator==(uint8_t data) const noexcept
    {
        return (data & mask_isolater_) == mask_;
    }

    inline constexpr bool operator!=(uint8_t data) const noexcept
    {
        return !(*this == data);
    }

    inline friend bool operator==(uint8_t data, const Instruction &bitmask) noexcept
    {
        return bitmask == data;
    }

    inline friend bool operator!=(uint8_t data, const Instruction &bitmask) noexcept
    {
        return bitmask != data;
    }

private:
    const uint8_t mask_isolater_;
    const uint8_t mask_;

    template <std::size_t... indexes>
    constexpr uint8_t CreateMaskIsolater(const char *mask, std::index_sequence<indexes...>) const noexcept
    {
        return static_cast<uint8_t>((((mask[indexes] == '0' || mask[indexes] == '1') << (CHAR_BIT - 1 - indexes)) | ...));
    }

    template <std::size_t... indexes>
    constexpr uint8_t CreateMask(const char *mask, std::index_sequence<indexes...>) const noexcept
    {
        return static_cast<uint8_t>((((mask[indexes] == '1') << (CHAR_BIT - 1 - indexes)) | ...));
    }
};

struct InstructionSet
{
    static constexpr auto MOV_r1_r2 = Instruction("01DDDSSS");
    static constexpr auto HLT = Instruction("01110110");
    static constexpr auto MOV_r_M = Instruction("01DDD110");
    static constexpr auto MOV_M_r = Instruction("01110SSS");

    static constexpr auto STAX = Instruction("00RP0010");
    static constexpr auto STA = Instruction("00110010");
    static constexpr auto SHLD = Instruction("00100010");

    static constexpr auto MVI_r = Instruction("00DDD110");
    static constexpr auto MVI_M = Instruction("00110110");

    static constexpr auto LXI = Instruction("00RP0001");

    static constexpr auto LDAX = Instruction("00RP1010");
    static constexpr auto LHLD = Instruction("00101010");
    static constexpr auto LDA = Instruction("00111010");

    static constexpr auto XCHG = Instruction("11101011");

    static constexpr auto ADD_r = Instruction("10000SSS");
    static constexpr auto ADD_M = Instruction("10000110");

    static constexpr auto ADI = Instruction("11000110");

    static constexpr auto ADC_r = Instruction("10001SSS");
    static constexpr auto ADC_M = Instruction("10001110");

    static constexpr auto ACI = Instruction("11001110");

    static constexpr auto SUB_r = Instruction("10010SSS");
    static constexpr auto SUB_M = Instruction("10010110");

    static constexpr auto SUI = Instruction("11010110");

    static constexpr auto SBB_r = Instruction("10011SSS");
    static constexpr auto SBB_M = Instruction("10011110");

    static constexpr auto SBI = Instruction("11011110");

    static constexpr auto INR_r = Instruction("00DDD100");
    static constexpr auto INR_M = Instruction("00110100");

    static constexpr auto DCR_r = Instruction("00DDD101");
    static constexpr auto DCR_M = Instruction("00110101");

    static constexpr auto INX = Instruction("00RP0011");

    static constexpr auto DCX = Instruction("00RP1011");

    static constexpr auto DAD = Instruction("00RP1001");

    static constexpr auto DAA = Instruction("00100111");

    static constexpr auto ANA_r = Instruction("10100SSS");
    static constexpr auto ANA_M = Instruction("10100110");

    static constexpr auto ANI = Instruction("11100110");

    static constexpr auto XRA_r = Instruction("10101SSS");
    static constexpr auto XRA_M = Instruction("10101110");

    static constexpr auto XRI = Instruction("11101110");

    static constexpr auto ORA_r = Instruction("10110SSS");
    static constexpr auto ORA_M = Instruction("10110110");

    static constexpr auto ORI = Instruction("11110110");

    static constexpr auto CMP_r = Instruction("10111SSS");
    static constexpr auto CMP_M = Instruction("10111110");

    static constexpr auto CPI = Instruction("11111110");

    static constexpr auto RLC = Instruction("00000111");

    static constexpr auto RRC = Instruction("00001111");

    static constexpr auto RAL = Instruction("00010111");

    static constexpr auto RAR = Instruction("00011111");

    static constexpr auto CMA = Instruction("00101111");

    static constexpr auto CMC = Instruction("00111111");

    static constexpr auto STC = Instruction("00110111");

    static constexpr auto JMP = Instruction("11000011");

    static constexpr auto JC = Instruction("11CCC010");

    static constexpr auto CALL = Instruction("11001101");

    static constexpr auto CC = Instruction("11CCC100");

    static constexpr auto RET = Instruction("11001001");

    static constexpr auto RC = Instruction("11CCC000");

    static constexpr auto RST = Instruction("11NNN111");

    static constexpr auto PCHL = Instruction("11101001");

    static constexpr auto PUSH_rp = Instruction("11RP0101");
    static constexpr auto PUSH_PSW = Instruction("11110101");

    static constexpr auto POP_rp = Instruction("11RP0001");
    static constexpr auto POP_PSW = Instruction("11110001");

    static constexpr auto XTHL = Instruction("11100011");

    static constexpr auto SPHL = Instruction("11111001");

    static constexpr auto IN = Instruction("11011011");

    static constexpr auto OUT = Instruction("11010011");

    static constexpr auto EI = Instruction("11111011");

    static constexpr auto DI = Instruction("11110011");

    static constexpr auto NOP = Instruction("00000000");
};

#endif /* INSTRUCTION_H */
