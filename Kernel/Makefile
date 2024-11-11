include Makefile.inc

KERNEL=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_NATIVE=$(wildcard native/*.c)
SOURCES_IDT=$(wildcard idt/*.c)
SOURCES_HEAP=$(wildcard heap/*.c)		# NEW (Added in SO)
SOURCES_TESTS=$(wildcard tests/*.c)
SOURCES_PROCESSES=$(wildcard processes/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_NATIVE=$(SOURCES_NATIVE:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
OBJECTS_HEAP=$(SOURCES_HEAP:.c=.o) 		# NEW (Added in SO)
OBJECTS_TESTS=$(SOURCES_TESTS:.c=.o)
OBJECTS_PROCESSES=$(SOURCES_PROCESSES:.c=.o)
TOTAL_OBJ = $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_ASM) $(OBJECTS_NATIVE) $(OBJECTS_IDT) $(OBJECTS_HEAP) $(OBJECTS_TESTS) $(OBJECTS_PROCESSES)

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
