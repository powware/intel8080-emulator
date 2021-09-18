#ifndef CPU_H
#define CPU_H

#include "register.h"

class CPU
{
public:
    CPU();
    ~CPU();

private:
    Register a_;
    Register b_;
    Register c_;
    Register d_;
    Register e_;
    Register f_;
    Register g_;
    Register h_;
    Register l_;
    Register status_;
    Register stack_pointer_;
    Register program_counter_;
};

#endif /* CPU_H */
