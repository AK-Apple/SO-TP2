#ifndef REGISTERS_SNAPSHOT_H
#define REGISTERS_SNAPSHOT_H

#include <stddef.h>
#include <stdint.h>
#include "shared.h"

void take_registers_snapshot(uint64_t rsp);

void print_registers(StackedRegisters regs);

void get_latest_registers(StackedRegisters *regs);

#endif
