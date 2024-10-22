#include "../include/shell.h"
#include "../include/stdio.h"
#include "../include/syscalls.h"

static int var1 = 0;
static int var2 = 0;

void shell_process() {
    shell();
}

int main() {
    char *shell_args[] = {"shell"};
    sys_create_process((Program)shell_process, sizeof(shell_args)/sizeof(shell_args[0]), shell_args);

    while(1) {
        sys_yield();
    }

    if (var1 == 0 && var2 == 0)
        return 0xDEADC0DE;

    return 0xDEADBEEF;
}
