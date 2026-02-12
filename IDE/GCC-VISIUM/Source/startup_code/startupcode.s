!*
!*   Assembler start-up preamble code.
!*
!*   �2007 Goodrich Control Systems Limited. Copyright in this document belongs to
!*   Goodrich Control Systems Limited and all rights are reserved. No reproduction
!*   of all or part of this document shall be made without the prior written consent
!*   of Goodrich Control Systems Limited. This document contains information that
!*   may be confidential and its disclosure to others requires the written consent
!*   of Goodrich Control Systems Limited.
!*
!*   Build Version       Author          Date                    Change Reference
!*   1.2                 M.E.Bradbury    24-Apr-2007             1. Allow use of a C boot_functions so mitigate
!*                                                                  stack pointer alteration by re-initialising it
!*                                                                  and providing a global 'return to' point.
!*                                                               2. Uncommented code lines required for call to
!*                                                                  boot_functions.s
!*   1.1                 M.E.Bradbury    27-Mar-2007             1. WDT buffer removed (now a C declaration).
!*                                                               2. Call to '_boot_functions' added.
!*                                                               3. Stack pointer initialised first to match
!*                                                                  assembler start-up scheme ('gnat_ada_start.s').
!*                                                                  Ref: Review Report TBD
!*   1.0                 M.E.Bradbury    18-Jul-2006             1. Original
!*
!*
!*
!* CM INFORMATION:
!*   CM FILE NAME:-               %name: startupcode.s %
!*   CM FILE VERSION:-            %version: 3 %
!*   CM FILE INSTANCE:-           %instance: 2 %
!*   CM ORIGINAL CHECK-IN DATE:-  %date_created: Tue Apr 24 14:46:38 2007 %
!*   CM ORIGINAL CHECK-IN BY:-    %created_by: asc %
!*   CM MODIFIED CHECK-IN DATE:-  %date_modified: %
!*   CM MODIFIED CHECK-IN BY:-    %derived_by: asc %
!*
!*
!*   NB Since this routine is to be used prior to the elaboration process, care
!*   must be taken to ensure no declarations use the .bss and .data areas.
!*
!*   This routine is NOT intended to be altered by the user since it forms
!*   part of the "generic" set of start-up and scheduler code.
!*

!  Constant table containing various definitions of memory areas for passing
!  to the C start-up code by mapping a structure over it.

	.rdata
	.global	_address_array		! Make this array visible
	.global	_endram			! Make end of RAM address accessible
	.align	4
_address_array:				! Format to match that in C startup callee
	.long	__initial_data_start	! Start of constant values in ROM
	.long	__data_start		! Destination for constant values in RAM
	.long	__data_end		! End of destn for constant values in RAM
	.long	__bss_start		! Start of BSS area in RAM
_endram:.long	__bss_end		! End of BSS area in RAM


!  Assembler start-up code - this is the preamble to the C
!  start-up code and is best kept to a minimum, but nevertheless
!  some operations are best done here before launching the C.
!  Stack area tests must be done before any stack is used and
!  given the availability of an assembler function which has
!  close control over register usage, it's that which is used.
!  It's convenient to do the system memory tests here too.
!  User memory is tested elsewhere (but before elaboration);
!  depending on the amount to test, interrupts need to be
!  serviced during the test.

	.section .program, "x"
	.align  4
	.global _start
	.global	_ret_to_startup
	.global _exit

_start:
	moviu   r23,%u __stack_top	    ! Initialize the stack pointer
	movil   r23,%l __stack_top      ! (allows boot_functions to be written in C)

_ret_to_startup:			        ! Address to use if return may be invalid
	moviu   r23,%u __stack_top	    ! Re-initialize the stack pointer
	movil   r23,%l __stack_top	    ! (boot_functions may have changed it)

	moviu	r1,%u __sysmem_start	! Start of system RAM
	movil	r1,%l __sysmem_start
	moviu	r2,%u __sysmem_end	    ! End of system RAM
	moviu	r3,%u _tstrama		    ! Test routine
	movil	r3,%l _tstrama
	bra	tr,r3,r21
	movil	r2,%l __sysmem_end	    ! LSW of end of system RAM
	and.l	r0,r1,r1		        ! Check test result
	brr	ne,_exit		            ! Handle test failure

	moviu   r1,%u __stack_vital	    ! Low address for stack
	movil   r1,%l __stack_vital
	moviu   r2,%u __stack_top	    ! Initial position of stack
	moviu	r3,%u _tstrama		    ! Test routine
	movil	r3,%l _tstrama
	bra	tr,r3,r21
	movil   r2,%l __stack_top	    ! LSW of initial position of stack
	and.l	r0,r1,r1		        ! Check test result
	brr	ne,_exit		            ! Handle test failure

	moviu	r1,%u _entry_point	    ! Progress with the C start-up code
	movil	r1,%l _entry_point
	bra	tr,r1,r21
	move.l  r22,r0			        ! Terminate the backtrace

_exit:
	moviu	r1,%u _error_handler	! Invoke the (user defined) error handler
	movil	r1,%l _error_handler
	bra	tr,r1,r0
	nop

	.end
