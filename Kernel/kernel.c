#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include "memory_allocator.h"
#include "IO.h"
#include "scheduling.h"
#include "interrupts.h"
#include "defs.h"

#define MB (1L << 20)

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;
static uint32_t *const memory_count_mb_address = (uint32_t *) (SYS_VARIABLES + SYS_MEM_COUNT);

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
        sampleCodeModuleAddress,
        sampleDataModuleAddress};

    uint64_t userlandModuleSize = loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);
    uint64_t allocator_capacity = *memory_count_mb_address * MB - (uint64_t)(sampleDataModuleAddress + userlandModuleSize);
    initialize_memory_allocator(sampleDataModuleAddress + userlandModuleSize, allocator_capacity);

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
        while(sys_read(0, dummy_buffer, 8) == 0) ;
        sys_clearScreen();
    }
    create_process((Program)sampleCodeModuleAddress, sizeof(argv_shell)/sizeof(argv_shell[0]), argv_shell);
    kernel_restart_count++;
    while (1)
        yield();

    return 0;
}