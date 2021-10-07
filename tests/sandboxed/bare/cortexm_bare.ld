OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

_rom_start_addr = 0x0;
_rom_length = 1024;
_ram_start_addr = 0x3000000;
_ram_length = 4096;

PHDRS
{
  code         PT_LOAD;
  data         PT_LOAD;
}

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
    } > rom :code

   /*------------------------------------------------------------- */
   /* PLT section contains code for accessing the dynamically linked functions
    * this measn functions from shared libraries in a position independent manner */
   .plt : ALIGN(4)
   {
       *(.plt)
       . = ALIGN(4);
   } >rom :code


    . = ALIGN(4);
    _etext = .;

    .relocate :
    {
        . = ALIGN(4);
        _srelocate = .;
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        KEEP (*(SORT(.xfa.*)))
        . = ALIGN(4);
        _erelocate = .;
    } > ram AT> rom :data

   /* The global offset table is the table for indirectly accessing the global variables
    * The table contains addresses of the global variables. The text section contains 
    * a address of the GOT base and a offset in it to access the appropriate variables.
    * This is done to access the variables in a position independent manner. */
   .got : ALIGN(4)
   {
       _sgot = .;
       *(.got)
   } >ram AT> rom :data

   /* got.plt section contains entries which is used with the PLT to access the functions 
    * in a position independent manner. */
   .got.plt : ALIGN(4)
   {
        _sgot_plt = .;
       *(.got.plt)
       _edata = .;
   } >ram AT> rom :data
/*------------------------------------------------------------- */

    /* .bss section which is used for uninitialized data */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = . ;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = . ;
    } > ram :data

    /*
     * collect all uninitialized sections that go into RAM
     */
    .noinit (NOLOAD) :
    {
        __noinit_start = .;
        *(.noinit)
        . = ALIGN(4);
        __noinit_end = .;
    }  > ram :data

    /* heap section */
    . = ALIGN(4);
    _sheap = . ;
    _eheap = ORIGIN(ram) + LENGTH(ram);

    /* Populate information about ram size */
    _sram = ORIGIN(ram);
    _eram = ORIGIN(ram) + LENGTH(ram);
}
