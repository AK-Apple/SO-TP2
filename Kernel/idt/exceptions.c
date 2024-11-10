// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "video.h"
#include "exceptions.h"
#include "IO.h"
#include "lib.h"
#include "registers.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 1

static void zero_division();
static void invalid_opcode();

static const char* zero_division_exception_message = "Divide by Zero Exception\n";

static const char* invalid_opcode_exception_message = "Invalid Opcode Exception\n";

void exceptionDispatcher(int exception, StackedRegisters* stack_pointer) {
    sys_clearScreen();
    if (exception == ZERO_EXCEPTION_ID){
        zero_division();
    }
    else if (exception == INVALID_OPCODE_ID){
        invalid_opcode();
    }
    take_registers_snapshot((uint64_t)stack_pointer);
    print_registers(*stack_pointer);
    printf_error("[kernel] Killing process with pid=%d because it caused an exception.\n", get_pid());
}

static void zero_division() {
    sys_write(2, zero_division_exception_message, buflen(zero_division_exception_message));
}

static void invalid_opcode() {
    sys_write(2, invalid_opcode_exception_message, buflen(invalid_opcode_exception_message));
}