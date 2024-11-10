#include "registers.h"
#include "IO.h"
#include "shared.h"

StackedRegisters registers = {0};

void take_registers_snapshot(uint64_t rsp) {
    registers = *(StackedRegisters *)rsp;
}

void print_registers(StackedRegisters regs) {
    printf("\nRAX  : 0x%16lx", regs.rax);
    printf("\nRBX  : 0x%16lx", regs.rbx); 
    printf("\nRCX  : 0x%16lx", regs.rcx); 
    printf("\nRDX  : 0x%16lx", regs.rdx); 
    printf("\nRDI  : 0x%16lx", regs.rdi); 
    printf("\nRSI  : 0x%16lx", regs.rsi); 
    printf("\nRBP  : 0x%16lx", regs.rbp); 
    printf("\nRSP  : 0x%16lx", regs.rsp); 
    printf("\nR08  : 0x%16lx", regs.r8);
    printf("\nR09  : 0x%16lx", regs.r9);
    printf("\nR10  : 0x%16lx", regs.r10); 
    printf("\nR11  : 0x%16lx", regs.r11); 
    printf("\nR12  : 0x%16lx", regs.r12); 
    printf("\nR13  : 0x%16lx", regs.r13); 
    printf("\nR14  : 0x%16lx", regs.r14); 
    printf("\nR15  : 0x%16lx", regs.r15); 
    printf("\nRIP  : 0x%16lx", regs.rip); 
    printf("\nFLAG : 0x%16lx", regs.rflags);
    printf("\nCS   : 0x%16lx", regs.cs);
    printf("\nSS   : 0x%16lx", regs.ss);
    printf("\n");
}

void get_latest_registers(StackedRegisters *regs) {
    *regs = registers;
}