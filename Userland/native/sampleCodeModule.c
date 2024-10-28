// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/shell.h"
#include "../include/stdio.h"
#include "../include/syscalls.h"

static int var1 = 0;
static int var2 = 0;

int main() {
    shell();

    if (var1 == 0 && var2 == 0)
        return 0xDEADC0DE;

    return 0xDEADBEEF;
}
