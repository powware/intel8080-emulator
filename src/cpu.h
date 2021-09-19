#ifndef CPU_H
#define CPU_H

#include <thread>
#include <cassert>

#include "register.h"
#include "instruction.h"

#include "ram.h"
#include "rom.h"
#include "memory_map.h"

enum OpCode : uint8_t
{
    MOV = 0b0100'0000,
    MOV_r_M = 0b0100'1100,
    MOV_M_r = 0b0111'0000

};

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
    BC = 00,
    DE = 01,
    HL = 10,
    SP = 11
};

void HexDump(const MemoryMap &memory)
{
    const char *numbers = "0123456789ABCDEF";
    for (uint16_t i = 0; i < static_cast<uint16_t>(memory.GetSize()); i += 0x10)
    {
        std::cout << numbers[(i & 0x0F000) >> (3 * 4)];
        std::cout << numbers[(i & 0x00F00) >> (2 * 4)];
        std::cout << numbers[(i & 0x000F0) >> (1 * 4)];
        std::cout << numbers[(i & 0x0000F)];

        for (uint16_t j = 0; j < 0xF; j++)
        {
            uint8_t value;
            if (!memory.Read(i + j, value))
            {
                return;
            }

            std::cout << " ";
            std::cout << numbers[(value & 0xF0) >> (1 * 4)];
            std::cout << numbers[(value & 0x0F)];
        }

        std::cout << std::endl;
    }
}

class CPU final
{
public:
    CPU() {}

    virtual ~CPU() {}

    void AddROM(std::filesystem::path &&path)
    {
        memory_map_.AddMemory<ROM>(std::forward<std::filesystem::path>(path));
    }

    void AddRAM(std::size_t size)
    {
        memory_map_.AddMemory<RAM>(size);
    }

    void StartExecution()
    {
        HexDump(memory_map_);
        //execution_thread_ = std::thread([this]()
        //                                {
        while (true)
        {
            if (!FetchInstruction())
            {
                std::cout << "instruction invalid or not supported\n";
            }
        }
        //                                });
    }

private:
    RegisterPair astatus_{"A", "Status"};
    RegisterPair bc_{"B", "C"};
    RegisterPair de_{"D", "E"};
    RegisterPair hl_{"H", "L"};

    Register16 stack_pointer_{"Stack Pointer"};
    Register16 program_counter_{"Program Counter"};

    MemoryMap memory_map_;

    std::thread execution_thread_;

    bool FetchInstruction()
    {
        uint8_t op_code;
        if (!memory_map_.Read(program_counter_.GetHighLow(), op_code))
        {
            return false;
        }

        if (op_code & MOV)
        {
            if (op_code & MOV_r_M)
            {
                Register8 &r = GetRegister8FromOpCode<2>(op_code);

                memory_map_.Write(hl_, r);

                program_counter_++;
            }
            else if (op_code & MOV_M_r)
            {
                Register8 &r = GetRegister8FromOpCode<5>(op_code);

                memory_map_.Read(hl_, r);

                program_counter_++;
            }
            else // MOV_r1_r2
            {
                Register8 &r1 = GetRegister8FromOpCode<2>(op_code);
                Register8 &r2 = GetRegister8FromOpCode<5>(op_code);

                r2 = r1;

                program_counter_++;
            }
        }
        else
        {
            program_counter_++;
        }

        return false;
    }

    template <uint8_t position>
    Register8 &GetRegister8FromOpCode(uint8_t op_code)
    {
        static_assert(position == 2 || position == 5);

        if constexpr (position == 2)
        {
            return GetRegister8((op_code & 0b0011'1000) >> 3);
        }
        else
        {
            return GetRegister8(op_code & 0b0000'0111);
        }
    }

    Register8 &GetRegister8(uint8_t register_code)
    {
        assert(register_code <= 0b111);
        assert(register_code != 0b110);

        switch (RegisterCode(register_code))
        {
        case RegisterCode::A:
        {
            return astatus_.GetHigh();
        }
        break;
        case RegisterCode::B:
        {
            return bc_.GetHigh();
        }
        case RegisterCode::C:
        {
            return bc_.GetLow();
        }
        case RegisterCode::D:
        {
            return de_.GetHigh();
        }
        case RegisterCode::E:
        {
            return de_.GetLow();
        }
        case RegisterCode::H:
        {
            return hl_.GetHigh();
        }
        case RegisterCode::L:
        {
            return hl_.GetLow();
        }
        break;
        default:
        {
            return astatus_.GetLow(); // needed to satisfy compiler
        }
        break;
        }
    }
};

#endif /* CPU_H */
