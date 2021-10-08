OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

_rom_start_addr = 0x0;
_rom_length = 1024;
_ram_start_addr = 0x3000000;
_ram_length = 4096;

MEMORY
{
    rom      (rx)   : ORIGIN = _rom_start_addr, LENGTH = _rom_length
    ram      (w!rx) : ORIGIN = _ram_start_addr, LENGTH = _ram_length
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;
        *(.crt)
        *(.text .text.* .gnu.linkonce.t.*)
        *(.rodata .rodata* .gnu.linkonce.r.*)
        KEEP (*(SORT(.roxfa.*)))

        . = ALIGN(4);
        _efixed = .;            /* End of text section */
    } > rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;

    _srelocate = .;
    .relocate :
    {
        . = ALIGN(4);
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        KEEP (*(SORT(.xfa.*)))
        . = ALIGN(4);
    } > ram AT> rom
    _erelocate = LOADADDR(.relocate);

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > ram

    /*
     * collect all uninitialized sections that go into RAM
     */
    .noinit (NOLOAD) :
    {
        __noinit_start = .;
        *(.noinit)
        . = ALIGN(4);
        __noinit_end = .;
    }  > ram

    /* heap section */
    . = ALIGN(4);
    _sheap = . ;
    _eheap = ORIGIN(ram) + LENGTH(ram);

    /* Populate information about ram size */
    _sram = ORIGIN(ram);
    _eram = ORIGIN(ram) + LENGTH(ram);
}
