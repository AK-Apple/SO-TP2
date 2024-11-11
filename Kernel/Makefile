include Makefile.inc

KERNEL=kernel.bin

SRC_DIRS = . drivers native idt heap tests processes
ASM_DIRS = asm

SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
SOURCES_ASM = $(wildcard $(ASM_DIRS)/*.asm)
OBJECTS = $(SOURCES:.c=.o) $(SOURCES_ASM:.asm=.o)

TOTAL_OBJ = $(OBJECTS)


LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../Shared $(MMAN) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf $(TOTAL_OBJ) $(LOADEROBJECT) *.elf *.bin

.PHONY: all clean
