/*----------------------------------------------------------------------------------------------------------------------
                           Copyright 2015 Rolls-Royce Controls and Data Services Limited.
      The information in this document is the property of Rolls-Royce Controls and Data Services Limited and may not
	be copied or communicated to a third party, or used for any purpose other than that for which it is supplied without
	                the express written consent of Rolls-Royce Controls and Data Services Limited.
-----------------------------------------------------------------------------------------------------------------------
*//**

   \file target_box_unit_gr6.ld
   Modified version of the visiumcore_linker_script.ld - Modified memory regions to utilise DV/Trims and Calibration
   regions of ROM in support of AdaTest95 instrumented code as these are not used for HLRT/LLT. Added additional
   sections required to support TargetRunner's modified libc target-to-host I/O emulation layer.

*//*
----------------------------------------------------------------------------------------------------------------------
   Change History:
   0.0.1 - Simon Daniel
   Modified memory layout to increase ROM region to accommodate AdaTest95 instrumented code
   Added heap start and end addresses to BSS section to support TargetRunner's modified ramprep routine
   Moved Development Variables to single ROM memory region to free some additional ROM for AdaTest95
   Included C code constructor and destructor sections to support TargetRunner's modified libc
   Included C code initialisation and finalisation routines section to support the above
   Moved all libc .rodata sections to ROM memory region as they are used within startup code routines
----------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------------------
                             Copyright 2015 Rolls-Royce Controls and Data Services Limited.
    The information in this document is the property of Rolls-Royce Controls and Data Services Limited and may not be
  copied or communicated to a third party, or used for any purpose other than that for which it is supplied without the
                       express written consent of Rolls-Royce Controls and Data Services Limited.
-----------------------------------------------------------------------------------------------------------------------
  $ExportControlStatement: EU Export Controlled 9E003.h.1 and 9E003.h.3 $
-----------------------------------------------------------------------------------------------------------------------
  Unit Details:
  
     $Id: linker_script_visiumcore_elf.ld 1.2 2015/05/13 13:42:47BST Daniel, Simon (SIM140) InWork  $
  
  Type:
 
     $Setting ArtefactType: ???? $
 
----------------------------------------------------------------------------------------------------------------------
  Current Status:
          $Revision: 1.2 $
          $Date: 2015/05/13 13:42:47BST $
          $Author: Daniel, Simon (SIM140) $
          $State: InWork $
 
----------------------------------------------------------------------------------------------------------------------
  Change History:
          $Log: linker_script_visiumcore_elf.ld  $
          Revision 1.2 2015/05/13 13:42:47BST Daniel, Simon (SIM140) 
          Bug fix to prevent data sections being placed in RAM
----------------------------------------------------------------------------------------------------------------------
                                                                   
  Description:                                                     
          This file is intended to act as a generic main linker script for projects using the VISIUMCORE processor.
          Not all program sections will be required for every project, but all projects should link without changes.
          Addresses can be altered to match the intended memory configuration of the project.
-------------------------------------------------------------------------------------------------------------------- */

/* Startup entry point */
ENTRY(_start)

/* Memory layout */
MEMORY
{
    /* Values for TargetRunner LLT/HLRT test build:
    * The size of PROM on the VISIUMCORE is 8MB and split into two defined memory sections:
    *     Bootstrap ROM         : Roughly 128KB : Dummy allocation for bootstrap program code
    *     ROM                   : Roughly 8MB   : Contains program code and read-only data for startup code
    * The size of RAM on the VISIUMCORE is 4MB and is split between three defined memory sections:
    *     System Memory : Spans 352B   : Contains context block RAM for use by the watchdog timer tasks
    *     Bootstrap RAM : Spans 32B    : Dummy allocation for bootstrap RAM (e.g. reset counter)
    *     RAM           : Roughly 4MB : Contains interrupt handler RAM, reset safe RAM, read-only data, data, BSS and heap
    */
    rom     : ORIGIN = 0x00020000, LENGTH = 8M - 0x00020000
    sysm    : ORIGIN = 0xE0000000, LENGTH = 0x00000160
    bootram : ORIGIN = 0xE0000160, LENGTH = 0x00000020
    stubram : ORIGIN = 0xE01F0000, LENGTH = 0x00010000
    ram     : ORIGIN = 0xE0000180, LENGTH = 4M - LENGTH(sysm) - LENGTH(bootram)  - LENGTH(stubram)
}

/* Section placement and ordering */
SECTIONS
{
/* The startup entry point is in .program, which must start at address
 * 0x00021000, as this is the target of the jump from the bootstrap.
 */
    
    /* Start of test build program code */
    .text 0x00021000 :
    {
        __start_of_prom  = .;
        /* Startup code */
        *(.program)
        *(.text.*)
        *(.text)		/* Added by SGH */
        . = ALIGN(4) ;
    } > rom

    /* C code initialisation and finalisation routines */
    /* These are required to call C code constructors and destructors */
    .init :
    {
        KEEP(*(.init))
        KEEP(*(.fini))
        _einit = .;
    } > rom

	/* C code constructors section */
    .ctors :
    {
        . = ALIGN(4);
        __CTOR_LIST__ = .;
        KEEP(*crtbegin*.o(.ctors))
        KEEP(*(EXCLUDE_FILE(*crtend*.o).ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        __CTOR_END__ = .;
    } > rom

	/* C code destructors section */
    .dtors :
    {
        __DTOR_LIST__ = .;
        KEEP(*crtbegin*.o(.dtors))
        KEEP(*(EXCLUDE_FILE(*crtend*.o).dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        __DTOR_END__ = .;
    } > rom

    /* Now reserve some space for constant data in ROM */
    .romdata : ALIGN(4)
    {
        /* For the VISIUMCORE, constant data is made to reside in the .ramdata sub section (RAM) */
        /* The constant data for the RAM initialisation routine in startup code, must however, reside in ROM */
        *startupcode.o(.rodata)
        *startupcode.o(.rodata.*)
        *memcpy.o(.rodata)
        *memcpy.o(.rodata.*)
        *memset.o(.rodata)
        *memset.o(.rodata.*)
	*lib_a-*.o(.rodata)
	*lib_a-*.o(.rodata.*)
    } AT> rom

/* Development Variables Sections */
    
    /* Store DVs in separate area for use with Trim Tool  */
    .dvconst :
    {
        __start_of_trims  = .;
        _sdvconst = ABSOLUTE(.);
          *(.dvconst)
        _edvconst = ABSOLUTE(.);
    } AT> rom

    /* Store Tables in separate area for use with Trim Tool  */
    .tblconst :
    {
        _stblconst = ABSOLUTE(.);
          *(.tblconst)
        _etblconst = ABSOLUTE(.);
    } AT> rom

    /* Store Carpets in separate area for use with Trim Tool  */
    .cptconst :
    {
        _scptconst = ABSOLUTE(.);
          *(.cptconst)
        _ecptconst = ABSOLUTE(.);
    } AT> rom

    /* Store Piles in separate area for use with Trim Tool  */
    .pilconst :
    {
        _spilconst = ABSOLUTE(.);
        *(.pilconst)
        _epilconst = ABSOLUTE(.);
        /* Variables declared with initialisation values have their values stored here */
        . = ALIGN(4);
        __initial_data_start   =  .;
    } AT> rom

/* RAM Memory Sections */

    /* System memory is the context block and located at the bottom of RAM.
     * It includes data that is essential for the operation of the watchdog timer tasks.
     * Labels are exported for the start and end addresses for access by test routines.
     */
    .sysmem (NOLOAD) : ALIGN(4)
    {
        __sysmem_start = .;
        /* SRC and watch dog timer data buffer */
        *(.wdtvars)
        /* Aligned to 32 longwords as required by ramclearblock startup routine */
        . = ALIGN(4*32);
        __sysmem_end = .;
    } AT> sysm

    /* Reset Safe RAM memory is located straight after system memory.
     * This section is defined separately to ensure startup code doe not automatically clear this section of RAM.
     * Labels are exported to the Reset Safe RAM routines.
     */
    .ResetSafeRAM (NOLOAD) : ALIGN(4)
    {
        __rsr_start = .;
        *(.ResetSafeRAM)
        __rsr_end = .;
    } AT> ram
   
    /* Inverse Reset Safe RAM memory is located straight after Reset Safe RAM memory.
     * This section is used for routines that rely on inverse data as opposed to CRC for data integrity verification.
     * Labels are exported to the Reset Safe RAM routines.
     */
    .InverseResetSafeRAM (NOLOAD) : ALIGN(4)
    {
        __inverse_rsr_start = .;
        *(.InverseResetSafeRAM)
        __inverse_rsr_end = .;
    } AT> ram

    /* Interrupt Handler RAM locations that are to be initialised to zero.
     * This section is defined separately to exclude it from the main PUC memory check routines.
     * The start and end addresses are exported to the IHBSS clear and PUC test routines.
     */
    .ih_bss (NOLOAD) : ALIGN(4)
    {
        __ram_test_start = .;
        __s_ihbss = .;
        *(.ih_bss)
        . = ALIGN(4);
        __e_ihbss = .;
    } AT> ram
 
    /* Program section containing variables declared with initialisation values.
     * They are relocated here and the initial values copied from ROM during RAM initialisation.
     * Note: ALIGN(4) is used to ensure .ramdata begins on a longword boundary.
     * __data_start must reside at the start to allow startup code to copy the data from ROM (__initial_data_start) to RAM correctly.
     */
    .ramdata : AT (__initial_data_start) ALIGN(4)
    {
        __data_start = . ;
        /* For the VISIUMCORE, constant data (.rodata items) are made to reside in the .ramdata sub section */
        /* This ensures constant data resides in RAM and helps avoid burst mode penalties */
        *(.rodata)
        *(.rodata.*)
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        __data_end = . ;
    } > ram
 
    /* RAM locations that are to be initialised to zero */
	/* Heap start and heap end addresses are exported to the startup ramprep routine */
    .bss : ALIGN(4)
    {
        __bss_start = . ;
        *(.bss)
        *(.bss.*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        __bss_end = .;
        __ram_test_end = .;
        _sheap = .;
	    _end = .;
	    PROVIDE(end = .);
        . = . + 1M;
        _eheap = .;
        . = ALIGN(4);
    } AT> ram
    
/* Stack Sizing Labels and Testing */

   /* RAM Start address set to the first location in RAM memory */
   _RAM_Start = ORIGIN(sysm);

   /* RAM Size set to the entire size of System Memory and RAM */
   _RAM_Size = LENGTH(sysm) + LENGTH(ram);

    /* RAM End calculated from RAM Start address and RAM Size */
    _RAM_End = _RAM_Start + _RAM_Size;

    /* Locate the top of the stack just beyond the end of RAM for a pre-decrement stack */
    __stack_top    = _RAM_End;
    __end_of_prom  = __initial_data_start + SIZEOF(.ramdata) + 8;
    __end_of_trims = __start_of_trims + SIZEOF(.dvconst) + SIZEOF(.tblconst) + SIZEOF(.cptconst) + SIZEOF(.pilconst) + 8;

    /* Define a lower limit on the address range to be checked at the beginning of the startup sequence */
    __stack_vital  = _RAM_End - 64K;

    /* Ensure that stack space does not overrun */
    ASSERT( __bss_end < __stack_vital , "error, insufficient stack space")

/* Debug Information
 * Symbols in the debugging sections are relative to the beginning of the section so begin them at 0
 */

    /* STAB Debug Sections */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .comment 0 : { *(.comment) }

    /* DWARF 1.1 and DWARF 2 Debug Sections */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    /* DWARF 2 Debug Sections */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* DWARF 3 Debug Sections */
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF 4 Debug Sections */
    .debug_types    0 : { *(.debug_types) }
}
