// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include "memory_allocator.h"
#include "IO.h"
#include "process.h"
#include "interrupts.h"
#include "defs.h"
#include "keyboard.h"
#include "scheduler.h"
#include "pipes.h"

#define MB (1L << 20)

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PAGE_SIZE = 0x1000;

#define SAMPLE_CODE_MODULE_ADDRESS  ((void *)0x400000)
#define SAMPLE_DATA_MODULE_ADDRESS ((void *)0x500000)
#define MEMORY_COUNT_MB_ADDRESS  ((uint32_t *)(SYS_VARIABLES + SYS_MEM_COUNT))

void clearBSS(void *bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void *get_stack_base()
{
    return (void *)((uint64_t)&endOfKernel + PAGE_SIZE * 8 // The size of the stack itself, 32KiB
                    - sizeof(uint64_t)                    // Begin at the top of the stack
    );
}

void *initialize_kernel_binary()
{
    void *module_addresses[] = {
        SAMPLE_CODE_MODULE_ADDRESS,
        SAMPLE_DATA_MODULE_ADDRESS};

    uint64_t userland_module_size = loadModules(&endOfKernelBinary, module_addresses);

    clearBSS(&bss, &endOfKernel - &bss);
    uint64_t allocator_capacity = *MEMORY_COUNT_MB_ADDRESS * MB - (uint64_t)(SAMPLE_DATA_MODULE_ADDRESS + userland_module_size);
    initialize_memory_allocator(SAMPLE_DATA_MODULE_ADDRESS + userland_module_size, allocator_capacity);
    return get_stack_base();
}

int main()
{
    create_init_process();
    load_idt();
    initialize_pipes();
    static char *argv_shell[] = {"shell"};

    fd_t fds[] = {STDIN, STDOUT, STDERR};
    sys_clear_screen();
    pid_t shell_pid = create_process((Program)SAMPLE_CODE_MODULE_ADDRESS, sizeof(argv_shell)/sizeof(argv_shell[0]), argv_shell, fds);
    set_foreground(shell_pid);

    while (1) {
        _hlt();
        yield();
    }

    return 0;
}