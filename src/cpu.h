#ifndef CPU_H
#define CPU_H

#include <cassert>
#include <cassert>
#include <iostream>

#include "register.h"
#include "instruction.h"

#include "ram.h"
#include "rom.h"
#include "memory.h"

//void HexDump(const Memory &memory)
//{
//    const char *numbers = "0123456789ABCDEF";
//    for (uint16_t i = 0; i < static_cast<uint16_t>(memory.GetSize()); i += 0x10)
//    {
//        std::cout << numbers[(i & 0x0F000) >> (3 * 4)];
//        std::cout << numbers[(i & 0x00F00) >> (2 * 4)];
//        std::cout << numbers[(i & 0x000F0) >> (1 * 4)];
//        std::cout << numbers[(i & 0x0000F)];
//
//        for (uint16_t j = 0; j < 0xF; j++)
//        {
//            uint8_t value = memory.Read(i + j);
//
//            std::cout << " ";
//            std::cout << numbers[(value & 0xF0) >> (1 * 4)];
//            std::cout << numbers[(value & 0x0F)];
//        }
//
//        std::cout << std::endl;
//    }
//}

class CPU final
{
public:
    CPU() {}

    virtual ~CPU() {}

    void AddROM(std::filesystem::path &&path)
    {
        memory_.AddMemory<ROM>(std::forward<std::filesystem::path>(path));
    }

    void AddRAM(std::size_t size)
    {
        memory_.AddMemory<RAM>(size);
    }

    void StartExecution() noexcept
    {
        try
        {
            while (true)
            {
                auto op_code = FetchInstruction();
                ExecuteInstruction(op_code);
            }
        }
        catch (std::runtime_error &exception)
        {
            std::cout << exception.what() << "\n";
        }
    }

private:
    struct
    {
        bool zero;
        bool sign;
        bool parity;
        bool carry;
        bool auxiliary_carry;
    } flags_;

    Register a_{"A"};

    RegisterPair bc_{"B", "C"};
    Register &b_ = bc_.high_;
    Register &c_ = bc_.low_;

    RegisterPair de_{"D", "E"};
    Register &d_ = de_.high_;
    Register &e_ = de_.low_;

    RegisterPair hl_{"H", "L"};
    Register &h_ = hl_.high_;
    Register &l_ = hl_.low_;

    RegisterPair stack_pointer_{"Stack Pointer"};
    RegisterPair program_counter_{"Program Counter"};

    Memory memory_;

    uint8_t FetchInstruction()
    {
        return ReadMemory(program_counter_);
    }

    void ExecuteInstruction(uint8_t op_code)
    {
        if (op_code == InstructionSet::MOV_r1_r2)
        {
            if (op_code == InstructionSet::HLT)
            {
                std::cout << "HLT\n";
            }
            if (op_code == InstructionSet::MOV_r_M)
            {
                auto &destination = GetDestinationRegister(op_code);

                destination = ReadMemory(hl_);

                std::cout << "MOV_r_M: " << destination << " <- (" << hl_ << ")\n";
            }
            else if (op_code == InstructionSet::MOV_M_r)
            {
                auto &source = GetSourceRegister(op_code);

                WriteMemory(hl_, source);

                std::cout << "MOV_M_r: (" << hl_ << ") <- " << source << "\n";
            }
            else // MOV_r1_r2
            {
                auto &destination = GetDestinationRegister(op_code);
                auto &source = GetSourceRegister(op_code);

                destination = source;

                std::cout << "MOV_r1_r2: " << destination << " <- " << source << "\n";
            }
        }
        else if (op_code == InstructionSet::MVI_r)
        {
            if (op_code == InstructionSet::MVI_M)
            {
                auto immediate = ReadMemory(++program_counter_);

                WriteMemory(hl_, immediate);

                std::cout << "MVI_M: (" << hl_ << ") <- " << int(immediate) << "\n";
            }
            else // MVI_r
            {
                auto immediate = ReadMemory(++program_counter_);
                auto &destination = GetDestinationRegister(op_code);
                destination = immediate;

                std::cout << "MIV_r: " << destination << " <- " << int(immediate) << "\n";
            }
        }
        else if (op_code == InstructionSet::LXI)
        {
            auto &destination = GetRegisterPair(op_code);
            auto immediate_low = ReadMemory(++program_counter_);
            auto immediate_high = ReadMemory(++program_counter_);

            destination.high_ = immediate_high;
            destination.low_ = immediate_low;

            std::cout << "LXI: " << destination << " <- " << int((immediate_high << 8) | immediate_low) << "\n";
        }
        else if (op_code == InstructionSet::LDAX)
        {
            if (op_code == InstructionSet::LDA)
            {
                auto immediate_low = ReadMemory(++program_counter_);
                auto immediate_high = ReadMemory(++program_counter_);
                auto immediate = (immediate_high << 8) | immediate_low;

                a_ = ReadMemory(immediate);
            }
            else if (op_code == InstructionSet::LHLD)
            {
                auto immediate_low = ReadMemory(++program_counter_);
                auto immediate_high = ReadMemory(++program_counter_);
                auto immediate = (immediate_high << 8) | immediate_low;

                l_ = ReadMemory(immediate);
                h_ = ReadMemory(++immediate);

                std::cout << "LHLD: " << l_ << " <- (" << immediate - 1 << ") && " << h_ << " <- (" << immediate << ")\n";
            }
            else // LDAX
            {
                auto &source = GetRegisterPair(op_code);

                a_ = ReadMemory(source);

                std::cout << "LDAX: " << a_ << " <- (" << source << ")\n";
            }
        }
        else if (op_code == InstructionSet::STAX)
        {
            if (op_code == InstructionSet::STA)
            {
                auto immediate_low = ReadMemory(++program_counter_);
                auto immediate_high = ReadMemory(++program_counter_);
                auto immediate = (immediate_high << 8) | immediate_low;

                WriteMemory(immediate, a_);

                std::cout << "STA: (" << int(immediate) << ")" << a_ << "\n";
            }
            else if (op_code == InstructionSet::SHLD)
            {
                auto immediate_low = ReadMemory(++program_counter_);
                auto immediate_high = ReadMemory(++program_counter_);
                auto immediate = (immediate_high << 8) | immediate_low;

                WriteMemory(immediate, l_);
                WriteMemory(++immediate, h_);

                std::cout << "SHLD: (" << immediate - 1 << ") <- " << l_ << " && (" << immediate << ")" << h_ << "\n";
            }
            else // STAX
            {
                auto &destination = GetRegisterPair(op_code);

                WriteMemory(destination, a_);

                std::cout << "STAX: (" << destination << ") <- " << a_ << "\n";
            }
        }
        else if (op_code == InstructionSet::XCHG)
        {
            swap(h_, d_);
            swap(l_, e_);

            std::cout << "XCHG\n";
        }
        else if (op_code == InstructionSet::ADD_r)
        {
            if (op_code == InstructionSet::ADD_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = a_ + memory;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "ADD_M\n";
            }
            else // ADD_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ + source;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "ADD_r\n";
            }
        }
        else if (op_code == InstructionSet::ADI)
        {

            auto immediate = ReadMemory(++program_counter_);

            auto temp = a_ + immediate;
            SetAllFlags(temp);
            a_ = temp;

            std::cout << "ADi\n";
        }
        else if (op_code == InstructionSet::ADC_r)
        {
            if (op_code == InstructionSet::ADC_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = a_ + memory + flags_.carry;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "ADC_M\n";
            }
            else // ADC_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ + source + flags_.carry;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "ADC_r\n";
            }
        }
        else if (op_code == InstructionSet::ACI)
        {

            auto immediate = ReadMemory(++program_counter_);

            auto temp = a_ + immediate + flags_.carry;
            SetAllFlags(temp);
            a_ = temp;

            std::cout << "ACI\n";
        }
        else if (op_code == InstructionSet::SUB_r)
        {
            if (op_code == InstructionSet::SUB_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = a_ - memory;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "SUB_M\n";
            }
            else // SUB_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ - source;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "SUB_r\n";
            }
        }
        else if (op_code == InstructionSet::SUI)
        {

            auto immediate = ReadMemory(++program_counter_);

            auto temp = a_ - immediate;
            SetAllFlags(temp);
            a_ = temp;

            std::cout << "SUI\n";
        }
        else if (op_code == InstructionSet::SBB_r)
        {
            if (op_code == InstructionSet::SBB_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = a_ - memory - flags_.carry;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "SBB_M\n";
            }
            else // SBB_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ - source - flags_.carry;
                SetAllFlags(temp);
                a_ = temp;

                std::cout << "SBB_r\n";
            }
        }
        else if (op_code == InstructionSet::SBI)
        {

            auto immediate = ReadMemory(++program_counter_);

            auto temp = a_ - immediate - flags_.carry;
            SetAllFlags(temp);
            a_ = temp;

            std::cout << "SBI\n";
        }
        else if (op_code == InstructionSet::INR_r)
        {
            if (op_code == InstructionSet::INR_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = memory + 1;
                SetAllFlagsExceptCarry(temp);
                WriteMemory(hl_, temp);

                std::cout << "INR_M\n";
            }
            else // INR_R
            {
                auto &destination = GetDestinationRegister(op_code);

                auto temp = destination + 1;
                SetAllFlagsExceptCarry(temp);
                destination = temp;

                std::cout << "INR_r\n";
            }
        }
        else if (op_code == InstructionSet::DCR_r)
        {
            if (op_code == InstructionSet::DCR_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = memory - 1;
                SetAllFlagsExceptCarry(temp);
                WriteMemory(hl_, temp);

                std::cout << "DCR_M\n";
            }
            else // DCR_R
            {
                auto &destination = GetDestinationRegister(op_code);

                auto temp = destination - 1;
                SetAllFlagsExceptCarry(temp);
                destination = temp;

                std::cout << "DCR_r\n";
            }
        }
        else if (op_code == InstructionSet::INX)
        {
            auto &destination = GetRegisterPair(op_code);

            destination = destination + 1;

            std::cout << "INX\n";
        }
        else if (op_code == InstructionSet::DCX)
        {
            auto &destination = GetRegisterPair(op_code);

            destination = destination - 1;

            std::cout << "DCX\n";
        }
        else if (op_code == InstructionSet::DAD)
        {
            auto &source = GetRegisterPair(op_code);

            auto temp = hl_ + source;
            flags_.carry = temp > 0xFFFF;
            hl_ = temp;

            std::cout << "DAD\n";
        }
        else if (op_code == InstructionSet::DAA)
        {
            std::cout << "DAA not supported\n";
        }
        else if (op_code == InstructionSet::ANA_r)
        {
            if (op_code == InstructionSet::ANA_M)
            {
                auto memory = ReadMemory(hl_);

                a_ = a_ & memory;
                SetAllFlags(a_);

                std::cout << "ANA_M\n";
            }
            else // ANA_r
            {
                auto &source = GetSourceRegister(op_code);

                a_ = a_ & source;
                SetAllFlags(a_);

                std::cout << "ANA_r\n";
            }
        }
        else if (op_code == InstructionSet::ANI)
        {
            auto immediate = ReadMemory(++program_counter_);

            a_ = a_ & immediate;
            SetAllFlags(a_);
        }
        else if (op_code == InstructionSet::XRA_r)
        {
            if (op_code == InstructionSet::XRA_M)
            {
                auto memory = ReadMemory(hl_);

                a_ = a_ ^ memory;
                SetAllFlags(a_);

                std::cout << "XRA_M\n";
            }
            else // XRA_r
            {
                auto &source = GetSourceRegister(op_code);

                a_ = a_ ^ source;
                SetAllFlags(a_);

                std::cout << "XRA_r\n";
            }
        }
        else if (op_code == InstructionSet::XRI)
        {
            auto immediate = ReadMemory(++program_counter_);

            a_ = a_ ^ immediate;
            SetAllFlags(a_);
        }
        else if (op_code == InstructionSet::ORA_r)
        {
            if (op_code == InstructionSet::ORA_M)
            {
                auto memory = ReadMemory(hl_);

                a_ = a_ | memory;
                SetAllFlags(a_);

                std::cout << "ORA_M\n";
            }
            else // ORA_r
            {
                auto &source = GetSourceRegister(op_code);

                a_ = a_ | source;
                SetAllFlags(a_);

                std::cout << "ORA_r\n";
            }
        }
        else if (op_code == InstructionSet::ORI)
        {
            auto immediate = ReadMemory(++program_counter_);

            a_ = a_ | immediate;
            SetAllFlags(a_);

            std::cout << "ORI\n";
        }
        else if (op_code == InstructionSet::CMP_r)
        {
            if (op_code == InstructionSet::CMP_M)
            {
                auto memory = ReadMemory(hl_);

                auto temp = a_ - memory;
                SetAllFlags(temp);

                std::cout << "CMP_M\n";
            }
            else // CMP_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ - source;
                SetAllFlags(temp);

                std::cout << "CMP_r\n";
            }
        }
        else if (op_code == InstructionSet::CPI)
        {
            auto immediate = ReadMemory(++program_counter_);

            auto temp = a_ - immediate;
            SetAllFlags(temp);

            std::cout << "CPI\n";
        }
        else if (op_code == InstructionSet::RLC)
        {
            auto temp = a_ << 1;
            SetCarryFlag(temp);
            a_ = temp | uint8_t{flags_.carry};

            std::cout << "RLC\n";
        }
        else if (op_code == InstructionSet::RRC)
        {
            flags_.carry = (a_ & 0b1) == 1;
            uint8_t temp = a_ >> 1;
            a_ = temp | (uint8_t{flags_.carry} << 8);

            std::cout << "RRC\n";
        }
        else if (op_code == InstructionSet::RAL)
        {
            auto old_carry = flags_.carry;
            auto temp = a_ << 1;
            SetCarryFlag(temp);
            a_ = temp | uint8_t{old_carry};

            std::cout << "RAL\n";
        }
        else if (op_code == InstructionSet::RAR)
        {
            auto new_carry = (a_ & 0b1) == 1;
            a_ = (a_ >> 1) | (uint8_t{flags_.carry} << 8);
            flags_.carry = new_carry;

            std::cout << "RAR\n";
        }
        else if (op_code == InstructionSet::CMA)
        {
            a_ = ~a_;

            std::cout << "CMA\n";
        }
        else if (op_code == InstructionSet::CMC)
        {
            flags_.carry = !flags_.carry;

            std::cout << "CMC\n";
        }
        else if (op_code == InstructionSet::STC)
        {
            flags_.carry = true;

            std::cout << "STC\n";
        }
        else
        {
            std::cout << "Instruction not supported\n";
        }

        ++program_counter_;
    }

    uint8_t ReadMemory(int address)
    {
        return memory_.Read(narrow_cast<uint16_t>(address));
    }

    void WriteMemory(int address, int data)
    {
        memory_.Write(narrow_cast<uint16_t>(address), narrow_cast<uint8_t>(data));
    }

    Register &GetRegister(uint8_t register_code) noexcept
    {
        assert(register_code <= 0b111 && register_code != 0b110);

        switch (RegisterCode{register_code})
        {
        case RegisterCode::A:
        {
            return a_;
        }
        break;
        case RegisterCode::B:
        {
            return b_;
        }
        case RegisterCode::C:
        {
            return c_;
        }
        case RegisterCode::D:
        {
            return d_;
        }
        case RegisterCode::E:
        {
            return e_;
        }
        case RegisterCode::H:
        {
            return h_;
        }
        case RegisterCode::L:
        {
            return l_;
        }
        break;
        }

        std::terminate();
    }

    Register &GetSourceRegister(uint8_t op_code) noexcept
    {
        return GetRegister(op_code & 0b0000'0111);
    }

    Register &GetDestinationRegister(uint8_t op_code) noexcept
    {
        return GetRegister((op_code & 0b0011'1000) >> 3);
    }

    RegisterPair &GetRegisterPair(uint8_t op_code) noexcept
    {
        const auto register_pair_code = narrow_cast<uint8_t>((op_code & 0b0011'0000) >> 4);

        switch (RegisterPairCode{register_pair_code})
        {
        case RegisterPairCode::BC:
        {
            return bc_;
        }
        break;
        case RegisterPairCode::DE:
        {
            return de_;
        }
        case RegisterPairCode::HL:
        {
            return hl_;
        }
        case RegisterPairCode::SP:
        {
            return stack_pointer_;
        }
        break;
        }

        std::terminate();
    }

    inline void SetAllFlags(int temp)
    {
        SetAllFlagsExceptCarry(temp);
        SetCarryFlag(temp);
    }

    inline void SetAllFlagsExceptCarry(int temp)
    {
        SetZeroFlag(temp);
        SetSignFlag(temp);
        SetParityFlag(temp);
        SetCarryFlag(temp);
        SetAuxiliaryCarryFlag(temp);
    }

    inline void SetZeroFlag(int temp)
    {
        flags_.zero = narrow_cast<uint8_t>(temp) == 0;
    }

    inline void SetSignFlag(int temp)
    {
        flags_.sign = (temp & 0b1000'0000) != 0;
    }

    inline void SetParityFlag(int temp)
    {
        auto sum_of_bits = 0;
        for (int i = 0; i < 8; ++i)
        {
            sum_of_bits += (temp & (1 << i)) >> i;
        }

        flags_.parity = (sum_of_bits % 2) == 0;
    }

    inline void SetCarryFlag(int temp)
    {
        flags_.carry = temp > 0xFF;
    }

    inline void SetAuxiliaryCarryFlag(int temp)
    {
        (void)temp;
        // not yet supported
    }
};

#endif /* CPU_H */
