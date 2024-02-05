OUTPUT_ARCH( "riscv" )

SECTIONS
{
  . = 0x00001000;
  .text : {
    *(.text._start)
    *(.text)
  }
  . = ALIGN(0x1000);
  .rodata :
  {
    *(.rdata)
    *(.rodata)
  }
  .data : { *(.data) }
  .debug : { *(.debug) }

  . = ALIGN(0x1000);
  .bss : { *(.bss) }

  . = ALIGN(0x1000);
  .malloc_zone :
  {
    __malloc_start = .;
    . = . + 128*4096;
    __malloc_zone_stop = .;
  }
  _end = .;
}
