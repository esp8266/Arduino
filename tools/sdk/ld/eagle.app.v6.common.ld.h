/* This linker script generated from xt-genldscripts.tpp for LSP . */
/* Linker Script for ld -N */

PHDRS
{
  dport0_0_phdr PT_LOAD;
  dram0_0_phdr PT_LOAD;
  dram0_0_bss_phdr PT_LOAD;
  iram1_0_phdr PT_LOAD;
  irom0_0_phdr PT_LOAD;
}


/*  Default entry point:  */
ENTRY(app_entry)
EXTERN(_DebugExceptionVector)
EXTERN(_DoubleExceptionVector)
EXTERN(_KernelExceptionVector)
EXTERN(_NMIExceptionVector)
EXTERN(_UserExceptionVector)
EXTERN(core_version)
PROVIDE(_memmap_vecbase_reset = 0x40000000);
/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x00000110;
_memmap_cacheattr_wt_base = 0x00000110;
_memmap_cacheattr_bp_base = 0x00000220;
_memmap_cacheattr_unused_mask = 0xFFFFF00F;
_memmap_cacheattr_wb_trapnull = 0x2222211F;
_memmap_cacheattr_wba_trapnull = 0x2222211F;
_memmap_cacheattr_wbna_trapnull = 0x2222211F;
_memmap_cacheattr_wt_trapnull = 0x2222211F;
_memmap_cacheattr_bp_trapnull = 0x2222222F;
_memmap_cacheattr_wb_strict = 0xFFFFF11F;
_memmap_cacheattr_wt_strict = 0xFFFFF11F;
_memmap_cacheattr_bp_strict = 0xFFFFF22F;
_memmap_cacheattr_wb_allvalid = 0x22222112;
_memmap_cacheattr_wt_allvalid = 0x22222112;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

  .dport0.rodata : ALIGN(4)
  {
    _dport0_rodata_start = ABSOLUTE(.);
    *(.dport0.rodata)
    *(.dport.rodata)
    _dport0_rodata_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.literal : ALIGN(4)
  {
    _dport0_literal_start = ABSOLUTE(.);
    *(.dport0.literal)
    *(.dport.literal)
    _dport0_literal_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.data : ALIGN(4)
  {
    _dport0_data_start = ABSOLUTE(.);
    *(.dport0.data)
    *(.dport.data)
    _dport0_data_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)
    . = ALIGN(4);
    _Pri_3_HandlerAddress = ABSOLUTE(.);
    _data_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .noinit : ALIGN(4)
  {
    *(.noinit)
  } >dram0_0_seg :dram0_0_phdr
  
#ifdef VTABLES_IN_DRAM
#include "eagle.app.v6.common.ld.vtables.h"
#endif

  /* IRAM is split into .text and .text1 to allow for moving specific */
  /* functions into IRAM that would be matched by the irom0.text matcher */
  .text : ALIGN(4)
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
     *(.UserEnter.text)
    . = ALIGN(16);
    *(.DebugExceptionVector.text)
    . = ALIGN(16);
    *(.NMIExceptionVector.text)
    . = ALIGN(16);
    *(.KernelExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.UserExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.DoubleExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)

    /* all functional callers are placed in IRAM (including SPI/IRQ callbacks/etc) here */
    *(.text._ZNKSt8functionIF*EE*)  /* std::function<any(...)>::operator()() const */
  } >iram1_0_seg :iram1_0_phdr

  .irom0.text : ALIGN(4)
  {
    _irom0_text_start = ABSOLUTE(.);
    *(.ver_number)
    *.c.o( EXCLUDE_FILE (umm_malloc.c.o) .literal*, EXCLUDE_FILE (umm_malloc.c.o) .text* )
    *.cpp.o(.literal*, .text*)
#ifdef VTABLES_IN_FLASH
    *(.rodata._ZTV*) /* C++ vtables */
#endif

    *libgcc.a:unwind-dw2.o(.literal .text .rodata .literal.* .text.* .rodata.*)
    *libgcc.a:unwind-dw2-fde.o(.literal .text .rodata .literal.* .text.* .rodata.*)

    *libc.a:(.literal .text .literal.* .text.*)
    *libm.a:(.literal .text .literal.* .text.*)
    *libgcc.a:_umoddi3.o(.literal .text)
    *libgcc.a:_udivdi3.o(.literal .text)
    *libstdc++.a:( .literal .text .literal.* .text.*)
    *libstdc++-exc.a:( .literal .text .literal.* .text.*)
    *libsmartconfig.a:(.literal .text .literal.* .text.*)
    *liblwip_gcc.a:(.literal .text .literal.* .text.*)
    *liblwip_src.a:(.literal .text .literal.* .text.*)
    *liblwip2-536.a:(.literal .text .literal.* .text.*)
    *liblwip2-1460.a:(.literal .text .literal.* .text.*)
    *liblwip2-536-feat.a:(.literal .text .literal.* .text.*)
    *liblwip2-1460-feat.a:(.literal .text .literal.* .text.*)
    *liblwip6-536-feat.a:(.literal .text .literal.* .text.*)
    *liblwip6-1460-feat.a:(.literal .text .literal.* .text.*)
    *libbearssl.a:(.literal .text .literal.* .text.*)
    *libaxtls.a:(.literal .text .literal.* .text.*)
    *libat.a:(.literal.* .text.*)
    *libcrypto.a:(.literal.* .text.*)
    *libespnow.a:(.literal.* .text.*)
    *libjson.a:(.literal.* .text.*)
    *liblwip.a:(.literal.* .text.*)
    *libmesh.a:(.literal.* .text.*)
    *libnet80211.a:(.literal.* .text.*)
    *libsmartconfig.a:(.literal.* .text.*)
    *libssl.a:(.literal.* .text.*)
    *libupgrade.a:(.literal.* .text.*)
    *libwpa.a:(.literal.* .text.*)
    *libwpa2.a:(.literal.* .text.*)
    *libwps.a:(.literal.* .text.*)
    *(.irom0.literal .irom.literal .irom.text.literal .irom0.text .irom0.text.* .irom.text .irom.text.*)

    /* __FUNCTION__ locals */
    *(.rodata._ZZ*__FUNCTION__)
    *(.rodata._ZZ*__PRETTY_FUNCTION__)
    *(.rodata._ZZ*__func__)

    /* std::* exception strings, in their own section to allow string coalescing */
    *(.irom.exceptiontext)

    /* c++ typeof IDs, etc. */
    *(.rodata._ZTIN* .rodata._ZTSN10* .rodata._ZTISt* .rodata._ZTSSt*)

    /* Fundamental type info */
    *(.rodata._ZTIPKc .rodata._ZTIc .rodata._ZTIv .rodata._ZTSv .rodata._ZTSc .rodata._ZTSPKc .rodata._ZTSi .rodata._ZTIi)

    . = ALIGN(4);
    *(.gcc_except_table .gcc_except_table.*)
    . = ALIGN(4);
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;  /* Add a 0 entry to terminate the list */

    _irom0_text_end = ABSOLUTE(.);
    _flash_code_end = ABSOLUTE(.);
  } >irom0_0_seg :irom0_0_phdr



  .text1 : ALIGN(4)
  {
    *(.literal .text .iram.literal .iram.text .iram.text.* .literal.* .text.* .stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
#ifdef VTABLES_IN_IRAM
    *(.rodata._ZTV*) /* C++ vtables */
#endif
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >iram1_0_seg :iram1_0_phdr

#if defined(VTABLES_IN_IRAM) || defined(VTABLES_IN_FLASH)
#include "eagle.app.v6.common.ld.vtables.h"
#endif

  .lit4 : ALIGN(4)
  {
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
  } >iram1_0_seg :iram1_0_phdr


}

/* get ROM code address */
INCLUDE "../ld/eagle.rom.addr.v6.ld"
