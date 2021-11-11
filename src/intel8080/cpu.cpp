#include "cpu.h"

#include <cassert>

#include <SFML/Window/Keyboard.hpp>

#include "instruction.h"
#include "logger.h"

inline static std::tuple<uint16_t, uint8_t> Addition(uint8_t lhs, uint8_t rhs, bool carry = false)
{
    uint16_t result = static_cast<uint16_t>(lhs + rhs + uint8_t(carry));
    return std::tuple(result, static_cast<uint8_t>(result ^ lhs ^ rhs ^ uint8_t(carry) >> 1));
}

inline static std::tuple<uint16_t, uint8_t> Subtraction(uint8_t lhs, uint8_t rhs, bool carry = false)
{
    return Addition(lhs, ~rhs, !carry);
}

CPU::CPU() {}

CPU::~CPU() {}

void CPU::Run()
{
    executing_ = true;
    while (executing_)
    {
        uint8_t op_code = FetchInstruction();
        ExecuteInstruction(op_code);
    }
}

void CPU::Stop() noexcept
{
    executing_ = false;
}

void CPU::Interrupt(uint8_t interrupt) noexcept
{
    assert(interrupt <= 0b111);

    std::scoped_lock lock(interrupt_mutex_);
    if (interrupts_enabled_)
    {
        interrupt_ = interrupt;
        interrupt_requested_ = true;
    }
}

inline uint8_t CPU::FetchInstruction()
{
    Logger::Instance() << std::hex << static_cast<uint16_t>(program_counter_) << std::dec << ": ";
    std::scoped_lock lock(interrupt_mutex_);
    if (interrupts_enabled_ && interrupt_requested_)
    {
        interrupt_requested_ = false;
        return static_cast<uint8_t>(0b1100'0111 | interrupt_ << 3); // return RST interrupt_
    }

    return ReadMemory(program_counter_++);
}

void CPU::ExecuteInstruction(uint8_t op_code)
{
    if (op_code == InstructionSet::MOV_r1_r2)
    {
        if (op_code == InstructionSet::HLT)
        {
            throw std::runtime_error("HLT");
        }
        if (op_code == InstructionSet::MOV_r_M)
        {
            auto &destination = GetDestinationRegister(op_code);
            destination = ReadMemory(hl_);

            Logger::Instance() << "MOV_r_M\n";
        }
        else if (op_code == InstructionSet::MOV_M_r)
        {
            auto &source = GetSourceRegister(op_code);
            WriteMemory(hl_, source);

            Logger::Instance() << "MOV_M_r\n";
        }
        else // MOV_r1_r2
        {
            auto &destination = GetDestinationRegister(op_code);
            auto &source = GetSourceRegister(op_code);

            destination = source;

            Logger::Instance() << "MOV_r1_r2\n";
        }
    }
    else if (op_code == InstructionSet::MVI_r)
    {
        if (op_code == InstructionSet::MVI_M)
        {
            uint8_t immediate = ReadImmediate();
            WriteMemory(hl_, immediate);

            Logger::Instance() << "MVI_M\n";
        }
        else // MVI_r
        {
            auto &destination = GetDestinationRegister(op_code);
            destination = ReadImmediate();

            Logger::Instance() << "MIV_r\n";
        }
    }
    else if (op_code == InstructionSet::LXI)
    {
        auto &destination = GetRegisterPair(op_code);
        destination = ReadImmediateDWord();

        Logger::Instance() << "LXI\n";
    }
    else if (op_code == InstructionSet::LDAX)
    {
        if (op_code == InstructionSet::LDA)
        {
            uint16_t immediate = ReadImmediateDWord();
            a_ = ReadMemory(immediate);
        }
        else if (op_code == InstructionSet::LHLD)
        {
            uint16_t immediate = ReadImmediateDWord();
            l_ = ReadMemory(immediate);
            h_ = ReadMemory(++immediate);

            Logger::Instance() << "LHLD: " << l_ << " <- (" << immediate - 1 << ") && " << h_ << " <- (" << immediate << ")\n";
        }
        else // LDAX
        {
            auto &source = GetRegisterPair(op_code);

            a_ = ReadMemory(source);

            Logger::Instance() << "LDAX: " << a_ << " <- (" << source << ")\n";
        }
    }
    else if (op_code == InstructionSet::STAX)
    {
        if (op_code == InstructionSet::STA)
        {
            uint16_t immediate = ReadImmediateDWord();
            WriteMemory(immediate, a_);

            Logger::Instance() << "STA: (" << int(immediate) << ")" << a_ << "\n";
        }
        else if (op_code == InstructionSet::SHLD)
        {
            uint16_t immediate = ReadImmediateDWord();
            WriteMemory(immediate, l_);
            WriteMemory(++immediate, h_);

            Logger::Instance() << "SHLD: (" << immediate - 1 << ") <- " << l_ << " && (" << immediate << ")" << h_ << "\n";
        }
        else // STAX
        {
            auto &destination = GetRegisterPair(op_code);
            WriteMemory(destination, a_);

            Logger::Instance() << "STAX: (" << destination << ") <- " << a_ << "\n";
        }
    }
    else if (op_code == InstructionSet::XCHG)
    {
        swap(h_, d_);
        swap(l_, e_);

        Logger::Instance() << "XCHG\n";
    }
    else if (op_code == InstructionSet::ADD_r)
    {
        if (op_code == InstructionSet::ADD_M)
        {
            uint8_t memory = ReadMemory(hl_);
            ADD(memory);
        }
        else // ADD_r
        {
            auto &source = GetSourceRegister(op_code);
            ADD(source);
        }
    }
    else if (op_code == InstructionSet::ADI)
    {
        uint8_t immediate = ReadImmediate();
        ADD(immediate);
    }
    else if (op_code == InstructionSet::ADC_r)
    {
        if (op_code == InstructionSet::ADC_M)
        {
            uint8_t memory = ReadMemory(hl_);
            ADC(memory);
        }
        else // ADC_r
        {
            auto &source = GetSourceRegister(op_code);
            ADC(source);
        }
    }
    else if (op_code == InstructionSet::ACI)
    {
        uint8_t immediate = ReadImmediate();
        ADC(immediate);
    }
    else if (op_code == InstructionSet::SUB_r)
    {
        if (op_code == InstructionSet::SUB_M)
        {
            uint8_t memory = ReadMemory(hl_);
            SUB(memory);
        }
        else // SUB_r
        {
            auto &source = GetSourceRegister(op_code);
            SUB(source);
        }
    }
    else if (op_code == InstructionSet::SUI)
    {
        uint8_t immediate = ReadImmediate();
        SUB(immediate);
    }
    else if (op_code == InstructionSet::SBB_r)
    {
        if (op_code == InstructionSet::SBB_M)
        {
            uint8_t memory = ReadMemory(hl_);
            SBB(memory);
        }
        else // SBB_r
        {
            auto &source = GetSourceRegister(op_code);
            SBB(source);
        }
    }
    else if (op_code == InstructionSet::SBI)
    {
        uint8_t immediate = ReadImmediate();
        SBB(immediate);
    }
    else if (op_code == InstructionSet::INR_r)
    {
        if (op_code == InstructionSet::INR_M)
        {
            uint8_t memory = ReadMemory(hl_);
            uint8_t result = INR(memory);
            WriteMemory(hl_, result);

            Logger::Instance() << "INR_M\n";
        }
        else // INR_R
        {
            auto &destination = GetDestinationRegister(op_code);
            uint8_t result = INR(destination);
            destination = result;

            Logger::Instance() << "INR_r\n";
        }
    }
    else if (op_code == InstructionSet::DCR_r)
    {
        if (op_code == InstructionSet::DCR_M)
        {
            uint8_t memory = ReadMemory(hl_);
            uint8_t result = DCR(memory);
            WriteMemory(hl_, result);

            Logger::Instance() << "DCR_M\n";
        }
        else // DCR_R
        {
            auto &destination = GetDestinationRegister(op_code);
            uint8_t result = DCR(destination);
            destination = result;

            Logger::Instance() << "DCR_r\n";
        }
    }
    else if (op_code == InstructionSet::INX)
    {
        auto &destination = GetRegisterPair(op_code);
        ++destination;

        Logger::Instance() << "INX\n";
    }
    else if (op_code == InstructionSet::DCX)
    {
        auto &destination = GetRegisterPair(op_code);
        --destination;

        Logger::Instance() << "DCX\n";
    }
    else if (op_code == InstructionSet::DAD)
    {
        auto &source = GetRegisterPair(op_code);

        uint32_t temp = hl_ + source;
        flags_.carry = temp > std::numeric_limits<uint16_t>::max();
        hl_ = static_cast<uint16_t>(temp);

        Logger::Instance() << "DAD\n";
    }
    else if (op_code == InstructionSet::DAA)
    {
        uint8_t correction = 0;
        if (((a_ & 0b1111) > 9) || flags_.auxiliary_carry)
        {
            correction += 6;
        }

        if (((((a_ + correction) & 0b1111'0000) >> 4) > 9) || flags_.carry)
        {
            correction += (6 << 4);
        }

        ADD(correction);
    }
    else if (op_code == InstructionSet::ANA_r)
    {
        if (op_code == InstructionSet::ANA_M)
        {
            uint8_t memory = ReadMemory(hl_);
            ANA(memory);

            Logger::Instance() << "ANA_M\n";
        }
        else // ANA_r
        {
            auto &source = GetSourceRegister(op_code);
            ANA(source);

            SetAuxiliaryCarryFlag(a_ << 1); // The 8080 logical AND instructions set the flag to reflect the logical OR of bit 3 of the values involved in the AND operation.

            Logger::Instance() << "ANA_r\n";
        }
    }
    else if (op_code == InstructionSet::ANI)
    {
        uint8_t immediate = ReadImmediate();
        ANA(immediate);

        flags_.auxiliary_carry = false;
    }
    else if (op_code == InstructionSet::XRA_r)
    {
        if (op_code == InstructionSet::XRA_M)
        {
            uint8_t memory = ReadMemory(hl_);
            XRA(memory);

            Logger::Instance() << "XRA_M\n";
        }
        else // XRA_r
        {
            auto &source = GetSourceRegister(op_code);
            XRA(source);

            Logger::Instance() << "XRA_r\n";
        }
    }
    else if (op_code == InstructionSet::XRI)
    {
        uint8_t immediate = ReadImmediate();
        XRA(immediate);

        Logger::Instance() << "XRI\n";
    }
    else if (op_code == InstructionSet::ORA_r)
    {
        if (op_code == InstructionSet::ORA_M)
        {
            uint8_t memory = ReadMemory(hl_);
            ORA(memory);

            Logger::Instance() << "ORA_M\n";
        }
        else // ORA_r
        {
            auto &source = GetSourceRegister(op_code);
            ORA(source);

            Logger::Instance() << "ORA_r\n";
        }
    }
    else if (op_code == InstructionSet::ORI)
    {
        uint8_t immediate = ReadImmediate();
        ORA(immediate);

        Logger::Instance() << "ORI\n";
    }
    else if (op_code == InstructionSet::CMP_r)
    {
        if (op_code == InstructionSet::CMP_M)
        {
            uint8_t memory = ReadMemory(hl_);
            CMP(memory);

            Logger::Instance() << "CMP_M\n";
        }
        else // CMP_r
        {
            auto &source = GetSourceRegister(op_code);
            CMP(source);

            Logger::Instance() << "CMP_r\n";
        }
    }
    else if (op_code == InstructionSet::CPI)
    {
        uint8_t immediate = ReadImmediate();
        CMP(immediate);

        Logger::Instance() << "CPI " << a_ << " " << +immediate << "\n";
    }
    else if (op_code == InstructionSet::RLC)
    {
        uint16_t temp = a_ << 1;
        SetCarryFlag(temp);
        a_ = static_cast<uint8_t>(temp | uint8_t(flags_.carry));

        Logger::Instance() << "RLC\n";
    }
    else if (op_code == InstructionSet::RRC)
    {
        flags_.carry = a_ & 1;
        a_ = static_cast<uint8_t>((uint8_t(flags_.carry) << 7) | (a_ >> 1));

        Logger::Instance() << "RRC\n";
    }
    else if (op_code == InstructionSet::RAL)
    {
        bool old_carry = flags_.carry;
        uint16_t temp = a_ << 1;
        SetCarryFlag(temp);
        a_ = static_cast<uint8_t>(temp | uint8_t(old_carry));

        Logger::Instance() << "RAL\n";
    }
    else if (op_code == InstructionSet::RAR)
    {
        bool new_carry = a_ & 1;
        a_ = static_cast<uint8_t>((uint8_t(flags_.carry) << 7 | (a_ >> 1)));
        flags_.carry = new_carry;

        Logger::Instance() << "RAR\n";
    }
    else if (op_code == InstructionSet::CMA)
    {
        a_ = ~a_;

        Logger::Instance() << "CMA\n";
    }
    else if (op_code == InstructionSet::CMC)
    {
        flags_.carry = !flags_.carry;

        Logger::Instance() << "CMC\n";
    }
    else if (op_code == InstructionSet::STC)
    {
        flags_.carry = true;

        Logger::Instance() << "STC\n";
    }
    else if (op_code == InstructionSet::JMP || op_code == InstructionSet::JC)
    {
        uint16_t immediate = ReadImmediateDWord(); // has to be done unconditionally to move program_counter_ correctly
        if (op_code == InstructionSet::JC && !CheckCondition(op_code))
        {
            Logger::Instance() << "JC skipped\n";
            return;
        }

        program_counter_ = immediate;

        Logger::Instance() << (op_code == InstructionSet::JMP ? "JMP" : "JC")
                           << "\n";
    }
    else if (op_code == InstructionSet::CALL || op_code == InstructionSet::CC)
    {
        uint16_t immediate = ReadImmediateDWord(); // has to be done unconditionally to move program_counter_ correctly
        if (op_code == InstructionSet::CC && !CheckCondition(op_code))
        {
            Logger::Instance() << "CC skipped\n";
            return;
        }

        Push(program_counter_);
        program_counter_ = immediate;

        Logger::Instance() << (op_code == InstructionSet::CALL ? "CALL" : "CC")
                           << "\n";
    }
    else if (op_code == InstructionSet::RET || op_code == InstructionSet::RC)
    {
        if (op_code == InstructionSet::RC && !CheckCondition(op_code))
        {
            Logger::Instance() << "RET skipped\n";
            return;
        }

        Pop(program_counter_);

        Logger::Instance() << (op_code == InstructionSet::RET ? "RET" : "RC")
                           << "\n";
    }
    else if (op_code == InstructionSet::RST)
    {
        Push(program_counter_);
        program_counter_ = GetInterruptAddress(op_code);

        Logger::Instance() << "RST\n";
    }
    else if (op_code == InstructionSet::PCHL)
    {
        program_counter_ = hl_;

        Logger::Instance() << "PCHL\n";
    }
    else if (op_code == InstructionSet::PUSH_rp)
    {

        if (op_code == InstructionSet::PUSH_PSW)
        {
            uint8_t status = static_cast<uint8_t>((uint8_t(flags_.sign) << 7) | (uint8_t(flags_.zero) << 6) | (uint8_t(flags_.auxiliary_carry) << 4) | (uint8_t(flags_.parity) << 2) | (1 << 1) | uint8_t(flags_.carry));
            Push(a_);
            Push(status);

            Logger::Instance() << "PUSH_PSW\n";
        }
        else // PUSH_rp
        {
            auto &source = GetRegisterPair(op_code);
            Push(source);

            Logger::Instance() << "PUSH_rp\n";
        }
    }
    else if (op_code == InstructionSet::POP_rp)
    {
        if (op_code == InstructionSet::POP_PSW)
        {
            uint8_t status;
            Pop(status);
            Pop(a_);

            flags_.carry = status & 0b0000'0001;
            flags_.parity = status & 0b0000'0100;
            flags_.auxiliary_carry = status & 0b0001'0000;
            flags_.zero = status & 0b0100'0000;
            flags_.sign = status & 0b1000'0000;

            Logger::Instance() << "POP_PSW\n";
        }
        else // POP_rp
        {
            auto &destination = GetRegisterPair(op_code);
            Pop(destination);

            Logger::Instance() << "POP_rp\n";
        }
    }
    else if (op_code == InstructionSet::XTHL)
    {
        RegisterPair temp("temp");
        temp = hl_;
        hl_.low_ = ReadMemory(stack_pointer_);
        WriteMemory(stack_pointer_, temp.low_);

        hl_.high_ = ReadMemory(stack_pointer_ + 1);
        WriteMemory(stack_pointer_ + 1, temp.high_);

        Logger::Instance() << "XTHL\n";
    }
    else if (op_code == InstructionSet::SPHL)
    {
        stack_pointer_ = hl_;

        Logger::Instance() << "SPHL\n";
    }
    else if (op_code == InstructionSet::IN)
    {
        switch (ReadImmediate())
        {
        case 1:
        {
            a_ = 0b0000'0000;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                a_ = a_ | 0b0000'00001;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
            {
                a_ = a_ | 0b0000'00100;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                a_ = a_ | 0b0000'10000;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                a_ = a_ | 0b0001'00000;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                a_ = a_ | 0b0010'00000;
            }
        }
        break;
        case 2:
        {
            a_ = 0b0000'0000;
        }
        break;
        case 3:
        {
            a_ = static_cast<uint8_t>(shift_ >> (CHAR_BIT - shift_offset_));
        }
        break;
        default:
        {
            throw std::runtime_error("CPU::ExecuteInstruction(): IN: unsupported Port.");
        }
        break;
        }

        Logger::Instance() << "IN\n";
    }
    else if (op_code == InstructionSet::OUT)
    {
        switch (ReadImmediate())
        {
        case 2:
        {
            shift_offset_ = a_ & 0b0000'0111;
        }
        break;
        case 3:
        {
            // sound related
        }
        break;
        case 4:
        {
            shift_.low_ = shift_.high_;
            shift_.high_ = a_;
        }
        break;
        case 5:
        {
            // sound related
        }
        break;
        case 6:
        {
            // watchdog
        }
        break;
        default:
        {
            throw std::runtime_error("CPU::ExecuteInstruction(): OUT: unsupported Port.");
        }
        break;
        }

        Logger::Instance() << "OUT\n";
    }
    else if (op_code == InstructionSet::EI)
    {
        std::scoped_lock lock(interrupt_mutex_);
        interrupts_enabled_ = true;
        interrupt_requested_ = false;

        Logger::Instance() << "EI\n";
    }
    else if (op_code == InstructionSet::DI)
    {
        std::scoped_lock lock(interrupt_mutex_);
        interrupts_enabled_ = false;

        Logger::Instance() << "DI\n";
    }
    else if (op_code == InstructionSet::NOP)
    {
        Logger::Instance() << "NOP\n";
    }
    else
    {
        throw std::logic_error("CPU::ExecuteInstruction(): Unhandled Instruction.");
    }
}

inline void CPU::ADD(uint8_t value, bool carry) noexcept
{
    auto [result, carry_per_bit] = Addition(a_, value, carry);
    SetAllFlags(result, carry_per_bit);
    a_ = static_cast<uint8_t>(result);
}

inline void CPU::ADC(uint8_t value) noexcept
{
    ADD(value, flags_.carry);
}

inline void CPU::SUB(uint8_t value, bool carry) noexcept
{
    auto [result, carry_per_bit] = Subtraction(a_, value, carry);
    SetAllFlags(result, carry_per_bit);
    a_ = static_cast<uint8_t>(result);
}

inline void CPU::SBB(uint8_t value) noexcept
{
    SUB(value, flags_.carry);
}

inline uint8_t CPU::INR(uint8_t value) noexcept
{
    auto [result, carry_per_bit] = Addition(value, 1);
    SetNonCarryFlags(result);
    SetAuxiliaryCarryFlag(carry_per_bit);

    return static_cast<uint8_t>(result);
}

inline uint8_t CPU::DCR(uint8_t value) noexcept
{
    auto [result, carry_per_bit] = Subtraction(value, 1);
    SetNonCarryFlags(result);
    SetAuxiliaryCarryFlag(carry_per_bit);

    return static_cast<uint8_t>(result);
}

inline void CPU::ANA(uint8_t value) noexcept
{
    a_ = a_ & value;
    SetNonCarryFlags(a_);

    flags_.carry = false;
}

inline void CPU::XRA(uint8_t value) noexcept
{
    a_ = a_ ^ value;
    SetNonCarryFlags(a_);

    flags_.carry = false;
    flags_.auxiliary_carry = false;
}

inline void CPU::ORA(uint8_t value) noexcept
{
    a_ = a_ | value;
    SetNonCarryFlags(a_);

    flags_.carry = false;
    flags_.auxiliary_carry = false;
}

inline void CPU::CMP(uint8_t value) noexcept
{
    auto [result, carry_per_bit] = Subtraction(a_, value);
    SetSignFlag(result);
    SetParityFlag(result);
    SetAuxiliaryCarryFlag(carry_per_bit);
    flags_.zero = (a_ == value);
    flags_.carry = (a_ < value);
}

inline uint8_t CPU::ReadImmediate()
{
    return ReadMemory(program_counter_++);
}

inline uint16_t CPU::ReadImmediateDWord()
{
    uint8_t low = ReadImmediate();
    uint8_t high = ReadImmediate();
    return static_cast<uint16_t>((high << CHAR_BIT) | low);
}

inline uint8_t CPU::ReadMemory(uint16_t address)
{
    return memory_.Read(address);
}

inline void CPU::WriteMemory(uint16_t address, uint8_t data)
{
    memory_.Write(address, data);
}

inline void CPU::Push(const RegisterPair &rp)
{
    Push(rp.high_);
    Push(rp.low_);
}

inline void CPU::Push(uint8_t value)
{
    WriteMemory(--stack_pointer_, value);
}

inline void CPU::Pop(Register &value)
{
    value = ReadMemory(stack_pointer_++);
}

inline void CPU::Pop(uint8_t &value)
{
    value = ReadMemory(stack_pointer_++);
}

inline void CPU::Pop(RegisterPair &rp)
{
    Pop(rp.low_);
    Pop(rp.high_);
}

inline Register &CPU::GetSourceRegister(uint8_t op_code) noexcept
{
    return GetRegister(op_code & 0b0000'0111);
}

inline Register &CPU::GetDestinationRegister(uint8_t op_code) noexcept
{
    return GetRegister((op_code & 0b0011'1000) >> 3);
}

Register &CPU::GetRegister(uint8_t register_code) noexcept
{
    assert(register_code <= 0b111 && register_code != 0b110);

    switch (register_code)
    {
    case 0b111:
    {
        return a_;
    }
    break;
    case 0b000:
    {
        return b_;
    }
    case 0b001:
    {
        return c_;
    }
    case 0b010:
    {
        return d_;
    }
    case 0b011:
    {
        return e_;
    }
    case 0b100:
    {
        return h_;
    }
    case 0b101:
    {
        return l_;
    }
    break;
    }

    std::terminate();
}

RegisterPair &CPU::GetRegisterPair(uint8_t op_code) noexcept
{
    switch ((op_code & 0b0011'0000) >> 4)
    {
    case 0b00:
    {
        return bc_;
    }
    break;
    case 0b01:
    {
        return de_;
    }
    case 0b10:
    {
        return hl_;
    }
    case 0b11:
    {
        return stack_pointer_;
    }
    break;
    }

    std::terminate(); // satisfy compiler (unreachable)
}

bool CPU::CheckCondition(uint8_t op_code) const noexcept
{
    switch ((op_code & 0b0011'1000) >> 3)
    {
    case 0b000: // non zero
    {
        return !flags_.zero;
    }
    break;
    case 0b001: // zero
    {
        return flags_.zero;
    }
    break;
    case 0b010: // no carry
    {
        return !flags_.carry;
    }
    break;
    case 0b011: // carry
    {
        return flags_.carry;
    }
    break;
    case 0b100: // odd parity
    {
        return !flags_.parity;
    }
    break;
    case 0b101: // even parity
    {
        return flags_.parity;
    }
    break;
    case 0b110: // positive
    {
        return !flags_.sign;
    }
    case 0b111: // negative
    {
        return flags_.sign;
    }
    break;
    default:
    {
        std::terminate();
    }
    break;
    }
}

inline void CPU::SetAllFlags(uint16_t result, uint16_t carry_per_bit) noexcept
{
    SetNonCarryFlags(result);
    SetCarryFlags(carry_per_bit);
}

inline void CPU::SetNonCarryFlags(uint16_t result) noexcept
{
    SetZeroFlag(result);
    SetSignFlag(result);
    SetParityFlag(result);
}

inline void CPU::SetZeroFlag(uint16_t temp) noexcept
{
    flags_.zero = static_cast<uint8_t>(temp) == 0;
}

inline void CPU::SetSignFlag(uint16_t temp) noexcept
{
    flags_.sign = temp & 0b1000'0000;
}

inline void CPU::SetParityFlag(uint16_t temp) noexcept
{
    int sum_of_bits = 0;
    for (int i = 0; i < CHAR_BIT; ++i)
    {
        if (temp & (1 << i))
        {
            ++sum_of_bits;
        }
    }

    flags_.parity = (sum_of_bits % 2) == 0;
}

inline void CPU::SetCarryFlags(uint16_t carry_per_bit) noexcept
{
    SetCarryFlag(carry_per_bit);
    SetAuxiliaryCarryFlag(carry_per_bit);
}

inline void CPU::SetCarryFlag(uint16_t carry_per_bit) noexcept
{
    flags_.carry = carry_per_bit & 0b1'0000'0000;
}

inline void CPU::SetAuxiliaryCarryFlag(uint16_t carry_per_bit) noexcept
{
    flags_.auxiliary_carry = carry_per_bit & 0b0001'0000;
}

inline uint16_t CPU::GetInterruptAddress(uint8_t op_code) const noexcept
{
    return op_code & 0b0011'1000;
}
