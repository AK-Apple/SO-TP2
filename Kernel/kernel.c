#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <idtLoader.h>
#include "fl_allocator.h"
#include "IO.h"
#include "test_mm.h"
#include "scheduling.h"
#include "test_util.h"

extern void test_int_80h();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

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
    initialize_allocator(sampleDataModuleAddress + userlandModuleSize);

    return getStackBase();
}

int main()
{
    printf("Voy a crear algo\n");
    create_init_process();
    printf("init creado\n");
    load_idt();

    printf("iniciando test de procesos\n");
    test_processes();

    printf("test finalizado\n");

    get_all_processes();

    // while(1);
    // sys_registers();     // descomentar para ver registros del kernel

    ((EntryPoint)sampleCodeModuleAddress)();

    while (1)
        ;

    return 0;
}