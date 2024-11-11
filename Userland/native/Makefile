include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin

# List of directories with source files
SRC_DIRS = . exec libc
ASM_DIRS = libasm

# Collect source files
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
SOURCES_ASM = $(wildcard $(ASM_DIRS)/*.asm)

# Convert source files to object files
OBJECTS = $(SOURCES:.c=.o) $(SOURCES_ASM:.asm=.o)

# Total object files
TOTAL_OBJ = $(OBJECTS)


all: $(MODULE)

$(MODULE): $(TOTAL_OBJ)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld $(TOTAL_OBJ) -o ../$(MODULE)
	$(LD) $(LDFLAGS) -T sampleCodeModule.ld --oformat=elf64-x86-64 -o ../0000-sampleCodeModule.elf $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -I../../Shared -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(TOTAL_OBJ)

.PHONY: all clean print
