# Quest 03 - GDB Kernel Debugging

I used GDB with QEMU to inspect the kernel boot process.

## EL2 to EL1

Before `eret`:

- PC: `0x80034`
- Exception level: EL2

After executing `eret`:

- PC: `0x8005c`
- Location: `el1_entry`
- Exception level: EL1

This confirms the switch from EL2 to EL1.

## kernel_main

I set a breakpoint at `kernel_main` at address `0x809d0`.

I stepped through:

- `uart_init()`
- `init_printf(NULL, putc)`

and reached the first `printf()` at line 24 of `kernel.c`.

The screenshots show the GDB debugging results.
