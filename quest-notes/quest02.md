# Quest 02 - Kernel Image Inspection

## 1. What is an ELF file?

ELF stands for Executable and Linkable Format. It is a standard format used to store executable machine code, symbols, sections, debugging information, and other metadata.

My kernel ELF file was identified as:

ELF 64-bit LSB executable, ARM aarch64, statically linked, with debug_info, not stripped.

## 2. What are ELF sections?

ELF sections organize different types of program data.

Examples include:

- `.text` - executable machine instructions
- `.data` - initialized global/static variables
- `.bss` - uninitialized global/static variables
- symbol/debug sections - information used by tools such as GDB

## 3. What are EL1, EL2, and EL3?

AArch64 provides different exception or privilege levels.

- EL1 - operating system/kernel level
- EL2 - hypervisor level
- EL3 - secure monitor level

This kernel primarily operates at EL1.

## 4. Address of kernel_main

Using:

`aarch64-linux-gnu-nm kernel/build-rpi3qemu/kernel8.elf | grep kernel_main`

I found:

`kernel_main = 0x809d0`

## 5. First instructions at kernel_main

Using objdump, I found:

`0x809d0: a9bf7bfd    stp x29, x30, [sp, #-16]!`

`0x809d4: 910003fd    mov x29, sp`

These are the first two AArch64 instructions in kernel_main.

## 6. First 8 bytes

The first 8 bytes are:

`fd 7b bf a9 fd 03 00 91`

The system is little-endian, so the byte order in memory is the reverse byte order of the displayed 32-bit instruction words.

## 7. Size of an AArch64 instruction

Each AArch64 instruction is 4 bytes, or 32 bits.

Therefore, the first 8 bytes contain two instructions.

## 8. ELF to raw kernel image

The build process converts `kernel8.elf` into the raw bootable kernel image using objcopy with binary output.

The ELF contains sections, symbols, and debug information, while the raw `.img` contains the bytes that are loaded for execution.

## 9. Location of kernel_main bytes in kernel image

The kernel is loaded starting at:

`0x80000`

The address of kernel_main is:

`0x809d0`

Therefore:

`0x809d0 - 0x80000 = 0x9d0`

So the first bytes of kernel_main appear at file offset:

`0x9d0`

I verified this using:

`xxd -g 1 -s 0x9d0 -l 8 kernel/kernel8-rpi3qemu.img`

Output:

`000009d0: fd 7b bf a9 fd 03 00 91`
