include Makefile.inc

KERNEL=kernel.bin

SOURCES=$(wildcard *.c)
SOURCES_ASM=$(wildcard asm/*.asm)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_NATIVE=$(wildcard native/*.c)
SOURCES_IDT=$(wildcard idt/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
OBJECTS_NATIVE=$(SOURCES_NATIVE:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
TOTAL_OBJ = $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_ASM) $(OBJECTS_NATIVE) $(OBJECTS_IDT)

LOADERSRC=loader.asm
LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

$(KERNEL): $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf $(LOADEROBJECT) $(STATICLIBS) $(TOTAL_OBJ)
%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o drivers/*.o *.o *.bin
	rm -rf idt/*.o drivers/*.o *.o *.bin
	rm -rf drivers/*.o drivers/*.o *.o *.bin
	rm -rf native/*.o drivers/*.o *.o *.bin

.PHONY: all clean
