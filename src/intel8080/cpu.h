#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <atomic>
#include <mutex>

#include "register.h"
#include "memory.h"

class CPU final
{
public:
    CPU();

    virtual ~CPU();

    template <typename MemoryType, typename... Args>
    requires std::is_base_of_v<MemoryInterface, MemoryType>
    void AddMemory(Args &&...args)
    {
        memory_.AddMemory<MemoryType>(std::forward<Args>(args)...);
    }

    void Run();

    void Stop() noexcept;

    void Interrupt(uint8_t interrupt) noexcept;

private:
    struct
    {
        bool carry = false;
        bool parity = false;
        bool auxiliary_carry = false;
        bool zero = false;
        bool sign = false;
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

    uint8_t shift_offset_{0};
    RegisterPair shift_{"Shift"};

    Memory memory_;

    std::atomic<bool> executing_{false};

    std::mutex interrupt_mutex_;
    bool interrupts_enabled_{false};
    bool interrupt_requested_{false};
    uint8_t interrupt_;

    inline uint8_t FetchInstruction();

    void ExecuteInstruction(uint8_t op_code);

    inline void ADD(uint8_t value, bool carry = false) noexcept;

    inline void ADC(uint8_t value) noexcept;

    inline void SUB(uint8_t value, bool carry = false) noexcept;

    inline void SBB(uint8_t value) noexcept;

    inline uint8_t INR(uint8_t value) noexcept;

    inline uint8_t DCR(uint8_t value) noexcept;

    inline void ANA(uint8_t value) noexcept;

    inline void XRA(uint8_t value) noexcept;

    inline void ORA(uint8_t value) noexcept;

    inline void CMP(uint8_t value) noexcept;

    inline uint8_t ReadImmediate();

    inline uint16_t ReadImmediateDWord();

    inline uint8_t ReadMemory(uint16_t address);

    inline void WriteMemory(uint16_t address, uint8_t data);

    inline void Push(const RegisterPair &rp);

    inline void Push(uint8_t value);

    inline void Pop(Register &value);

    inline void Pop(uint8_t &value);

    inline void Pop(RegisterPair &rp);

    inline Register &GetSourceRegister(uint8_t op_code) noexcept;

    inline Register &GetDestinationRegister(uint8_t op_code) noexcept;

    Register &GetRegister(uint8_t register_code) noexcept;

    RegisterPair &GetRegisterPair(uint8_t op_code) noexcept;

    inline uint16_t GetInterruptAddress(uint8_t op_code) const noexcept;

    bool CheckCondition(uint8_t op_code) const noexcept;

    inline void SetAllFlags(uint16_t result, uint16_t carry_per_bit) noexcept;

    inline void SetNonCarryFlags(uint16_t result) noexcept;

    inline void SetZeroFlag(uint16_t temp) noexcept;

    inline void SetSignFlag(uint16_t temp) noexcept;

    inline void SetParityFlag(uint16_t temp) noexcept;

    inline void SetCarryFlags(uint16_t carry_per_bit) noexcept;

    inline void SetCarryFlag(uint16_t carry_per_bit) noexcept;

    inline void SetAuxiliaryCarryFlag(uint16_t carry_per_bit) noexcept;
};

#endif /* CPU_H */
