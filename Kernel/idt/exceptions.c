// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "video.h"
#include "exceptions.h"
#include "IO.h"
#include "lib.h"
#include "scheduling.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 1

static void zero_division();
static void invalid_opcode();

static const char* zero_division_exception_message = "Error Divide by Zero Exception";

static const char* invalid_opcode_exception_message = "Error Invalid Opcode Exception";

void exceptionDispatcher(int exception, StackedRegisters* stack_pointer) {
    sys_clearScreen();
    if (exception == ZERO_EXCEPTION_ID){
        zero_division();
    }
    else if (exception == INVALID_OPCODE_ID){
        invalid_opcode();
    }

    print_regs(*stack_pointer);
}

static void zero_division() {
    sys_write(2 ,zero_division_exception_message, buflen(zero_division_exception_message));
    printf("\n");
}

static void invalid_opcode() {
    sys_write(2, invalid_opcode_exception_message, buflen(invalid_opcode_exception_message));
    printf("\n");
}