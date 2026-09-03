// #define K2_DEBUG_VERBOSE
#define K2_DEBUG_WARN

#include <stddef.h>
#include <stdint.h>

#include "plat.h"
#include "utils.h"
#include "sched.h"

extern void test_ktimer();     // unittests.c
extern void test_fb_voffset(); // unittests.c
extern void donut();           // donut.c
extern void donut_simple();    // donut.c
extern void donut_text();      // donut.c

void uart_send_string(char* str);

struct cpu cpus[NCPU];

void kernel_main() {
	uart_init();                       // bring up the UART for kernel debugging
	init_printf(NULL, putc);           // wire printf() to output via UART (putc)
	printf("------ kernel boot ------  core %d\n\r", cpuid());
	printf("build time (kernel.c) %s %s\n", __DATE__, __TIME__);

	sys_timer_init();                  // kernel timer: delay, timekeeping...
	enable_interrupt_controller(0);    // coreid

	// Quest 09: enable CPU IRQ
	enable_irq();

	generic_timer_init();              // periodic ticks alive

	if (fb_init() != 0) BUG();         // will show the OS logo

	// test_ktimer();

	// Quest 08 disabled so execution can continue
	// test_fb_voffset();              // cycle through color quads

	// donut();                        // uses virtual timer for animation

	// Quest 10: pixel donut
	donut_simple();

	// donut_text();                   // textual UART donut animation

	while (1)
		asm volatile("wfi");
}
