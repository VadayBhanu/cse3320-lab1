# Quest 06 - Framebuffer and OS Logo

I booted the Lab 1 kernel using QEMU on the rpi3qemu platform.

The framebuffer initialized successfully and displayed the UVA OS logo at 320x240 resolution.

Because the graphical QEMU window was unstable under WSL, I used the QEMU monitor `screendump` command to capture the framebuffer directly.

The attached image verifies successful framebuffer output.
