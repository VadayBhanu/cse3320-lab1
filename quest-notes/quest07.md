I tested the kernel debug-level system using mbox.c, kernel/Makefile, and debug.h.

The local debug level in mbox.c can be controlled using macros such as K2_DEBUG_INFO and K2_DEBUG_WARN.

The global debug level is controlled by CONFIG_GLOBAL_DEBUG_LEVEL in kernel/Makefile.

I temporarily changed mbox.c from K2_DEBUG_INFO to K2_DEBUG_WARN, rebuilt the kernel, and booted it in QEMU.

With the warning level enabled, the informational mailbox output was suppressed while the kernel continued to boot successfully.

After testing, I restored mbox.c to K2_DEBUG_INFO.
