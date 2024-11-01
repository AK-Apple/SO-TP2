// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include "memory_allocator.h"
#include "IO.h"
#include "scheduling.h"
#include "interrupts.h"
#include "defs.h"
#include "keyboard.h"

#define MB (1L << 20)

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

#define SAMPLE_CODE_MODULE_ADDRESS  ((void *)0x400000)
#define SAMPLE_DATA_MODULE_ADDRESS ((void *)0x500000)
#define MEMORY_COUNT_MB_ADDRESS  ((uint32_t *)(SYS_VARIABLES + SYS_MEM_COUNT))

void clearBSS(void *bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
                    - sizeof(uint64_t)                    // Begin at the top of the stack
    );
}

void *initializeKernelBinary()
{
    void *moduleAddresses[] = {
        SAMPLE_CODE_MODULE_ADDRESS,
        SAMPLE_DATA_MODULE_ADDRESS};

    uint64_t userlandModuleSize = loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);
    uint64_t allocator_capacity = *MEMORY_COUNT_MB_ADDRESS * MB - (uint64_t)(SAMPLE_DATA_MODULE_ADDRESS + userlandModuleSize);
    initialize_memory_allocator(SAMPLE_DATA_MODULE_ADDRESS + userlandModuleSize, allocator_capacity);
    initialize_keyboard_driver();
    return getStackBase();
}

int main()
{
    create_init_process();
    load_idt();
    static char *argv_shell[] = {"shell"};
    static int kernel_restart_count = 0;
    if(kernel_restart_count) {
        char dummy_buffer[32] = {0};
        printf_color("Kernel iniciado %d veces. Presiona cualquier tecla para continuar...", 0x00FFCC00, kernel_restart_count);
        sys_read(0, dummy_buffer, 1);
        sys_clearScreen();
    }
    int fds[] = {STDIN, STDOUT, STDERR};
    create_process((Program)SAMPLE_CODE_MODULE_ADDRESS, sizeof(argv_shell)/sizeof(argv_shell[0]), argv_shell, fds);
    kernel_restart_count++;
    while (1) {
        _hlt();
        yield();
    }

    return 0;
}