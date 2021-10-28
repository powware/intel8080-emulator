#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <cassert>
#include <iostream>
#include <filesystem>
#include <atomic>
#include <thread>

#include "register.h"
#include "instruction.h"
#include "ram.h"
#include "rom.h"
#include "memory.h"
#include "logger.h"

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

using namespace std::chrono_literals;

class CPU final
{
public:
    CPU() {}

    virtual ~CPU() {}

    template <typename MemoryType, typename... Args>
    requires std::is_base_of_v<MemoryInterface, MemoryType>
    void AddMemory(Args &&...args)
    {
        memory_.AddMemory<MemoryType>(std::forward<Args>(args)...);
    }

    bool Run() noexcept
    {

        try
        {
            while (true)
            {
                uint8_t op_code = FetchInstruction();
                ExecuteInstruction(op_code);

                std::this_thread::sleep_for(1ms);
            }
        }
        catch (std::runtime_error &exception)
        {
            std::cout << "CPU::Run(): " << exception.what() << "\n";

            return false;
        }

        return true;
    }

private:
    struct
    {
        bool carry;
        bool parity;
        bool auxiliary_carry;
        bool zero;
        bool sign;
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

    inline uint8_t FetchInstruction()
    {
        return ReadMemory(program_counter_++);
    }

    void ExecuteInstruction(uint8_t op_code)
    {
        Logger::Log("0x{h}: ", op_code);

        if (op_code == InstructionSet::MOV_r1_r2)
        {
            if (op_code == InstructionSet::HLT)
            {
                Logger::Log("HLT\n");
            }
            if (op_code == InstructionSet::MOV_r_M)
            {
                auto &destination = GetDestinationRegister(op_code);

                destination = ReadMemory(hl_);

                Logger::Log("MOV_r_M: {} <- ({})\n", destination, hl_);
            }
            else if (op_code == InstructionSet::MOV_M_r)
            {
                auto &source = GetSourceRegister(op_code);

                WriteMemory(hl_, source);

                Logger::Log("MOV_r_M: {} <- ({})\n", hl_, source);
            }
            else // MOV_r1_r2
            {
                auto &destination = GetDestinationRegister(op_code);
                auto &source = GetSourceRegister(op_code);

                destination = source;

                Logger::Log("MOV_r1_r2: {} <- {}\n", destination, source);
            }
        }
        else if (op_code == InstructionSet::MVI_r)
        {
            if (op_code == InstructionSet::MVI_M)
            {
                uint8_t immediate = ReadMemory(program_counter_++);

                WriteMemory(hl_, immediate);

                Logger::Log("MVI_M: ({}) <- {}\n", hl_, int(immediate));
            }
            else // MVI_r
            {
                auto &destination = GetDestinationRegister(op_code);
                uint8_t immediate = ReadMemory(program_counter_++);
                destination = immediate;

                std::cout << "MIV_r: " << destination << " <- " << int(immediate) << "\n";
            }
        }
        else if (op_code == InstructionSet::LXI)
        {
            auto &destination = GetRegisterPair(op_code);
            uint8_t immediate_low = ReadMemory(program_counter_++);
            uint8_t immediate_high = ReadMemory(program_counter_++);

            destination.high_ = immediate_high;
            destination.low_ = immediate_low;

            std::cout << "LXI: " << destination << " <- " << ((immediate_high << 8) | immediate_low) << "\n";
        }
        else if (op_code == InstructionSet::LDAX)
        {
            if (op_code == InstructionSet::LDA)
            {
                uint8_t immediate_low = ReadMemory(program_counter_++);
                uint8_t immediate_high = ReadMemory(program_counter_++);
                uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

                a_ = ReadMemory(immediate);
            }
            else if (op_code == InstructionSet::LHLD)
            {
                uint8_t immediate_low = ReadMemory(program_counter_++);
                uint8_t immediate_high = ReadMemory(program_counter_++);
                uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

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
                uint8_t immediate_low = ReadMemory(program_counter_++);
                uint8_t immediate_high = ReadMemory(program_counter_++);
                uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

                WriteMemory(immediate, a_);

                std::cout << "STA: (" << int(immediate) << ")" << a_ << "\n";
            }
            else if (op_code == InstructionSet::SHLD)
            {
                uint8_t immediate_low = ReadMemory(program_counter_++);
                uint8_t immediate_high = ReadMemory(program_counter_++);
                uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

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
                uint8_t memory = ReadMemory(hl_);

                auto temp = a_ + memory;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "ADD_M\n";
            }
            else // ADD_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ + source;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "ADD_r\n";
            }
        }
        else if (op_code == InstructionSet::ADI)
        {
            uint8_t immediate = ReadMemory(program_counter_++);

            auto temp = a_ + immediate;
            SetAllFlags(temp);
            a_ = static_cast<uint8_t>(temp);

            std::cout << "ADI\n";
        }
        else if (op_code == InstructionSet::ADC_r)
        {
            if (op_code == InstructionSet::ADC_M)
            {
                uint8_t memory = ReadMemory(hl_);

                auto temp = a_ + memory + uint8_t(flags_.carry);
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "ADC_M\n";
            }
            else // ADC_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ + source + uint8_t(flags_.carry);
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "ADC_r\n";
            }
        }
        else if (op_code == InstructionSet::ACI)
        {

            uint8_t immediate = ReadMemory(program_counter_++);

            auto temp = a_ + immediate + uint8_t(flags_.carry);
            SetAllFlags(temp);
            a_ = static_cast<uint8_t>(temp);

            std::cout << "ACI\n";
        }
        else if (op_code == InstructionSet::SUB_r)
        {
            if (op_code == InstructionSet::SUB_M)
            {
                uint8_t memory = ReadMemory(hl_);

                auto temp = a_ - memory;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "SUB_M\n";
            }
            else // SUB_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ - source;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "SUB_r\n";
            }
        }
        else if (op_code == InstructionSet::SUI)
        {

            uint8_t immediate = ReadMemory(program_counter_++);

            auto temp = a_ - immediate;
            SetAllFlags(temp);
            a_ = static_cast<uint8_t>(temp);

            std::cout << "SUI\n";
        }
        else if (op_code == InstructionSet::SBB_r)
        {
            if (op_code == InstructionSet::SBB_M)
            {
                uint8_t memory = ReadMemory(hl_);

                auto temp = a_ - memory - flags_.carry;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "SBB_M\n";
            }
            else // SBB_r
            {
                auto &source = GetSourceRegister(op_code);

                auto temp = a_ - source - flags_.carry;
                SetAllFlags(temp);
                a_ = static_cast<uint8_t>(temp);

                std::cout << "SBB_r\n";
            }
        }
        else if (op_code == InstructionSet::SBI)
        {

            uint8_t immediate = ReadMemory(program_counter_++);

            auto temp = a_ - immediate - flags_.carry;
            SetAllFlags(temp);
            a_ = static_cast<uint8_t>(temp);

            std::cout << "SBI\n";
        }
        else if (op_code == InstructionSet::INR_r)
        {
            if (op_code == InstructionSet::INR_M)
            {
                uint8_t memory = ReadMemory(hl_);

                auto temp = memory + 1;
                SetAllFlagsExceptCarry(temp);
                WriteMemory(hl_, static_cast<uint8_t>(temp));

                std::cout << "INR_M\n";
            }
            else // INR_R
            {
                auto &destination = GetDestinationRegister(op_code);

                auto temp = destination + 1;
                SetAllFlagsExceptCarry(temp);
                destination = static_cast<uint8_t>(temp);

                std::cout << "INR_r\n";
            }
        }
        else if (op_code == InstructionSet::DCR_r)
        {
            if (op_code == InstructionSet::DCR_M)
            {
                uint8_t memory = ReadMemory(hl_);

                auto temp = memory - 1;
                SetAllFlagsExceptCarry(temp);
                WriteMemory(hl_, static_cast<uint8_t>(temp));

                std::cout << "DCR_M\n";
            }
            else // DCR_R
            {
                auto &destination = GetDestinationRegister(op_code);

                auto temp = destination - 1;
                SetAllFlagsExceptCarry(temp);
                destination = static_cast<uint8_t>(temp);

                std::cout
                    << "DCR_r\n";
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
            SetCarryFlag(temp);
            hl_ = static_cast<uint16_t>(temp);

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
                uint8_t memory = ReadMemory(hl_);

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
            uint8_t immediate = ReadMemory(program_counter_++);

            a_ = a_ & immediate;
            SetAllFlags(a_);
        }
        else if (op_code == InstructionSet::XRA_r)
        {
            if (op_code == InstructionSet::XRA_M)
            {
                uint8_t memory = ReadMemory(hl_);

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
            uint8_t immediate = ReadMemory(program_counter_++);

            a_ = a_ ^ immediate;
            SetAllFlags(a_);
        }
        else if (op_code == InstructionSet::ORA_r)
        {
            if (op_code == InstructionSet::ORA_M)
            {
                uint8_t memory = ReadMemory(hl_);

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
            uint8_t immediate = ReadMemory(program_counter_++);

            a_ = a_ | immediate;
            SetAllFlags(a_);

            std::cout << "ORI\n";
        }
        else if (op_code == InstructionSet::CMP_r)
        {
            if (op_code == InstructionSet::CMP_M)
            {
                uint8_t memory = ReadMemory(hl_);

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
            uint8_t immediate = ReadMemory(program_counter_++);

            auto temp = a_ - immediate;
            SetAllFlags(temp);

            std::cout << "CPI\n";
        }
        else if (op_code == InstructionSet::RLC)
        {
            auto temp = a_ << 1;
            SetCarryFlag(temp);
            a_ = static_cast<uint8_t>(temp | uint8_t(flags_.carry));

            std::cout << "RLC\n";
        }
        else if (op_code == InstructionSet::RRC)
        {
            flags_.carry = (a_ & 0b1) == 1;
            uint8_t temp = a_ >> 1;
            a_ = static_cast<uint8_t>(temp | (uint8_t(flags_.carry) << 8));

            std::cout << "RRC\n";
        }
        else if (op_code == InstructionSet::RAL)
        {
            auto old_carry = flags_.carry;
            auto temp = a_ << 1;
            SetCarryFlag(temp);
            a_ = static_cast<uint8_t>(temp | uint8_t(old_carry));

            std::cout << "RAL\n";
        }
        else if (op_code == InstructionSet::RAR)
        {
            auto new_carry = (a_ & 0b1) == 1;
            a_ = static_cast<uint8_t>((a_ >> 1) | (uint8_t(flags_.carry) << 8));
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
        else if (op_code == InstructionSet::JMP || (op_code == InstructionSet::JC && CheckCondition(op_code)))
        {
            uint8_t immediate_low = ReadMemory(program_counter_++);
            uint8_t immediate_high = ReadMemory(program_counter_++);
            uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

            program_counter_ = immediate;

            std::cout << "JMP/JC\n";
        }
        else if (op_code == InstructionSet::CALL || (op_code == InstructionSet::CC && CheckCondition(op_code)))
        {
            WriteMemory(stack_pointer_ - 1, program_counter_.high_);
            WriteMemory(stack_pointer_ - 2, program_counter_.low_);

            stack_pointer_ = stack_pointer_ - 2;

            uint8_t immediate_low = ReadMemory(program_counter_++);
            uint8_t immediate_high = ReadMemory(program_counter_++);
            uint16_t immediate = static_cast<uint16_t>((immediate_high << 8) | immediate_low);

            program_counter_ = immediate;

            std::cout << "CALL/CC\n";
        }
        else if (op_code == InstructionSet::RET || (op_code == InstructionSet::RC && CheckCondition(op_code)))
        {
            program_counter_.low_ = ReadMemory(stack_pointer_);
            program_counter_.high_ = ReadMemory(stack_pointer_ + 1);

            stack_pointer_ = stack_pointer_ + 2;

            std::cout << "CALL/CC\n";
        }
        else if (op_code == InstructionSet::RST)
        {
            WriteMemory(stack_pointer_ - 1, program_counter_.high_);
            WriteMemory(stack_pointer_ - 2, program_counter_.low_);

            stack_pointer_ = stack_pointer_ - 2;

            program_counter_ = GetInterruptAddress(op_code);

            std::cout << "RST\n";
        }
        else if (op_code == InstructionSet::PCHL)
        {
            program_counter_ = hl_;

            std::cout << "PCHL\n";
        }
        else if (op_code == InstructionSet::PUSH_rp)
        {
            auto &source = GetRegisterPair(op_code);

            WriteMemory(stack_pointer_ - 1, source.high_);
            WriteMemory(stack_pointer_ - 2, source.low_);

            stack_pointer_ = stack_pointer_ - 2;

            std::cout << "PUSH_rp\n";
        }
        else if (op_code == InstructionSet::PUSH_PSW)
        {
            uint8_t status = static_cast<uint8_t>((uint8_t(flags_.sign) << 7) | (uint8_t(flags_.zero) << 6) | (uint8_t(flags_.auxiliary_carry) << 4) | (uint8_t(flags_.parity) << 2) | (1 << 1) | uint8_t(flags_.carry));

            WriteMemory(stack_pointer_ - 1, a_);
            WriteMemory(stack_pointer_ - 2, status);

            stack_pointer_ = stack_pointer_ - 2;

            std::cout << "PUSH_PSW\n";
        }
        else if (op_code == InstructionSet::POP_rp)
        {
            auto &destination = GetRegisterPair(op_code);

            destination.low_ = ReadMemory(stack_pointer_ + 1);
            destination.high_ = ReadMemory(stack_pointer_ + 2);

            stack_pointer_ = stack_pointer_ + 2;

            std::cout << "POP_rp\n";
        }
        else if (op_code == InstructionSet::POP_PSW)
        {
            a_ = ReadMemory(stack_pointer_ + 1);
            uint8_t status = ReadMemory(stack_pointer_ + 2);

            stack_pointer_ = stack_pointer_ + 2;

            flags_.carry = status & 0b0000'0001;
            flags_.parity = status & 0b0000'0100;
            flags_.auxiliary_carry = status & 0b0000'1000;
            flags_.zero = status & 0b0100'0000;
            flags_.sign = status & 0b1000'0000;

            std::cout << "POP_PSW\n";
        }
        else if (op_code == InstructionSet::XTHL)
        {
            uint8_t temp = l_;
            l_ = ReadMemory(stack_pointer_ + 1);
            WriteMemory(stack_pointer_ + 1, temp);

            temp = h_;
            h_ = ReadMemory(stack_pointer_ + 2);
            WriteMemory(stack_pointer_ + 2, temp);

            std::cout << "XTHL\n";
        }
        else if (op_code == InstructionSet::SPHL)
        {
            stack_pointer_ = hl_;

            std::cout << "SPHL\n";
        }
        else if (op_code == InstructionSet::IN)
        {
            std::cout << "IN\n";
        }
        else if (op_code == InstructionSet::OUT)
        {
            std::cout << "OUT\n";
        }
        else if (op_code == InstructionSet::EI)
        {
            std::cout << "EI\n";
        }
        else if (op_code == InstructionSet::DI)
        {
            std::cout << "DI\n";
        }
        else if (op_code == InstructionSet::NOP)
        {
            std::cout << "NOP\n";
        }
    }

    uint8_t ReadMemory(uint16_t address)
    {
        return memory_.Read(address);
    }

    void WriteMemory(uint16_t address, uint8_t data)
    {
        memory_.Write(address, data);
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
        switch (RegisterPairCode((op_code & 0b0011'0000) >> 4))
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

        std::terminate(); // satisfy compiler (unreachable)
    }

    enum class ConditionIdentifier : uint8_t
    {
        NZ = 0b000,
        Z = 0b001,
        NC = 0b010,
        C = 0b011,
        PO = 0b100,
        PE = 0b101,
        P = 0b110,
        M = 0b111,
    };

    bool CheckCondition(uint8_t op_code) noexcept
    {
        switch (ConditionIdentifier((op_code & 0b0011'1000) >> 3))
        {
        case ConditionIdentifier::NZ:
        {
            return !flags_.zero;
        }
        break;
        case ConditionIdentifier::Z:
        {
            return flags_.zero;
        }
        break;
        case ConditionIdentifier::NC:
        {
            return !flags_.carry;
        }
        case ConditionIdentifier::C:
        {
            return flags_.carry;
        }
        case ConditionIdentifier::PO:
        {
            return !flags_.parity;
        }
        break;
        case ConditionIdentifier::PE:
        {
            return flags_.parity;
        }
        break;
        case ConditionIdentifier::P:
        {
            return !flags_.sign;
        }
        case ConditionIdentifier::M:
        {
            return flags_.sign;
        }
        break;
        }

        std::terminate();
    }

    inline void SetAllFlags(int temp) noexcept
    {
        SetAllFlagsExceptCarry(temp);
        SetCarryFlag(temp);
    }

    inline void SetAllFlagsExceptCarry(int temp) noexcept
    {
        SetZeroFlag(temp);
        SetSignFlag(temp);
        SetParityFlag(temp);
        SetCarryFlag(temp);
        SetAuxiliaryCarryFlag(temp);
    }

    inline void SetZeroFlag(int temp) noexcept
    {
        flags_.zero = static_cast<uint8_t>(temp) == 0;
    }

    inline void SetSignFlag(int temp) noexcept
    {
        flags_.sign = (temp & 0b1000'0000) != 0;
    }

    inline void SetParityFlag(int temp) noexcept
    {
        int sum_of_bits = 0;
        for (int i = 0; i < 8; ++i)
        {
            sum_of_bits += (temp & (1 << i)) >> i;
        }

        flags_.parity = (sum_of_bits % 2) == 0;
    }

    inline void SetCarryFlag(int temp) noexcept
    {
        flags_.carry = temp > 0xFF;
    }

    inline void SetAuxiliaryCarryFlag(int temp) noexcept
    {
        unused(temp);
        // not yet supported
    }

    inline uint8_t GetInterruptAddress(uint8_t op_code) const noexcept
    {
        return op_code & 0b0011'1000;
    }
};

#endif /* CPU_H */
