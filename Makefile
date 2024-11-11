# Default MMAN
MMAN = DONT_USE_BUDDY_ALLOCATOR

# Standard targets
all: toolchain bootloader kernel userland image

buddy: 
	@$(MAKE) all MMAN=USE_BUDDY_ALLOCATOR

toolchain:
	cd Toolchain; make all

bootloader: toolchain
	cd Bootloader; make all

kernel: toolchain
	cd Kernel; make all MMAN=-D$(MMAN)

userland: toolchain
	cd Userland; make all

image: toolchain kernel bootloader userland
	cd Image; make all

clean:
	cd Toolchain; make clean
	cd Bootloader; make clean
	cd Image; make clean
	cd Kernel; make clean
	cd Userland; make clean

.PHONY: all clean buddy toolchain bootloader kernel userland image

