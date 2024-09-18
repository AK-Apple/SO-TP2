#include <stdint.h>
#include "video.h"
#include "exceptions.h"
#include "IO.h"
#include "lib.h"
#include "naiveConsole.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 1
#define REG_SIZE 18

static void zero_division();
static void invalid_opcode();
static void showRegisters(const uint64_t regex[18]);

static const char* registerNames[18] = {
        "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15", "RIP", "RFLAGS"
};

static const char* zero_division_exception_message = "Error Divide by Zero Exception";

static const char* invalid_opcode_exception_message = "Error Invalid Opcode Exception";

void exceptionDispatcher(int exception, const uint64_t regex[18]) {
    sys_clearScreen();
    if (exception == ZERO_EXCEPTION_ID){
        zero_division();
    }
    else if (exception == INVALID_OPCODE_ID){
        invalid_opcode();
    }
    showRegisters(regex);
}

static void zero_division() {
    sys_write(2 ,zero_division_exception_message, buflen(zero_division_exception_message));
    printf("\n");
}

static void invalid_opcode() {
    sys_write(2, invalid_opcode_exception_message, buflen(invalid_opcode_exception_message));
    printf("\n");
}

static void showRegisters(const uint64_t regex[18]) {
    char *regs[REG_SIZE] = {
            "RAX", "RBX", "RCX", "RDX", "RDI", "RSI", "RBP", "RSP",
            "R08", "R09", "R10", "R11", "R12", "R13", "R14",
            "R15", "IP ", "RFLAGS"};

    char toPrint[18];
    for (int i = 0; i < REG_SIZE; i++) {
        printf(regs[i]);
        int digits = uintToBase(regex[i], toPrint, 16);
        printf(" : 0x");
        int zeros = 15;
        while (zeros > digits) {
            printf("0");
            zeros--;
        }
        printf(toPrint);
        printf("\n");
    }
}