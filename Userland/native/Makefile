include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard *.c)
SOURCES_EXEC=$(wildcard exec/*.c)
SOURCES_LIBC=$(wildcard libc/*.c)
SOURCES_ASM=$(wildcard libasm/*.asm)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_EXEC=$(SOURCES_EXEC:.c=.o)
OBJECTS_LIBC=$(SOURCES_LIBC:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)

TOTAL_OBJ= $(OBJECTS) $(OBJECTS_ASM) $(OBJECTS_EXEC) $(OBJECTS_LIBC)

all: $(MODULE)

$(MODULE): $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(TOTAL_OBJ) -o ../$(MODULE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../0000-sampleCodeModule.elf $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o exec/*.o libasm/*.o libc/*.o

.PHONY: all clean print
