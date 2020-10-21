void libc_init(void) {
#if MODULE_NEWLIB || MODULE_PICOLIBC
    /* initialize std-c library (this must be done after board_init) */
    extern void __libc_init_array(void);
    __libc_init_array();
#endif
}
