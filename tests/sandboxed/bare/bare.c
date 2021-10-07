void _start_c(void);

void __attribute((section(".crt"))) _start(void)
{
    _start_c();
}

void _start_c(void) {
    __asm__("svc 89");
}
